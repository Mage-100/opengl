#include <GLFW/glfw3.h>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <shader.hpp>
#include <filesystem>

#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// #include "pch.h"

#include "Window.hpp"
#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "VertexBufferLayout.hpp"
#include "Camera.hpp"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

std::filesystem::path exe_dir() {
#ifdef _WIN32
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path();
#else
    return std::filesystem::read_symlink("/proc/self/exe").parent_path();
#endif
}

inline std::filesystem::path getShaderSourcePath() {
#ifdef DEBUG_BUILD
    return std::filesystem::path(SHADERS_SOURCE);
#else
    throw std::runtime_error("Shader source path unavailable in release build");
#endif
}

inline std::filesystem::path getAssetSourcePath() {
#ifdef DEBUG_BUILD
    return std::filesystem::path(ASSETS_SOURCE);
#else
    throw std::runtime_error("Asset source path unavailable in release build");
#endif
}

int main() {
    int fb_width = 0;
    int fb_height = 0;

    double last_mouse_x = 0;
    double last_mouse_y = 0;

    Window window(800, 600, "opengl_glass_ui");
    window.getFramebufferSize(&fb_width, &fb_height);
    last_mouse_x = fb_width / 2.0;
    last_mouse_y = fb_height / 2.0;

    window.onFramebufferSizeCallback(
        [&fb_width, &fb_height](int w, int h) {
            glViewport(0, 0, w, h);

            if (w == 0 || h == 0) return;
            fb_width = w;
            fb_height = h;
        }
    );

    float yaw = -90.0f;
    float pitch = 0.0f;
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    window.onCursorPosCallback([&](double xpos, double ypos) {
        float xoffset = xpos - last_mouse_x;
        float yoffset = last_mouse_y - ypos;
        last_mouse_x = xpos;
        last_mouse_y = ypos;

        const float sensitivity = 0.1f;
        if (glfwGetMouseButton(window.getGLFWhandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            yaw += xoffset * sensitivity;
            pitch += yoffset * sensitivity;
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }

        cameraFront.x = glm::cos(glm::radians(yaw));
        cameraFront.y = glm::sin(glm::radians(pitch));
        cameraFront.z = glm::sin(glm::radians(yaw));
    });

    // glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window.getGLFWhandle(), true);
    ImGui_ImplOpenGL3_Init();

    std::filesystem::path shaderSourcePath = getShaderSourcePath();
    std::filesystem::path assetsSourcePath = getAssetSourcePath();

    Shader shader1(
        (shaderSourcePath / "ex_4/vertex.glsl").string().c_str(),
        (shaderSourcePath / "ex_4/fragment.glsl").string().c_str()
    );

    Shader shader2(
        (shaderSourcePath / "ex_4/2/vertex.glsl").string().c_str(),
        (shaderSourcePath / "ex_4/2/fragment.glsl").string().c_str()
    );

    float vertices[] = {
        // x      y      z
        -1.0f,  1.0f, 0.0f, // Top-Left
         1.0f,  1.0f, 0.0f, // Top-Right
         1.0f, -1.0f, 0.0f, // Bottom-Right

         1.0f, -1.0f, 0.0f, // Bottom-Right
        -1.0f, -1.0f, 0.0f, // Bottom-Left
        -1.0f,  1.0f, 0.0f // Top-Left
    };

    VertexArray  va;
    VertexBuffer vb(vertices, 6 * 3 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(3);
    va.AddBuffer(vb, layout);

    Camera cm( glm::vec3(0.0f, 3.0f, 10.0f) );

    while(!window.shouldWindowClose()) {
        cm.SetCameraFront(cameraFront);

        static float mouse_x = fb_width/2.0;
        static float mouse_y = fb_height/2.0;

        static float dt = 0.0f;
        static float lastFrame = 0.0f;

        float currentFrame = glfwGetTime();
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;
        cm.SetCameraSpeed(10.0f * dt);

        glad_glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glad_glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // ImGUI STuff
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (glfwGetKey(window.getGLFWhandle(), GLFW_KEY_W) == GLFW_PRESS) {
            cm.MoveFront();
        }
        if (glfwGetKey(window.getGLFWhandle(), GLFW_KEY_A) == GLFW_PRESS) {
            cm.MoveLeft();
        }
        if (glfwGetKey(window.getGLFWhandle(), GLFW_KEY_S) == GLFW_PRESS) {
            cm.MoveBack();
        }
        if(glfwGetKey(window.getGLFWhandle(), GLFW_KEY_D) == GLFW_PRESS) {
            cm.MoveRight();
        }

        if (glfwGetKey(window.getGLFWhandle(), GLFW_KEY_R) == GLFW_PRESS) {
            cm.MoveUp();
        }

        if (glfwGetKey(window.getGLFWhandle(), GLFW_KEY_F) == GLFW_PRESS) {
            cm.MoveDown();
        }

        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));

        // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)fb_width / (float)fb_height, 0.1f, 100.0f);

        // glm::mat4 view = cm.GetViewMatrix();

        // va.Bind();
        // shader1.use();
        // shader1.setMat4("model", model);
        // shader1.setMat4("view", view);
        // shader1.setMat4("projection", projection);
        // shader1.setVec2("u_resolution", glm::vec2(fb_width, fb_height));


        glm::vec2 halfSize = glm::vec2(2.0f, 1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(0.01f, 1.0f, 1.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)fb_width / (float)fb_height, 0.1f, 100.0f);
        glm::mat4 view = cm.GetViewMatrix();

        va.Bind();
        shader2.use();
        shader2.setMat4("projection", projection);
        shader2.setMat4("model", model);
        shader2.setMat4("view", view);
        glad_glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.flush();
    }
    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    // if (width == 0 || height == 0) return;
    // fb_width = width;
    // fb_height = height;
}
