#include <iostream>
#include "Framebuffer.hpp"
#include <glad/glad.h>
#include <cstdlib>

Framebuffer::Framebuffer(int w, int h): width(w), height(h) {
    glad_glGenFramebuffers(1, &framebuffer);
    this->bind();

    glad_glGenTextures(1, &texture);
    glad_glBindTexture(GL_TEXTURE_2D, texture);
    glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glad_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glad_glGenRenderbuffers(1, &renderBuffer);
    glad_glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glad_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    glad_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Sorry Framebuffer could not be created." << std::endl;
        glad_glBindFramebuffer(GL_FRAMEBUFFER, 0);
        exit(EXIT_FAILURE);
    }
    glad_glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
    glad_glDeleteFramebuffers(1, &framebuffer);
}

unsigned int Framebuffer::getTexture() {
    return texture;
}

void Framebuffer::resize(int w, int h) {
    if (width == w and height == h) return;
    width = w;
    height = h;

    glad_glBindTexture(GL_TEXTURE_2D, texture);
    glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glad_glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glad_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

void Framebuffer::bind() {
    glad_glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}
