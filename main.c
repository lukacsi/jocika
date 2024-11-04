#include <concord/discord.h>
#include <concord/log.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define GUILD_ID 1300798374911414303
u64snowflake JOCI = 1300798374911414303;
u64snowflake BOT_PARANCSOK = 298545786315079681;
char LEADER = '!';

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
char* run_command(char* command) {
    printf("%s", command);
    char *response;
    return response;
}

void on_message(struct discord *client, const struct discord_message *event) {
    // Check if message should be responded to
    if (event->author->bot) {
        log_info("Ignored bot: %s", event->author->username);
        return;
    }
    else if (event->guild_id != JOCI && event->channel_id != BOT_PARANCSOK) {
        log_info("Message ignored in guild: \n%"PRIu64"    channel: %"PRIu64"", event->guild_id, event->channel_id);
        return;
    }

    if(event->content[0] == LEADER) {
        char msg[strlen(event->content + 1)];
        strcpy(msg, event->content);
        memmove(msg, msg + 1, strlen(msg));
        log_info("Leader character spotted!");
        char *response;
        char *command = strtok(msg, " ");
        memmove(msg, msg + strlen(command), strlen(msg)
        if(strcmp(command, "run") == 0) {
           response = run_command();
        }
    }
    else {
        struct discord_create_message params = { 
            .content = event->content
        };
        discord_create_message(client, event->channel_id, &params, NULL);
        log_info("Copied %s!", event->author->username);
    }
}

int main(void) {
    struct discord *client = discord_init(getenv("DISCORD_BOT_TOKEN"));
    discord_add_intents(client, DISCORD_GATEWAY_MESSAGE_CONTENT);
    discord_set_on_ready(client, &on_ready);
    discord_set_on_interaction_create(client, &on_interaction);
    discord_set_on_message_create(client,&on_message);
    discord_run(client);
}
