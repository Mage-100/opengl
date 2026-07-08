add_library(core-template PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Window.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/VertexBuffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/VertexArray.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/VertexBufferLayout.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Framebuffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp"
)

target_include_directories(core-template PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Window.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/VertexBuffer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/VertexArray.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/VertexBufferLayout.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Camera.hpp"
)

target_link_libraries(core-template PUBLIC
    glm::glm
    glad::glad
    glfw
    imgui::imgui
)
