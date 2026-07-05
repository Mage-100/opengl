#include <iostream>
#include <shader.hpp>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
// platform.hpp or at the top of wherever this lives
#ifdef _WIN32
    #include <Windows.h>
#endif

#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Framebuffer.hpp"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

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

    glEnable(GL_DEPTH_TEST);

    std::filesystem::path shaderSourcePath = getShaderSourcePath();
    std::filesystem::path assetsSourcePath = getAssetSourcePath();
    std::filesystem::path vertex_shader_path = shaderSourcePath / "ex_1/vertex.glsl";
    std::filesystem::path fragment_shader_path = shaderSourcePath / "ex_1/fragment.glsl";
    Shader shaderProgram(
        vertex_shader_path.string().c_str(),
        fragment_shader_path.string().c_str()
    );

    std::filesystem::path screenVertexShaderPath = shaderSourcePath / "screenVertex.glsl";
    std::filesystem::path screenFragmentShaderPath = shaderSourcePath / "screenFragment.glsl";
    Shader screenShader(
        screenVertexShaderPath.string().c_str(),
        screenFragmentShaderPath.string().c_str()
    );

    std::filesystem::path bgVertexShaderPath = shaderSourcePath / "vertexBG.glsl";
    std::filesystem::path bgFragmentShaderPath = shaderSourcePath / "fragmentBG.glsl";
    Shader bgShader(
        bgVertexShaderPath.string().c_str(),
        bgFragmentShaderPath.string().c_str()
    );

    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int screenVBO, screenVAO;

    glad_glGenBuffers(1, &screenVBO);
    glad_glGenVertexArrays(1, &screenVAO);
    glad_glBindVertexArray(screenVAO);

    glad_glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glad_glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glad_glEnableVertexAttribArray(0);
    glad_glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glad_glEnableVertexAttribArray(1);
    glad_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
    glad_glBindVertexArray(0);


    // 300,300        600,300
    // 300,500      600,500

    float vertices[] = {
        // x      y      z
        300.0f, 300.0f, 0.0f, // Top-Left
        600.0f, 300.0f, 0.0f, // Top-Right
        600.0f, 500.0f, 0.0f, // Bottom-Right

        600.0f, 500.0f, 0.0f, // Bottom-Right
        300.0f, 500.0f, 0.0f, // Bottom-Left
        300.0f, 300.0f, 0.0f // Top-Left
    };

    unsigned int VBO, VAO;

    glad_glGenBuffers(1, &VBO);
    glad_glGenVertexArrays(1, &VAO);
    glad_glBindVertexArray(VAO);

    glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glad_glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glad_glEnableVertexAttribArray(0);
    glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
    glad_glBindVertexArray(0);

    int fb_width = 0;
    int fb_height = 0;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    Framebuffer fb_glass(fb_width, fb_height);
    Framebuffer fb_bg(fb_width, fb_height);

    std::cout << "assetSource: " << assetsSourcePath.string().c_str() << std::endl;
    std::filesystem::path imagePath = assetsSourcePath / "grid_color.png";
    int x,y,n;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *bgData = stbi_load(imagePath.string().c_str(), &x, &y, &n, 4);
    unsigned int bgTexture;
    glad_glGenTextures(1, &bgTexture);
    glad_glBindTexture(GL_TEXTURE_2D, bgTexture);
    glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, bgData);
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    while(!glfwWindowShouldClose(window)) {
        // Render the background to a framebuffer
        fb_bg.bind();
        screenShader.use();
        glad_glBindVertexArray(screenVAO);
        glad_glBindTexture(GL_TEXTURE_2D, bgTexture);
        glad_glDrawArrays(GL_TRIANGLES, 0, 6);

        fb_glass.bind();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        glm::mat4 projection;
        projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
        shaderProgram.use();
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindTexture(GL_TEXTURE_2D, fb_bg.getTexture());
        shaderProgram.setInt("bgTexture", 0);
        shaderProgram.setMat4("projection", projection);
        shaderProgram.setVec2("uCenter", glm::vec2(450, 400));
        shaderProgram.setVec2("uHalfSize", glm::vec2(150,100));
        // bezel_width / uGlassThi
        // shaderProgram.setFloat("uGlassThickness", 1000.0);
        // shaderProgram.setFloat("uGlassThickness", 350.0);
        // shaderProgram.setFloat("bezel_width", 10.0);
        shaderProgram.setFloat("uGlassThickness", 1050.0);
        shaderProgram.setFloat("bezel_width", 30.0);
        shaderProgram.setFloat("uCornerRadius", 100);
        shaderProgram.setVec2("uResolution", glm::vec2(fb_width, fb_height));
        glad_glBindVertexArray(VAO);
        glad_glDrawArrays(GL_TRIANGLES, 0, 6);

        // second pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glad_glBindVertexArray(screenVAO);
        glad_glDisable(GL_DEPTH_TEST);
        glad_glDisable(GL_BLEND);
        glad_glBindTexture(GL_TEXTURE_2D, fb_bg.getTexture());
        glad_glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glad_glBindTexture(GL_TEXTURE_2D, fb_glass.getTexture());
        glad_glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
