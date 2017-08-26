#include "Lorenz.h"

float Lorenz::m_data[Lorenz::m_szData];
    
void Lorenz::Init(CEngine *e) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, m_szDataBytes, nullptr, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, m_szDataBytes, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, m_szDataPt * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, m_szDataPt * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Lorenz::Cleanup() {}
void Lorenz::Pause() {}
void Lorenz::Resume() {}

void Lorenz::ProcessInput(bool *keyboard, bool *mouse, double mxpos, double mypos) {
    if (keyboard[GLFW_KEY_Q] || keyboard[GLFW_KEY_ESCAPE])
        engine->Quit();
}

void Lorenz::Loop() {
    audioSource.read(audioBuffer, m_samples / m_framerate);
    execute();
}

void Lorenz::Render() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();
    shader.uMatrix4("proj", proj);

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(m_szData));
    glBindVertexArray(0);

    glfwSwapBuffers(CEngine::Instance().wnd);
}

struct point {
    float x;
    float y;
    float r;
    float g;
    float b;
};

int colors[31] = {
    0x4040ff,
    0x2c56fc,
    0x2a59fc,
    0x1180ed,
    0x04a6d5,
    0x02abd1,
    0x03d2aa,
    0x04d6a5,
    0x12ee7f,
    0x2bfc58,
    0x2dfc55,
    0x56fc2d,
    0x59fc2a,
    0x80ed11,
    0xa6d503,
    0xabd103,
    0xd2aa03,
    0xd6a504,
    0xee7f12,
    0xfc582b,
    0xfc552d,
    0xfc2c56,
    0xfc2a58,
    0xed1180,
    0xd503a6,
    0xd102ab,
    0xaa03d2,
    0xa504d6,
    0x7f12ee,
    0x582bfc,
    0x552dfc
};

void Lorenz::getColor(size_t colorDist, float& r, float& g, float& b) {
    bool wrap = true;
    size_t max = 64;
    size_t idx = static_cast<size_t>(((double)colorDist / (double)max) * 31);
    auto color = colors[wrap ? idx : std::min(idx, (size_t)31 - 1)];
    r = ((color >> 16) & 0xFF) / 255.0f;
    g = ((color >> 8) & 0xFF) / 255.0f;
    b = ((color >> 0) & 0xFF) / 255.0f;
}

void Lorenz::execute() {
    memset(m_data, 0, m_szData);

    const double winHeight = engine->wndH;
    const double halfHeight = winHeight / 2.0;
    const double winWidth = engine->wndW;

    const double dt = 0.0001;
    const double sigma = 10.0;
    const double beta = 8.0 / 3.0;
    const double rhoConst = 20.0;
    const double rhoMult = 0.0009;

    double lAvg = 0.0;
    double rAvg = 0.0;
    for (int i = 0; i < m_samples / m_framerate; i++) {
        lAvg += std::abs(audioBuffer[i].l);
        rAvg += std::abs(audioBuffer[i].r);
    }
    lAvg = lAvg / (m_samples / m_framerate);
    rAvg = rAvg / (m_samples / m_framerate);
    const double avg = lAvg + rAvg / 2.0;
    const auto rho = rhoConst + rhoMult * avg;

    const auto rotIntLeft = lAvg * (m_framerate / 131072.0);
    const auto rotIntRight = rAvg * (m_framerate / 131072.0);

    rotCountL += rotIntLeft;
    rotCountR += rotIntRight;

	if (rotCountL > 1000) rotCountL = 0;
	if (rotCountR > 1000) rotCountR = 0;

    double rotX = (rotCountL * 2.0 * M_PI) / 1000;
    double rotY = (rotCountR * 2.0 * M_PI) / 1000;

    double cosX = std::cos(rotX);
    double sinX = std::sin(rotX);
    double cosY = std::cos(rotY);
    double sinY = std::sin(rotY);

    const double zCenter = -1 + beta;
    const double equilibria = std::sqrt(beta * rho - beta);

    const auto scaleMultiplier = 1.25 * halfHeight / std::sqrt((beta * rho * rho) - (std::pow(zCenter - rho, 2) / std::pow(rho, 2)));

    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    double x0 = 0.1;
    double y0 = 0.0;
    double z0 = 0.0;

    for (int i = 0; i < m_numDataPts; i++) {
        double x1 = x0 + dt * sigma * (y0 - x0);
        double y1 = y0 + dt * (x0 * (rho - z0) - y0);
        double z1 = z0 + dt * (x0 * y0 - beta * z0);
        x0 = x1;
        y0 = y1;
        z0 = z1;

        double dist1 = std::sqrt(std::pow(x0 - equilibria, 2) + std::pow(y0 - equilibria, 2) + std::pow(z0 - zCenter, 2));
        double dist2 = std::sqrt(std::pow(x0 + equilibria, 2) + std::pow(y0 + equilibria, 2) + std::pow(z0 - zCenter, 2));
        size_t colorDist = static_cast<size_t>(std::min(dist1, dist2));

        float r, g, b;
        getColor(colorDist, r, g, b);

        x = x0;
        y = y0;
        z = z0 - zCenter;

        double rotMultX = x * cosY + z * sinY;
        double rotMultY = x * sinX * sinY + y * cosX - z * cosY * sinX;

        x = rotMultX * scaleMultiplier;
        y = rotMultY * scaleMultiplier;

        double winX = x + winWidth / 2.0;
        double winY = y + halfHeight;


        if (winY >= 0.0 && winY <= winHeight &&
            winX >= 0.0 && winX <= winWidth) {
            m_data[5 * i] = (float)winX;
            m_data[5 * i + 1] = (float)winY;
            m_data[5 * i + 2] = r;
            m_data[5 * i + 3] = g;
            m_data[5 * i + 4] = b;
        }
    }

	test = false;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_szDataBytes, m_data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
