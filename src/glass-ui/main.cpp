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

    std::filesystem::path vertex_shader_path = exe_dir() / "shaders" / "vertex.glsl";
    std::filesystem::path fragment_shader_path = exe_dir() / "shaders" / "fragment.glsl";
    Shader shaderProgram(
        vertex_shader_path.string().c_str(),
        fragment_shader_path.string().c_str()
    );

    float vertices[] = {
        // x      y      z
        0.0f, 0.0f, 0.0f, // Top-Left
        500.0f, 100.0f, 0.0f, // Top-Right
        500.0f, 500.0f, 0.0f, // Bottom-Right

        500.0f, 500.0f, 0.0f, // Bottom-Right
        100.0f, 500.0f, 0.0f, // Bottom-Left
        0.0f, 0.0f, 0.0f // Top-Left
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

    shaderProgram.use();
    while(!glfwWindowShouldClose(window)) {
        glad_glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 projection;
        projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

        shaderProgram.setMat4("projection", projection);

        glad_glBindVertexArray(VAO);
        glad_glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
