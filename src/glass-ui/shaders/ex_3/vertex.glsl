#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 vLocalPos;

uniform mat4 projection;
uniform mat4 model;
uniform vec2 uCenter;

void main() {

    vec4 shape = model * vec4(aPos, 1.0);
    gl_Position = projection * shape;
    // vLocalPos = shape.xy - uCenter;
    vLocalPos = aPos.xy;
}
