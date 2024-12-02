//
// Created by Microsoft Köszi on 18/11/2024.
//

#ifndef WEATHER_BOI_H
#define WEATHER_BOI_H

#include "dpp/dpp.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>


class WeatherBoi {
public:
  static size_t CallBackWriter(void* content, size_t size, size_t nmemb, void* userp);
  static std::string get_weather_data(const std::string& city, const std::string& api_key);
  static std::string json_weather_parser(const std::string& json_data);
};



#endif //WEATHER_BOI_H
