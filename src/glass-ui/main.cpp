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

    glEnable(GL_DEPTH_TEST);

    std::filesystem::path exe_path = exe_dir();
    std::filesystem::path vertex_shader_path = exe_path / "shaders" / "vertex.glsl";
    std::filesystem::path fragment_shader_path = exe_path / "shaders" / "fragment.glsl";
    Shader shaderProgram(
        vertex_shader_path.string().c_str(),
        fragment_shader_path.string().c_str()
    );

    std::filesystem::path screenVertexShaderPath = exe_path / "shaders" / "screenVertex.glsl";
    std::filesystem::path screenFragmentShaderPath = exe_path / "shaders" / "screenFragment.glsl";
    Shader screenShader(
        screenVertexShaderPath.string().c_str(),
        screenFragmentShaderPath.string().c_str()
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


    // 300,200      600,200
    // 300,400      600,400

    float vertices[] = {
        // x      y      z
        300.0f, 200.0f, 0.0f, // Top-Left
        600.0f, 200.0f, 0.0f, // Top-Right
        600.0f, 400.0f, 0.0f, // Bottom-Right

        600.0f, 400.0f, 0.0f, // Bottom-Right
        300.0f, 400.0f, 0.0f, // Bottom-Left
        300.0f, 200.0f, 0.0f // Top-Left
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

    unsigned int fbo;
    glad_glGenFramebuffers(1, &fbo);
    glad_glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    unsigned int texture;
    glad_glGenTextures(1, &texture);
    glad_glBindTexture(GL_TEXTURE_2D, texture);
    glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fb_width, fb_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glad_glBindTexture(GL_TEXTURE_2D, 0);

    glad_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    unsigned int rbo;
    glad_glGenRenderbuffers(1, &rbo);
    glad_glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glad_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb_width, fb_height);
    // glad_glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glad_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Yaaay!!!! Framebuffer was created." << std::endl;
    } else {
        std::cerr << "Sorry Framebuffer could not be created." << std::endl;
        glad_glBindFramebuffer(GL_FRAMEBUFFER, 0);
        exit(EXIT_FAILURE);
    }

    glad_glBindFramebuffer(GL_FRAMEBUFFER, 0);



    while(!glfwWindowShouldClose(window)) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        glm::mat4 projection;
        projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

        shaderProgram.use();
        shaderProgram.setMat4("projection", projection);
        glad_glBindVertexArray(VAO);
        glad_glDrawArrays(GL_TRIANGLES, 0, 6);

        // second pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glad_glBindVertexArray(screenVAO);
        glad_glDisable(GL_DEPTH_TEST);
        glad_glBindTexture(GL_TEXTURE_2D, texture);
        glad_glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
