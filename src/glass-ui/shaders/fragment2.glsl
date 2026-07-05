#version 330 core

in vec2 vLocalPos;

uniform vec2 uResolution;
uniform vec2 uHalfSize;
uniform float uCornerRadius;
uniform float uGlassThickness;  // add this uniform, set to ~50.0
uniform sampler2D bgTexture;

out vec4 FragColor;

float sdRoundedBox(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - b + r;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
}

float SDF(vec2 p) {
    return sdRoundedBox(p, uHalfSize, uCornerRadius);
}

float height_func(float t) {
    return pow(1.0 - pow(1.0 - t, 2.0), 0.5);
}

void main() {
    vec2 p = vLocalPos;
    float bezel_width = 20.0;
    float d = SDF(p);

    // Discard outside the shape
    if (d > 0.0) discard;

    // SDF gradient — EPSILON must be in pixel scale
    float EPSILON = 0.5;
    vec2 gradient = vec2(
        SDF(p + vec2(EPSILON, 0.0)) - SDF(p - vec2(EPSILON, 0.0)),
        SDF(p + vec2(0.0, EPSILON)) - SDF(p - vec2(0.0, EPSILON))
    );
    vec2 normal_2D = normalize(gradient);

    // Height and its derivative
    float t = clamp(-d / bezel_width, 0.0, 1.0);
    float dt = 0.01;
    float dh = (height_func(clamp(t + dt, 0.0, 1.0)) - height_func(clamp(t - dt, 0.0, 1.0))) / (2.0 * dt);
    dh = dh / bezel_width; // chain rule

    // 3D surface normal
    vec3 normal_3D = normalize(vec3(
        dh * normal_2D.x,
        dh * normal_2D.y,
        1.0
    ));

    // Refraction
    float eta = 1.0 / 2.5;
    vec3 I = vec3(0.0, 0.0, -1.0);
    vec3 T = refract(I, normal_3D, eta);

    // Ray-plane intersection to get pixel offset
    float Pz = height_func(t) * uGlassThickness;
    vec2 offset = (T.xy / abs(T.z)) * Pz;

    // Sample background
    vec2 screen_uv = gl_FragCoord.xy / uResolution;
    vec2 refracted_uv = clamp(screen_uv + offset / uResolution, 0.0, 1.0);
    // vec2 refracted_uv = clamp(screen_uv + vec2(0.05, 0.0), 0.0, 1.0);

    //Test
    // Light and view directions (both in camera space, viewer at +Z)
    vec3 L = normalize(vec3(-1.0, 1.0, 0.0));  // light from above-front
    vec3 V = vec3(0.0, 0.0, 1.0);             // viewer looking in +Z
    vec3 H = normalize(L + V);                 // halfway vector

    // Blinn-Phong specular
    float spec = pow(max(dot(normal_3D, H), 0.0), 64.0);  // 64 = shininess

    // Rim — only on the bezel, fades toward interior
    float rim = (1.0 - t);  // t=0 at border, t=1 at interior
    spec *= rim;

    vec4 bg = texture(bgTexture, refracted_uv);
    vec3 specColor = vec3(1.0, 1.0, 1.0);
    FragColor = vec4(bg.rgb + (spec * specColor), 1.0);
    // FragColor = vec4(bg.rgb, 1.0);
    // FragColor = vec4(abs(offset) / 50.0, 0.0, 1.0);
    // FragColor = vec4(normal_3D * 0.5 + 0.5, 1.0);
}
