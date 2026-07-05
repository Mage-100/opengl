#version 330 core
out vec4 FragColor;

uniform vec2 u_resolution;

void main() {
    vec2 uv = gl_FragCoord.xy / u_resolution;

    FragColor = vec4(uv.x, uv.y, 0.0, 1.0);
}
