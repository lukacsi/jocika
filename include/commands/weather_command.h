//
// Created by Microsoft Köszi on 18/11/2024.
//

#ifndef WEATHER_COMMAND_H
#define WEATHER_COMMAND_H
#include "command.h"


class WeatherCommand : public Command {
public:
    WeatherCommand() : Command("weather", "What's the weather like there?!") {
        add_option(dpp::command_option(dpp::co_string,
            "city",
            "Name the city",
            true));
    }
    void execute(const dpp::slashcommand_t& event, dpp::cluster& bot) override;
};



#endif //WEATHER_COMMAND_H
