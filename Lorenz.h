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
	bool test = true;
	void getColor(size_t colorDist, float& r, float& g, float& b);
	void execute();
    
    int rotCountR;
    int rotCountL;

	const static size_t m_samples = 44100;
	const static size_t m_framerate = 60;
    const static size_t m_numDataPts = (m_samples / m_framerate) * 300;

	// 2 floats for X, Y
	// 3 floats for R, G, B
	const static size_t m_szDataPt = 5;
	const static size_t m_szData = m_numDataPts * m_szDataPt;
	const static  size_t m_szDataBytes = m_szData * sizeof(float);
 	static float m_data[m_szData];

    GLuint vao;
    GLuint vbo;

	FifoAudioSource audioSource = FifoAudioSource("/tmp/mpd.fifo");
    AudioBuffer audioBuffer[m_samples / m_framerate];

    Shader shader = Shader("shaders/shader.vert", "shaders/shader.frag");
    glm::mat4 proj = glm::ortho(0.0f, static_cast<float>(engine->wndW), static_cast<float>(engine->wndH), 0.0f, -1.0f, 1.0f);
};
