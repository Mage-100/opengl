#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 vLocalPos;

uniform mat4 projection;
uniform vec2 uCenter;

void main() {

    gl_Position = projection * vec4(aPos, 1.0);
    vLocalPos = aPos.xy - uCenter;
}
