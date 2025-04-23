#pragma once

#include <stdint.h>

class Location
{
    static constexpr char* Tag = "Location";
    char                   locationResponseBuffer[2048];

public:
    static constexpr uint8_t MaxCityNameLength = 150;
    struct Data {
        char longitude[20];
        char latitude[20];
        char locationName[MaxCityNameLength];
    };

    static Location& instance()
    {
        static Location instance;
        return instance;
    }

    bool get(Data&);
    bool get(char*);
    bool setManual(char* const locationName);
};