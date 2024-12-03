//
// Created by Microsoft Köszi on 20/11/2024.
//

#include "commands/voice_recorder_stop_command.h"
#include <dpp/utility.h>

void VoiceRecStopCommand::execute(const dpp::slashcommand_t &event, dpp::cluster &bot) {
    if (!Globals::get_instance().is_vr_on()) {
        event.reply("Not recording voice. Nothing to stop");
        return;
    }
    Globals::get_instance().vc_file_close();
    Globals::get_instance().vr_disable();
    event.reply(dpp::ir_deferred_channel_message_with_source, "Processing...");
    const std::string input_pcm = "me.pcm";
    const std::string output_mp3 = "recording.mp3";

    std::string ffmpeg_command = "ffmpeg -hide_banner -loglevel quiet -y -f s16le -ar 48000 -ac 2 -i " + input_pcm + " " + output_mp3;

    int ret = system(ffmpeg_command.c_str());
    if (ret != 0) {
        event.edit_response("Failed to convert PCM to MP3. Please check FFmpeg installation and PCM file format.");
        return;
    }

    dpp::message msg(event.command.channel_id, "Stopped voice recording!");
    msg.add_file("recording.mp3", dpp::utility::read_file(output_mp3));

    event.edit_response(msg);
}
