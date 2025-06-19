#include "VoiceCityRecognizer.h"
#include "adapters/HTTPRequest.h"
#include <esp_log.h>
#include <json_parser.h>
#include <cstring>

bool VoiceCityRecognizer::findFinalJSON(char* str, size_t len, char** out_start, size_t* out_len)
{
    const int MAX_JSON = 128;
    size_t    starts[MAX_JSON];
    size_t    ends[MAX_JSON];
    int       count = 0;

    for (size_t i = 0; i < len && count < MAX_JSON; ++i)
    {
        if (str[i] == '{')
        {
            int    depth = 1;
            size_t j     = i + 1;
            while (j < len && depth > 0)
            {
                if (str[j] == '{')
                    depth++;
                else if (str[j] == '}')
                    depth--;
                j++;
            }
            if (depth == 0)
            {
                starts[count] = i;
                ends[count]   = j - 1;
                count++;
                i = j - 1;
            } else
                break;
        }
    }

    for (int idx = count - 1; idx >= 0; --idx)
    {
        char*  substr  = str + starts[idx];
        size_t obj_len = ends[idx] - starts[idx] + 1;
        if (strstr(substr, "\"body\"") && strstr(substr, "PARTIAL_UNDERSTANDING"))
        {
            *out_start = substr;
            *out_len   = obj_len;
            return true;
        }
    }

    return false;
}

bool VoiceCityRecognizer::recognizeCity(WavRecorder& recorder, char* city, size_t cityNameLen)
{
    const uint8_t* wavData = recorder.getWavData();
    size_t         wavSize = recorder.getSize();
    if (!wavData || wavSize == 0)
    {
        ESP_LOGE(Tag, "No WAV data available");
        return false;
    }

    HTTPRequest request = HTTPRequest::Builder()
                              .setHost("api.wit.ai", 443, "/speech?v=20240520")
                              .setMethod(HTTP_METHOD_POST)
                              .setSSL(esp_crt_bundle_attach)
                              .setBuffer(responseBuffer_, BufferSize)
                              .setBody(wavData, wavSize, "audio/wav")
                              .setAuthHeader("Bearer X4WNJUTOV754K7RPZR4USUJEDI6ZOTAH")
                              .build();

    uint32_t receivedLen = request.perform();
    if (receivedLen == 0)
    {
        ESP_LOGE(Tag, "Empty response from wit.ai");
        return false;
    }

    char*  resultJSON;
    size_t resultLen;
    if (findFinalJSON(responseBuffer_, receivedLen, &resultJSON, &resultLen))
    {
        ESP_LOGI(Tag, "JSON:%s", resultJSON);
        jparse_ctx_t jctx;
        if (json_parse_start(&jctx, resultJSON, resultLen) != OS_SUCCESS)
        {
            ESP_LOGE(Tag, "JSON parsing failed");
            return false;
        }

        bool ok = false;
        int  count1, values_count;
        if (json_obj_get_object(&jctx, "entities") == OS_SUCCESS &&
            json_obj_get_array(&jctx, "wit$location:location", &count1) == OS_SUCCESS &&
            json_arr_get_object(&jctx, 0) == OS_SUCCESS &&
            json_obj_get_object(&jctx, "resolved") == OS_SUCCESS &&
            json_obj_get_array(&jctx, "values", &values_count) == OS_SUCCESS &&
            (values_count > 0) && json_arr_get_object(&jctx, 0) == OS_SUCCESS &&
            json_obj_get_string(&jctx, "name", city, cityNameLen) == OS_SUCCESS)
        {
            ESP_LOGI(Tag, "City detected: %s", city);
            ok = true;
        }
        json_parse_end(&jctx);
        return ok;
    }
    ESP_LOGW(Tag, "City not found");
    return false;
}