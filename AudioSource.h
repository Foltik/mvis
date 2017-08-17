#pragma once
#include <cstdint>
#include <cstddef>

struct AudioBuffer {
    int16_t r;
    int16_t l;
};

class AudioSource {
public:
    explicit AudioSource() {};
    virtual ~AudioSource() {};

    virtual bool read(AudioBuffer* buffer, size_t size) = 0;
};


