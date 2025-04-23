#pragma once

#include <stdint.h>

#define WEATHER_API_KEY "b4d1299a4d4bb3e2ed262199a1c61cae"
//#define WEATHER_URL
//"http://api.openweathermap.org/data/2.5/weather?q=Kazan&appid="
// WEATHER_API_KEY
#define WEATHER_URL "http://api.openweathermap.org/data/2.5/weather?"
//#define FORECAST_URL
//"http://api.openweathermap.org/data/2.5/forecast?q=Kazan&appid="
// WEATHER_API_KEY
#define FORECAST_URL "http://api.openweathermap.org/data/2.5/forecast?"

class Weather
{
    static constexpr uint16_t RequestBufferSize = 256;
    static constexpr char     Tag[]             = "weather";
    struct Ctx {
        char openWeatherDataBuffer[32768];
        char longitude[20];
        char latitude[20];
        char requestURL[RequestBufferSize];
        char locationName[150];
    } * ctx_;

    Weather();
    Weather(const Weather&)            = delete;
    Weather& operator=(const Weather&) = delete;

public:
    static Weather& instance()
    {
        static Weather instance;
        return instance;
    }

    struct Data {
        float    temperature;
        float    humidity;
        float    feelsLike;
        char     description[64];
        char     city[64];
        char     country[64];
        uint32_t timestamp;
        uint32_t timestampOffset;
        char     icon[64];
        float    windSpeed;
        float    windDir;
        float    pressure;
        uint32_t sunriseTimestamp;
        uint32_t sunsetTimestamp;
    };

    bool getCurrentWeather(Data* data);

    bool getForecast(Data* data);

    bool getNextDayIdx(Data* data, uint8_t* curIdx);

    void setLocation(char* lat, char* lon, char* name);

    bool checkLocation(char* name);
};
