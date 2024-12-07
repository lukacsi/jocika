#include "utils/weather_boi.h"
#include <curl/curl.h>
#include <iostream>

size_t WeatherBoi::CallBackWriter(void* content, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)content, size*nmemb);
    return size*nmemb;
}

std::string WeatherBoi::get_weather_data(const std::string &city, const std::string &api_key) {
    CURL* curl;
    CURLcode res;
    std::string r_buff;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        std::string url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + api_key;
        std::cout << url << std::endl;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CallBackWriter);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r_buff);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    std::cout << "Raw response: " << r_buff <<std::endl;
    return r_buff;
}

std::string WeatherBoi::json_weather_parser(const std::string &json_data) {
    using json = nlohmann::json;

    try {
        auto data = json::parse(json_data);
        std::string city_name = data ["name"];
        std::string weather_description = data["weather"][0]["description"];
        float temp = data["main"]["temp"];

        std::string weather_report = "Weather in " + city_name + ":\n" +
            "Temperature: " + std::to_string(temp-273) + "°C\n" +
                "Description: " + weather_description;
        return weather_report;
    } catch (const std::exception& e) {
        return "Error parsing weather data: "+std::string(e.what());
    }
}


