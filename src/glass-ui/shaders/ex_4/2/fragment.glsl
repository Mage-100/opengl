#version 330 core

out vec4 FragColor;

float sdf(vec2 p, vec2 q, float r) {
    vec2 b = abs(p) - q + r;

    float outside = length(max(b, 0.0));
    float inside = min(max(b.x, b.y), 0.0);

    return outside + inside - r;
}

void main() {
    FragColor = vec4(0.8, 0.8, 0.8, 1.0);
}
