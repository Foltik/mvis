#include <zconf.h>
#include "FifoAudioSource.h"

FifoAudioSource::FifoAudioSource(std::string path) {
    m_path = path;
    openFifo();
}

FifoAudioSource::~FifoAudioSource() {
    if (m_fifo_fd >= 0)
        ::close(m_fifo_fd);
}

bool FifoAudioSource::openFifo() {
    m_fifo_fd = open(m_path.c_str(), O_RDONLY);
    if (m_fifo_fd < 0) {
        fprintf(stderr, "Could not read FIFO file %s\n", m_path.c_str());
        m_fifo_fd = -1;
        return false;
    }

    // Make sure not to block the file
    int flags = fcntl(m_fifo_fd, F_GETFL, 0);
    int ret = fcntl(m_fifo_fd, F_SETFL, flags | O_NONBLOCK);

    if (ret < 0)
        fprintf(stderr, "Could not set flags on file %s\n", m_path.c_str());

    return true;
}

bool FifoAudioSource::read(AudioBuffer* buffer, size_t size) {
    if (m_fifo_fd < 0) {
        openFifo();
    }

    size_t sizeBytes = sizeof(AudioBuffer) * size;
    size_t remainingBytes = sizeBytes;
    if (m_fifo_fd >= 0) {
        int attempts = 0;
        ::memset(buffer, 0, sizeBytes);

        while (remainingBytes > 0) {
            // Read into buffer
            ssize_t bytesRead = ::read(m_fifo_fd, buffer, remainingBytes);

            if (bytesRead == 0) {
                fprintf(stderr, "Could not read any bytes\n");
                return false;
            } else if (bytesRead == -1) {
                int err = errno;

                // EAGAIN = not enough data to read yet
                if (err == EAGAIN) {
                    if (attempts < 100) {
                        //fprintf(stderr, "Could not finish reading buffer\nRead: %ld, Size: %ld\n", bytesRead, sizeBytes);
                        ::memset(buffer, 0, sizeBytes);
                        ::close(m_fifo_fd);
                        m_fifo_fd = -1;
                        return false;
                    }
                } else {
                    // Wait for 1ms
                    nanosleep((const struct timespec[]){{0, 1000000L}}, NULL);
                    attempts++;
                }
            } else {
                remainingBytes -= bytesRead;
            }
        }
        return true;
    }
    return false;
}
