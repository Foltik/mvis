#pragma once
#include <string>
#include <cstdint>
#include <fcntl.h>
#include <cstring>

#include "AudioSource.h"

class FifoAudioSource : public AudioSource {
public:
    FifoAudioSource(std::string path);
    ~FifoAudioSource() override;

    bool read(AudioBuffer* buffer, size_t size);

private:
    std::string m_path;

    int m_fifo_fd;
    bool openFifo();
};


