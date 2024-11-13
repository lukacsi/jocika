#include "listeners/slog_listener.h"
#include <memory>
#include <fstream>

SLogListener::SLogListener(std::shared_ptr<CommandManager> cmdMan)
    : commandManager(cmdMan) {}

void SLogListener::register_listener(dpp::cluster& bot) {

    std::fstream pers_log;
    pers_log.open("personal_log.txt", std::ios::app);

    if(!pers_log.is_open()) {
        bot.on_message_create([this](const dpp::message_create_t& event) {
            event.reply(dpp::message("Failed to open log file before stuff.").set_flags(dpp::m_ephemeral));
        });
        return;
    }

    bot.on_message_create([this,&pers_log](const dpp::message_create_t& event){
        if(pers_log.is_open()) {
            for(int i=0; i<go.get_v_size();i++){
                if(event.msg.author == go.get_user(i) && event.msg.guild_id == go.get_guild(i)) {
                    std::cout << "Username: " << event.msg.author.username << "Message: " << event.msg.content <<" Ch_ID: "<< event.msg.channel_id << "\n";
                }
        }
        }
        //else {
          //  event.reply(dpp::message("Failed to open log file.").set_flags(dpp::m_ephemeral));
        //}
    });


}
