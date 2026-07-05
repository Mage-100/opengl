project(glass-ui)

add_library(core
    "Framebuffer.cpp"
    "VertexBuffer.cpp"
    "VertexArray.cpp"
    "VertexBufferLayout.cpp"
    "Camera.cpp"
)

target_link_libraries(core PUBLIC
    common_headers
    glm::glm
    glad::glad
    glfw
    imgui::imgui
)


if (CMAKE_BUILD_TYPE STREQUAL Debug )
target_compile_definitions(core PUBLIC
    DEBUG_BUILD
    SHADERS_SOURCE="${CMAKE_CURRENT_SOURCE_DIR}/shaders/"
    ASSETS_SOURCE="${CMAKE_CURRENT_SOURCE_DIR}/assets/"
)
endif()

file(GLOB EXAMPLES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/ex_*.cpp"
)


foreach(example ${EXAMPLES})
    message(STATUS "Found example: " ${example})
    get_filename_component(name ${example} NAME_WE)
    add_executable(${name} ${example})
    target_link_libraries(${name} PRIVATE
        core
    )
endforeach()
