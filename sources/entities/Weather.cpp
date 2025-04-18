#include "Weather.h"
#include "adapters/HTTPRequest.h"
#include <esp_log.h>
#include <json_parser.h>

char *urlEncode(const char *input) {
    size_t len = strlen(input);
    char *encoded = static_cast<char *>(malloc(len * 3 + 1));
    if (!encoded)
        return nullptr;

    char *p = encoded;
    while (*input) {
        if (isalnum((unsigned char)*input) || *input == '-' || *input == '_' ||
            *input == '.' || *input == '~') {
            *p++ = *input;
        } else {
            sprintf(p, "%%%02X", (unsigned char)*input);
            p += 3;
        }
        input++;
    }
    *p = '\0';
    return encoded;
}

Weather::Weather() {
    ctx_ = static_cast<Ctx *>(heap_caps_malloc(sizeof(Ctx), MALLOC_CAP_SPIRAM));
}

bool Weather::getCurrentWeather(Data *data) {
    if (!data)
        return false;

    if ((ctx_->latitude[0] != '\0') && (ctx_->longitude[0] != '\0'))
        snprintf(ctx_->requestURL, sizeof(ctx_->requestURL),
                 "%slat=%s&lon=%s&appid=%s", WEATHER_URL, ctx_->latitude,
                 ctx_->longitude, WEATHER_API_KEY);
    else if (ctx_->locationName[0] != '0') {
        char *encodedCity = urlEncode(ctx_->locationName);
        if (!encodedCity)
            return false;

        snprintf(ctx_->requestURL, sizeof(ctx_->requestURL), "%sq=%s&appid=%s",
                 WEATHER_URL, encodedCity, WEATHER_API_KEY);
        free(encodedCity);
    } else {
        ESP_LOGE(Tag, "Location not ready");
        return false;
    }

    HTTPRequest request(ctx_->requestURL, HTTP_METHOD_POST,
                        ctx_->openWeatherDataBuffer,
                        sizeof(ctx_->openWeatherDataBuffer));

    jparse_ctx_t jctx;
    int numCnt = 0;
    int idx = 0;
    uint32_t receivedLen;
    bool retVal = true;
    int64_t timestamp;
    int64_t timezone;
    int64_t sunrise;
    int64_t sunset;
    if (((receivedLen = request.perform()) > 0) &&
        (json_parse_start(&jctx, ctx_->openWeatherDataBuffer, receivedLen) ==
         OS_SUCCESS) &&
        (json_obj_get_array(&jctx, "weather", &numCnt) == OS_SUCCESS) &&
        (json_arr_get_object(&jctx, 0) == OS_SUCCESS) &&
        (json_obj_get_string(&jctx, "description", data->description, 64) ==
         OS_SUCCESS) &&
        (json_obj_get_string(&jctx, "icon", data->icon, 64) == OS_SUCCESS) &&
        (json_arr_leave_object(&jctx) == OS_SUCCESS) &&
        (json_obj_leave_array(&jctx) == OS_SUCCESS) &&
        (json_obj_get_object(&jctx, "main") == OS_SUCCESS) &&
        (json_obj_get_float(&jctx, "temp", &data->temperature) == OS_SUCCESS) &&
        (json_obj_get_float(&jctx, "feels_like", &data->feelsLike) ==
         OS_SUCCESS) &&
        (json_obj_get_float(&jctx, "pressure", &data->pressure) ==
         OS_SUCCESS) &&
        (json_obj_get_float(&jctx, "humidity", &data->humidity) ==
         OS_SUCCESS) &&
        (json_obj_leave_object(&jctx) == OS_SUCCESS) &&
        (json_obj_get_object(&jctx, "wind") == OS_SUCCESS) &&
        (json_obj_get_float(&jctx, "speed", &data->windSpeed) == OS_SUCCESS) &&
        (json_obj_get_float(&jctx, "deg", &data->windDir) == OS_SUCCESS) &&
        (json_obj_leave_object(&jctx) == OS_SUCCESS) &&
        (json_obj_get_int64(&jctx, "dt", &timestamp) == OS_SUCCESS) &&
        (json_obj_get_object(&jctx, "sys") == OS_SUCCESS) &&
        (json_obj_get_string(&jctx, "country", data->country,
                             sizeof(data->country)) == OS_SUCCESS) &&
        (json_obj_get_int64(&jctx, "sunrise", &sunrise) == OS_SUCCESS) &&
        (json_obj_get_int64(&jctx, "sunset", &sunset) == OS_SUCCESS) &&
        (json_obj_leave_object(&jctx) == OS_SUCCESS) &&
        (json_obj_get_int64(&jctx, "timezone", &timezone) == OS_SUCCESS) &&
        (json_obj_get_string(&jctx, "name", data->city, sizeof(data->city)) ==
         OS_SUCCESS)) {
        ESP_LOGI(Tag, "Parsed: %.1f, %.1f, %s,", data->temperature,
                 data->humidity, data->description);
        data->timestamp = (uint32_t)timestamp + timezone;
        data->timestampOffset = (uint32_t)timezone;
        data->sunriseTimestamp = (uint32_t)sunrise + timezone;
        data->sunsetTimestamp = (uint32_t)sunset + timezone;
        ESP_LOGI(Tag, "%lu, %lu, %s, %s", data->timestamp,
                 data->timestampOffset, data->city, data->country);
    } else {
        ESP_LOGE(Tag, "Parser failed: objects");
        retVal = false;
    }
    json_parse_end(&jctx);
    memset(ctx_->openWeatherDataBuffer, 0, sizeof(ctx_->openWeatherDataBuffer));
    data->temperature -= 273.15f;
    data->feelsLike -= 273.15f;
    return retVal;
}
bool Weather::getForecast(Data *data) {
    if (!data)
        return false;

    if ((ctx_->latitude[0] != '\0') && (ctx_->longitude[0] != '\0'))
        snprintf(ctx_->requestURL, sizeof(ctx_->requestURL),
                 "%slat=%s&lon=%s&appid=%s", FORECAST_URL, ctx_->latitude,
                 ctx_->longitude, WEATHER_API_KEY);
    else if (ctx_->locationName[0] != '0') {
        char *encodedCity = urlEncode(ctx_->locationName);
        if (!encodedCity)
            return false;

        snprintf(ctx_->requestURL, sizeof(ctx_->requestURL), "%sq=%s&appid=%s",
                 FORECAST_URL, encodedCity, WEATHER_API_KEY);
        free(encodedCity);
    } else {
        ESP_LOGE(Tag, "Location not ready");
        return false;
    }

    HTTPRequest request(ctx_->requestURL, HTTP_METHOD_POST,
                        ctx_->openWeatherDataBuffer,
                        sizeof(ctx_->openWeatherDataBuffer));

    jparse_ctx_t jctx;
    int numCnt = 0;
    int unused = 0;
    uint32_t receivedLen;
    bool retVal = true;
    int64_t timestamp;
    int64_t timezone;
    if (((receivedLen = request.perform()) > 0) &&
        (json_parse_start(&jctx, ctx_->openWeatherDataBuffer, receivedLen) ==
         OS_SUCCESS) &&
        (json_obj_get_array(&jctx, "list", &numCnt) == OS_SUCCESS)) {
        for (uint8_t i = 0; i < 40; i++) {
            if ((json_arr_get_object(&jctx, i) ==
                 OS_SUCCESS) && // array i START
                (json_obj_get_int64(&jctx, "dt", &timestamp) == OS_SUCCESS) &&
                (json_obj_get_object(&jctx, "main") ==
                 OS_SUCCESS) && // main START
                (json_obj_get_float(&jctx, "temp", &data[i].temperature) ==
                 OS_SUCCESS) &&
                (json_obj_leave_object(&jctx) == OS_SUCCESS) && // main END
                (json_obj_get_array(&jctx, "weather", &unused) ==
                 OS_SUCCESS) && // weather array
                                // START
                (json_arr_get_object(&jctx, 0) ==
                 OS_SUCCESS) && // weather array END
                (json_obj_get_string(&jctx, "icon", data[i].icon, 64) ==
                 OS_SUCCESS) &&
                (json_arr_leave_object(&jctx) == OS_SUCCESS) && // weather 0 END
                (json_obj_leave_array(&jctx) ==
                 OS_SUCCESS) && // weather array END
                (json_arr_leave_object(&jctx) == OS_SUCCESS) /// array i END
            ) {
                data[i].timestamp = (uint32_t)timestamp;
                data[i].temperature -= 273.15f;
            } else {
                ESP_LOGE(Tag, "Parser failed: objects");
                retVal = false;
            }
        }
    } else {
        ESP_LOGE(Tag, "Parser failed: objects");
        retVal = false;
    }

    if (retVal && (json_obj_leave_array(&jctx) == OS_SUCCESS) && // list END
        (json_obj_get_object(&jctx, "city") == OS_SUCCESS) &&    // city START
        (json_obj_get_int64(&jctx, "timezone", &timezone) == OS_SUCCESS) &&
        (json_obj_leave_object(&jctx) == OS_SUCCESS) // city END
    )
        for (uint8_t i = 0; i < 4; i++)
            data[i].timestamp += timezone;

    json_parse_end(&jctx);
    memset(ctx_->openWeatherDataBuffer, 0, sizeof(ctx_->openWeatherDataBuffer));

    uint8_t idx = 0;
    while (getNextDayIdx(data, &idx) == true) {
        char text[20];
        time_t timestampStruct = data[idx].timestamp;
        struct tm *timeInfo = localtime(&timestampStruct);
        strftime(text, sizeof(text), "%d.%m.%y", timeInfo);
    }

    return retVal;
}

bool Weather::getNextDayIdx(Data *data, uint8_t *curIdx) {
    if (!data || !curIdx || (*curIdx >= 40))
        return false;
    struct tm tm_first;
    gmtime_r((time_t *)&data[*curIdx].timestamp, &tm_first);

    for (; *curIdx < 40; (*curIdx)++) {
        struct tm tm_current;
        gmtime_r((time_t *)&data[*curIdx].timestamp, &tm_current);

        if (tm_current.tm_year != tm_first.tm_year ||
            tm_current.tm_mon != tm_first.tm_mon ||
            tm_current.tm_mday != tm_first.tm_mday) {
            return true;
        }
    }
    return false;
}

void Weather::setLocation(char *lat, char *lon, char *name) {
    if (!lat || !lon)
        return;
    strncpy(ctx_->latitude, lat, sizeof(ctx_->latitude));
    strncpy(ctx_->longitude, lon, sizeof(ctx_->longitude));
    strncpy(ctx_->locationName, name, sizeof(ctx_->locationName));
}
