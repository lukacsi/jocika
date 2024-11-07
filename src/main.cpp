#include <cstdint>
#include <cstdlib>
#include <dpp/appcommand.h>
#include <dpp/dpp.h>
#include <unistd.h>
#include <vector>
#include <mpg123.h>
#include <out123.h>

#define MEDIA_DIR "./media/"

const std::string BOT_TOKEN = std::getenv("DISCORD_BOT_TOKEN");

std::map<std::string, std::vector<uint8_t>> load_pcm_data() {
    std::map<std::string, std::vector<uint8_t>> pcm_data_map;
    
    mpg123_init();
    int err = 0;

    for (const auto& entry : std::filesystem::directory_iterator(MEDIA_DIR)) {
        if (entry.is_regular_file() && entry.path().extension() == ".mp3") {
            std::vector<uint8_t> pcmdata;
            unsigned char* buffer;
            size_t buffer_size, done;
            int channels, encoding;
            long rate;

            mpg123_handle *mh = mpg123_new(NULL, &err);
            mpg123_param(mh, MPG123_FORCE_RATE, 48000, 48000.0);
            buffer_size = mpg123_outblock(mh);
            buffer = new unsigned char[buffer_size];

            if (mpg123_open(mh, entry.path().c_str()) == MPG123_OK) {
                mpg123_getformat(mh, &rate, &channels, &encoding);

                while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK) {
                    for (size_t i = 0; i < done; i++) {
                        pcmdata.push_back(buffer[i]);
                    }
                }

                pcm_data_map[entry.path().filename().string()] = pcmdata;
            }

            delete[] buffer;
            mpg123_close(mh);
            mpg123_delete(mh);
        }
    }

    mpg123_exit();
    return pcm_data_map;
}

int main() {
    auto pcm_data_map = load_pcm_data();

    dpp::cluster bot(BOT_TOKEN);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([&bot, pcm_data_map](const dpp::slashcommand_t& event){
        if(event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }
        else if (event.command.get_command_name() == "vicc") {
            dpp::guild* g = dpp::find_guild(event.command.guild_id);

            auto current_vc = event.from->get_voice(event.command.guild_id);
            bool join_vc = true;

            std::string file_name; 
            if (std::holds_alternative<std::string>(event.get_parameter("file"))) {
                file_name = std::get<std::string>(event.get_parameter("file"));
            }
            
            if (current_vc) {
                auto user_vc = g->voice_members.find(event.command.get_issuing_user().id);

                if (user_vc != g->voice_members.end() && current_vc->channel_id == user_vc->second.channel_id) {
                    join_vc = false;
                    
                } else {
                    event.from->disconnect_voice(event.command.guild_id);
                    join_vc = true;
                }
            }

            if (join_vc) {
                if (!g->connect_member_voice(event.command.get_issuing_user().id)) {
                    event.reply("You dont seem to be in a voice channel!");
                    return;
                }

                event.reply("Joining voice.");
                bot.on_voice_ready([&pcm_data_map, &file_name, &event](const dpp::voice_ready_t& voice_event) {
                    dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

                    if (v && v->voiceclient && v->voiceclient->is_ready()) {
                        if (!file_name.empty() && pcm_data_map.find(file_name) != pcm_data_map.end()) {
                            v->voiceclient->send_audio_raw((uint16_t*)pcm_data_map.at(file_name).data(), pcm_data_map.at(file_name).size());
                            event.reply("Playing file: " + file_name);
                        } else if (file_name.empty()) {
                            for (const auto& [filename, pcmdata] : pcm_data_map) {
                                v->voiceclient->send_audio_raw((uint16_t*)pcmdata.data(), pcmdata.size());
                                sleep(1);
                            }
                            event.reply("Played all songs in the media library!");
                        } else {
                            event.reply("File not found: " + file_name);
                        }
                    } else {
                        event.reply("Failed to play audio: voice client not ready.");
                    } 
                });
            } else {
                event.reply("Mán itt vagyok!");
            }
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            dpp::slashcommand vicc("vicc", "Medium funny", bot.me.id);
            vicc.add_option(
                dpp::command_option(dpp::co_string, "file", "Specify the file name to play", false)
            );
            dpp::slashcommand ping("ping", "Ping pong!", bot.me.id);
            //bot.global_bulk_command_delete();
            bot.global_bulk_command_create({ vicc, ping });

        }
    });

    bot.start(dpp::st_wait);
    return EXIT_SUCCESS;
} 
