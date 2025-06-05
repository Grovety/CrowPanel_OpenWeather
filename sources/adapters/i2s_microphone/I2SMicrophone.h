#pragma once

#include <cstddef>
#include <driver/i2s_std.h>

class I2SMicrophone
{
    static constexpr int  sampleRate = 16000;
    static constexpr int  i2sPort    = 0;
    static constexpr char Tag[]      = "I2SMic";

    static constexpr int dmaDescNum  = 8;
    static constexpr int dmaFrameNum = 160;

    static constexpr int bitsPerSample  = 32; // INMP441 всегда 24/32-бит
    static constexpr int bytesPerSample = bitsPerSample / 8;

    void cleanup();

    i2s_chan_handle_t rxChannel = nullptr;

public:
    I2SMicrophone();
    ~I2SMicrophone();

    bool   init();
    void   deinit();
    size_t read(void* buffer, size_t size);
};
