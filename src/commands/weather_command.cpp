//
// Created by Microsoft Köszi on 18/11/2024.
//

#include "commands/weather_command.h"

void WeatherCommand::execute(const dpp::slashcommand_t &event, dpp::cluster &bot) {
    std::string city = std::get<std::string>(event.get_parameter("city"));

    if(city.empty()) {
        event.reply("Please specify a city!");
        return;
    }

    // TODO change later
    std::string api_key = "0ae2de3c3ed5fce1bd1f6a28bb429d1c";

    std::string raw_d = WeatherBoi::get_weather_data(city,api_key);
    std::string weather_r = WeatherBoi::json_weather_parser(raw_d);

    event.reply(weather_r);
}

