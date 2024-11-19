#include "commands/clear_command.h"
#include "commands/command_manager.h"
#include "commands/join_command.h"
#include "commands/library_command.h"
#include "commands/pause_command.h"
#include "commands/ping_command.h"
#include "commands/disconnect_command.h"
#include "commands/play_command.h"
#include "commands/queue_command.h"
#include "commands/resume_command.h"
#include "commands/skip_command.h"
#include "commands/stop_command.h"
#include "listeners/listener_manager.h"
#include "listeners/ready_listener.h"
#include "listeners/slashcommand_listener.h"
#include "utils/audio.h"
#include "utils/guild_audio_manager.h"
#include "utils/track_library.h"
#include <dpp/dpp.h>
#include <iostream>
#include <memory>
#include <string>
#include <commands/slog_command.h>
#include <listeners/slog_listener.h>

int main(int argc, char* argv[]) {
    bool register_commands = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "init") {
            register_commands = true;
        }
    }

    const std::string media_dir = "./media";
    const char* token_env = std::getenv("DISCORD_BOT_TOKEN");
    if(!token_env) {
        std::cerr << "DISCORD_BOT_TOKEN environmental variable not set." << std::endl;
        return EXIT_FAILURE;
    }
    const std::string token = token_env;
    dpp::cluster bot(token, dpp::i_default_intents | dpp::i_message_content);

    bot.on_log(dpp::utility::cout_logger());
    

    auto track_library = std::make_shared<TrackLibrary>();
    if (!track_library->init_tracks(media_dir)) {
        std::cout << "Failed to load media, or media_dir empty" << std::endl;
    }

    auto audio_processor = std::make_shared<Audio>();
    auto guild_audio_manager = std::make_shared<GuildAudioManager>(audio_processor, track_library);

    
    // register commands
    auto sharedCommandManager = std::make_shared<CommandManager>();
    sharedCommandManager->add_command(std::make_unique<PingCommand>());
    sharedCommandManager->add_command(std::make_unique<JoinCommand>(audio_processor, guild_audio_manager));
    sharedCommandManager->add_command(std::make_unique<DisconnectCommand>(audio_processor, guild_audio_manager));
    sharedCommandManager->add_command(std::make_unique<PlayCommand>(audio_processor, guild_audio_manager, track_library));
    sharedCommandManager->add_command(std::make_unique<SkipCommand>(audio_processor, guild_audio_manager));
    sharedCommandManager->add_command(std::make_unique<StopCommand>(audio_processor, guild_audio_manager));
    sharedCommandManager->add_command(std::make_unique<QueueCommand>(audio_processor, guild_audio_manager));
    sharedCommandManager->add_command(std::make_unique<ClearCommand>(audio_processor, guild_audio_manager));
    sharedCommandManager->add_command(std::make_unique<PauseCommand>(audio_processor, guild_audio_manager));
    sharedCommandManager->add_command(std::make_unique<ResumeCommand>(audio_processor, guild_audio_manager));
    sharedCommandManager->add_command(std::make_unique<LibraryCommand>(track_library));
    sharedCommandManager->add_command(std::make_unique<SLogCommand>());

    auto listenerManager = std::make_unique<ListenerManager>();
    listenerManager->add_listener(std::make_unique<ReadyListener>(sharedCommandManager, register_commands));
    listenerManager->add_listener(std::make_unique<SlashCommandListener>(sharedCommandManager));
    listenerManager->add_listener(std::make_unique<SLogListener>(sharedCommandManager));

    listenerManager->register_listeners(bot);

    bot.start(dpp::st_wait);
    return EXIT_SUCCESS;
} 
