project(glass-ui)

add_executable(glass-ui main.cpp)


target_link_libraries(glass-ui PRIVATE
    common_headers
    glm::glm
    glad::glad
    glfw
)

set(GLASS_UI_SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/screenVertex.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/screenFragment.glsl"
)

set(GLASS_UI_SHADER_OUTPUTS "")
foreach(SHADER ${GLASS_UI_SHADERS})
    file(RELATIVE_PATH REL "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${SHADER}")
    set(OUT "${CMAKE_CURRENT_BINARY_DIR}/shaders/${REL}")

    add_custom_command(
        OUTPUT ${OUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SHADER}" "${OUT}"
        DEPENDS "${SHADER}"
        COMMENT "Copying Shader: ${REL}"
    )
    list(APPEND GLASS_UI_SHADER_OUTPUTS ${OUT})
endforeach()

add_custom_target(glass_ui_shaders ALL
    DEPENDS ${GLASS_UI_SHADER_OUTPUTS}
)

add_custom_target(TARGET glass-ui POST_BUILD
    COMMAND  ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:glass-ui>/shaders"
    DEPENDS ${GLASS_UI_SHADERS}
    COMMENT "Staging shaders alongside glass-ui executable"
)

add_dependencies(glass-ui glass_ui_shaders)
