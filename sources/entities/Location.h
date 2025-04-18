#pragma once

class Location {
    static constexpr char *Tag = "Location";
    char locationResponseBuffer[2048];

  public:
    struct Data {
        char longitude[20];
        char latitude[20];
        char locationName[150];
    };

    static Location &instance() {
        static Location instance;
        return instance;
    }

    bool get(Data &);
    bool setManual(const char *locationName);
};