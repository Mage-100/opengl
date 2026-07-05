#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <shader.hpp>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
// platform.hpp or at the top of wherever this lives
#ifdef _WIN32
    #include <Windows.h>
#endif

#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// #include "Framebuffer.hpp"
#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "VertexBufferLayout.hpp"
#include "Camera.hpp"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

int fb_width = 0;
int fb_height = 0;

static float last_mouse_x = WINDOW_WIDTH / 2.0;
static float last_mouse_y = WINDOW_HEIGHT / 2.0;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void glfwMouseCallback(GLFWwindow* window, double xpos, double ypos);

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
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "opengl_glass-ui", NULL, NULL);

    if (window == NULL) {
        std::cerr << "Error: Failed to create GLFW Window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error: Failed to initialize OpenGL Context" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, glfwMouseCallback);
    // glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    std::filesystem::path shaderSourcePath = getShaderSourcePath();
    std::filesystem::path assetsSourcePath = getAssetSourcePath();

    Shader shader1(
        (shaderSourcePath / "ex_4/vertex.glsl").string().c_str(),
        (shaderSourcePath / "ex_4/fragment.glsl").string().c_str()
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

    Camera cm( glm::vec3(0.0f, 0.0f, 10.0f) );

    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    while(!glfwWindowShouldClose(window)) {

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

        // ImGUI STuff
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cm.MoveUp();
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cm.MoveLeft();
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cm.MoveDown();
        }
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cm.MoveRight();
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)fb_width / (float)fb_height, 0.1f, 100.0f);

        glm::mat4 view = cm.GetViewMatrix();

        va.Bind();
        shader1.use();
        shader1.setMat4("model", model);
        shader1.setMat4("view", view);
        shader1.setMat4("projection", projection);
        shader1.setVec2("u_resolution", glm::vec2(fb_width, fb_height));
        glad_glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    if (width == 0 || height == 0) return;
    fb_width = width;
    fb_height = height;
}

void glfwMouseCallback(GLFWwindow* window, double xpos, double ypos) {
    float xoffset = xpos - last_mouse_x;
    float yoffset = ypos - last_mouse_y;
    last_mouse_x = xpos;
    last_mouse_y = ypos;

    const float sensitivity = 0.1f;
    // std::printf("x: %f, y: %f\n", xoffset * sensitivity, yoffset * sensitivity);
}
