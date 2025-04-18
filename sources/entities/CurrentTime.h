#pragma once

#include <ctime>
#include <stdint.h>

class CurrentTime {
    CurrentTime() = default;
    time_t _timestamp = 0;
    time_t _timezoneOffset = 0;

  public:
    static CurrentTime &instance();

    void init(const char *ntpServer = "pool.ntp.org");
    bool sync(uint32_t timeoutMs = 10000);
    bool isTimeSet() const;
    time_t now() const;
    time_t nowLocal() const;
    void setTimezoneOffset(int32_t timezoneOffsetSeconds);
};