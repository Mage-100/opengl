#version 330 core

in vec2 vLocalPos;

uniform vec2 uResolution;
uniform vec2 uHalfSize;
uniform float uCornerRadius;
uniform float uGlassThickness;
uniform float bezel_width;
uniform sampler2D bgTexture;

out vec4 FragColor;

float sdRoundedBox(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - b + r;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
}

float sdfSquircle(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - b + r;
    return pow( pow(max(q.x, 0.0), 4.0) + pow(max(q.y, 0.0), 4.0) ,0.25) + min(max(q.x, q.y),0.0) - r;
}

float SDF(vec2 p) {
    // return sdRoundedBox(p, uHalfSize, uCornerRadius);
    return sdfSquircle(p, uHalfSize, uCornerRadius);
}

float height_func(float t) {
    // return pow(1.0 - pow(1.0 - t, 2.0), 0.5);
    return pow(1.0 - pow(1.0 - t, 4.0), 0.25);
}

void main() {
    vec2 p = vLocalPos;
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

    // Lighting
    // vec3 lightColor = vec3(0.0, 0.0, 0.0); // Black Color
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    // Ambient Lighting
    float ambientStrenght = 0.025;
    vec3 ambient = ambientStrenght * lightColor;

    // Diffuse Lighting
    vec3 lightPos = vec3(-1.0, -1.0, 0.5);
    vec3 lightDir = normalize(lightPos);
    float diff = pow(max(dot(normal_3D, lightDir), 0.0), 3.0);
    vec3 diffuse = diff * lightColor;

    // Specular Lighting
    float specularStrength = 0.1;
    vec3 viewPos = vec3(0.0, 0.0, 1.0);
    vec3 viewDir = normalize(viewPos);
    vec3 reflectDir = reflect(-lightDir, normal_3D);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * lightColor;

    // Chromatic aberration — slightly different UV per channel
    float aberration = 0.001;
    vec2 redUV   = refracted_uv + vec2(aberration, aberration);
    vec2 greenUV = refracted_uv;
    vec2 blueUV  = refracted_uv - vec2(aberration, aberration);

    // Frosted blur on each channel separately
    vec3 blurred = vec3(0.0);
    float samples = 0.0;
    for (float x = -1.0; x <= 1.0; x++) {
        for (float y = -1.0; y <= 1.0; y++) {
            vec2 off = vec2(x, y) / uResolution;
            blurred.r += texture(bgTexture, redUV   + off).r;
            blurred.g += texture(bgTexture, greenUV + off).g;
            blurred.b += texture(bgTexture, blueUV  + off).b;
            samples += 0.9;
        }
    }
    vec4 bg = vec4(blurred / samples, 1.0);

    // --- Edge Fade ---
    float edgeFade = smoothstep(0.0, 4.0, -d);

    // --- Tint ---
    vec3 tint = vec3(1.0, 1.0, 1.0);
    float tintStrength = 0.05;
    vec3 tinted_bg = mix(bg.rgb, tint, tintStrength);

    // --- Inner Shadow ---
    // vLocalPos.y is negative at top, positive at bottom (screen-space Y flipped)
    float normalizedY = (vLocalPos.y / uHalfSize.y) * 0.5 + 0.5; // 0=top, 1=bottom
    float innerShadow = mix(0.85, 1.0, smoothstep(0.0, 1.0, normalizedY));

    // vec4 bg = texture(bgTexture, refracted_uv);
    vec3 lightingAggregate = ambient + diffuse + specular;
    vec3 glass_color = clamp(
        tinted_bg * innerShadow
        + lightingAggregate
        + (tint * tintStrength),
        0.0, 1.0
    );
    vec4 result_color = clamp(vec4(glass_color, 1.0), 0.0,1.0);
    FragColor = result_color;
    // FragColor = vec4(org_color, 1.0);
    // FragColor = vec4(bg.rgb, 1.0);
    // FragColor = vec4(abs(offset) / 50.0, 0.0, 1.0);
    // FragColor = vec4(normal_3D * 0.5 + 0.5, 1.0);
}
