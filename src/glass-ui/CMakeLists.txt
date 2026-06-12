project(glass-ui)

add_executable(glass-ui main.cpp)


target_link_libraries(glass-ui PRIVATE
    common_headers
    glm::glm
    glad::glad
    glfw
)
