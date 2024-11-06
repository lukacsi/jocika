#include <concord/discord.h>
#include <concord/log.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// TEST SERVER ID
#define GUILD_ID 1300798374911414303
u64snowflake JOCI = 1300798374911414303;

// QUALTY MEMS ID
u64snowflake BOT_PARANCSOK = 298545786315079681;

// MY UID (ADMIN)
u64snowflake MY_USER_ID = 274276440642551818;

// COMMAND LEADER CHARACTER
const char LEADER = '!';

void on_ready(struct discord *client, const struct discord_ready *event) {
    log_info("Logged in as %s!", event->user->username);
    struct discord_create_guild_application_command ping = {
        .name = "ping",
        .description = "Ping command!"
    };
    struct discord_create_guild_application_command run = {
        .name = "run",
        .description = "Run a command on Joci."
    };
    discord_create_guild_application_command(client, event->application->id, GUILD_ID, &ping, NULL);
    discord_create_guild_application_command(client, event->application->id, GUILD_ID, &run, NULL);
}

void on_interaction(struct discord *client, const struct discord_interaction *event) {
    //if(event->type != DISCORD_INTERACTION_APPLICATION_COMMAND)
    //    return; /* return if interaction isn't a slash command */

    if(strcmp(event->data->name, "ping") == 0) {
        struct discord_interaction_response params = {
            .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
            .data = &(struct discord_interaction_callback_data){
                .content = "pong"
            }
        };
        discord_create_interaction_response(client, event->id, event->token, &params, NULL);
    }
//    } else if(strcmp(event->data->name, "run") == 0) {
//        if(!event->data || !event->data->values) {
//            log_info("No Data");
//            return;
//        }
//
//        char values[1024];
//        strings_to_json(values, sizeof(values), event->data->values);
//
//        char text[DISCORD_MAX_MESSAGE_LEN];
//        snprintf(text, sizeof(text),
//            "So you have chosen:\n"
//            "```json\n"
//            "%s\n"
//            "```",
//            values);
//
//        log_info("%s", event->data->components->array->value);
//        struct discord_interaction_response params = {
//            .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
//            .data = &(struct discord_interaction_callback_data){
//                .content = text,
//            }
//        };
//        discord_create_interaction_response(client, event->id, event->token, &params, NULL);
//    }
}

void remove_left(char *org, const char *to_remove) {
    char *pos = strstr(org, to_remove);

    if(pos) {
        int len = strlen(to_remove);
        memmove(pos, pos + len, strlen(pos + len) + 1);
    }
}

char *run_command(const char *command) {
    FILE *fp;
    char *response = NULL;
    size_t size = 0;

    fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen");
        return NULL;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t buffer_len = strlen(buffer);
        char *temp = realloc(response, size + buffer_len + 1);
        if (temp == NULL) {
            perror("realloc");
            free(response);
            pclose(fp);
            return NULL;
        }
        response = temp;
        strcpy(response + size, buffer);
        size += buffer_len;
    }

    pclose(fp);
    return response;
}

void on_message(struct discord *client, const struct discord_message *event) {
    // Check if message should be responded to
    if (event->guild_id != JOCI && event->channel_id != BOT_PARANCSOK) {
        log_info("Message ignored in guild: \n%"PRIu64"    channel: %"PRIu64"", event->guild_id, event->channel_id);
        return;
    } else if (event->author->bot) {
        log_info("Ignored bot: %s", event->author->username);
        return;
    }

    char *response = NULL;

    if (event->content[0] == LEADER) {
        log_info("Leader character spotted!");

        char msg[strlen(event->content + 1)];
        strcpy(msg, event->content);

        char *command = strtok(msg, " ");
        char *content = strtok(NULL, "");

        if (strcmp(command, "!run") == 0 || strcmp(command, "!r") == 0) {
            if (event->author->id != MY_USER_ID) {
                response = "You dont have permission to use this command!";
            } else {
                log_info("Running command: run, with argument: %s", content);
                response = run_command(content);
            }
        } else {
            log_info("Invalid command: %s", command);
            response = "Invalid command";
        }
    } else {
        response = event->content;
        log_info("Copied %s!", event->author->username);
    }

    struct discord_create_message params = {
        .content = response
    };
    discord_create_message(client, event->channel_id, &params, NULL);
}

int main(int argc, char *argv[]) {
    const char *config;
    if (argc > 1) {
        config = argv[1];
    } else {
        config = "../config.json";
    }

    struct discord *client = discord_config_init(config);
    discord_add_intents(client, DISCORD_GATEWAY_MESSAGE_CONTENT);
    discord_set_on_ready(client, &on_ready);
    discord_set_on_interaction_create(client, &on_interaction);
    discord_set_on_message_create(client,&on_message);
    discord_run(client);
}
