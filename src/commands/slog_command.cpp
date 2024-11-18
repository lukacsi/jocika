#include "commands/slog_command.h"
#include "globals.h"

void SLogCommand::execute(const dpp::slashcommand_t& event, const dpp::cluster& bot) {

/*

    dpp::snowflake tgt = std::get<dpp::snowflake>(event.get_parameter("target"));
    dpp::guild_member *gmembr;
    *gmembr = dpp::find_guild_member(event.command.guild_id,tgt);
    std::fstream pers_log("personal_log.txt");

*/


    if(gmembr == nullptr) {
        event.reply(dpp::message("User does not exist on this server.").set_flags(dpp::m_ephemeral));
    } else {
        Globals::get_instance().log_user(tgt,event.command.guild_id);
        event.reply(dpp::message("Logging target user.").set_flags(dpp::m_ephemeral));
    }
}

