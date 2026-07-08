#pragma once

class Framebuffer {
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    unsigned int getTexture();
    void bind();
    void resize(int width, int height);
private:
    unsigned int width;
    unsigned int height;
    unsigned int framebuffer = 0;
    unsigned int renderBuffer = 0;
    unsigned int texture = 0;
};
