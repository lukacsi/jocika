#include <dpp/dpp.h>
#include <iomanip>
#include <sstream>
 
#include <vector>
#include <fstream>
#include <iostream>
#include <mpg123.h>
#include <out123.h>

#define MUSIC_FILE "/home/oger/Code/linuxprog/nagyhf/media/vicc.mp3"

const std::string BOT_TOKEN = "";


int main() {

    std::vector<uint8_t> pcmdata;
	 
    mpg123_init();
 
    int err = 0;
    unsigned char* buffer;
    size_t buffer_size, done;
    int channels, encoding;
    long rate;
 
    mpg123_handle *mh = mpg123_new(NULL, &err);
    mpg123_param(mh, MPG123_FORCE_RATE, 48000, 48000.0);
 
    buffer_size = mpg123_outblock(mh);
    buffer = new unsigned char[buffer_size];
 
    mpg123_open(mh, MUSIC_FILE);
    mpg123_getformat(mh, &rate, &channels, &encoding);
 
    unsigned int counter = 0;
    for (int totalBytes = 0; mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK; ) {
        for (size_t i = 0; i < buffer_size; i++) {
            pcmdata.push_back(buffer[i]);
        }
        counter += buffer_size;
        totalBytes += done;
    }
    delete[] buffer;
    mpg123_close(mh);
    mpg123_delete(mh);

    dpp::cluster bot(BOT_TOKEN);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([pcmdata](const dpp::slashcommand_t& event){
        if(event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }
        else if (event.command.get_command_name() == "vicc") {
            dpp::guild* g = dpp::find_guild(event.command.guild_id);

            auto current_vc = event.from->get_voice(event.command.guild_id);
            bool join_vc = true;
            
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

                event.reply("Im bouta ");

                dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);
                v->voiceclient->send_audio_raw((uint16_t*)pcmdata.data(), pcmdata.size());

            } else {
                event.reply("Mán itt vagyok!");
            }
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            dpp::slashcommand vicc("vicc", "Medium funny", bot.me.id);
            dpp::slashcommand ping("ping", "Ping pong!", bot.me.id);
            bot.global_bulk_command_delete();
            bot.guild_bulk_command_create({ vicc, ping }, 298535878702137344);

        }
    });

    bot.start(dpp::st_wait);
    return EXIT_SUCCESS;

} 
