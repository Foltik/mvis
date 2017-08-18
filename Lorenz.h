#pragma once
#include <k5/k5.h>
#include <memory>

#include "FifoAudioSource.h"

class Lorenz : public IFrame {
public:
    void Init(CEngine* e) override;
    void Cleanup() override;

    void Pause() override;
    void Resume() override;

    void ProcessInput(bool *keyboard, bool *mouse, double mxpos, double mypos) override;
    void Loop() override;
    void Render() override;

    static Lorenz& Instance() {
        static Lorenz instance;
        return instance;
    }

protected:
    Lorenz() = default;
    Lorenz(const Lorenz&) = delete;
    Lorenz(Lorenz&&) = delete;
    void operator=(const Lorenz&) = delete;
    void operator=(Lorenz&&) = delete;

private:
    FifoAudioSource audioSource = FifoAudioSource("/tmp/mpd.fifo");
    AudioBuffer* audioBuffer = nullptr;

    int rotCountR;
    int rotCountL;

    void execute();

    size_t numPts = (44100 / 60) * 300;
    float* pts = new float[numPts * 5];

    GLuint vao;
    GLuint vbo;

    Shader shader = Shader("shaders/shader.vert", "shaders/shader.frag");
    glm::mat4 proj = glm::ortho(0.0f, static_cast<float>(engine->wndW), static_cast<float>(engine->wndH), 0.0f, -1.0f, 1.0f);
};