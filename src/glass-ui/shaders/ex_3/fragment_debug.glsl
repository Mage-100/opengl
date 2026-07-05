#version 330 core

in vec2 vLocalPos;

uniform bool uDebugMode;
uniform int uNormFactor;
uniform vec2 uResolution;
uniform vec2 uHalfSize;
uniform float uCornerRadius;
uniform float uGlassThickness;
uniform float bezel_width;
uniform float refractive_index;
uniform sampler2D bgTexture;

out vec4 FragColor;

#define sq(x) pow(x,2.0)

float doubleCircleSigmoid (float x, float a){
  float min_param_a = 0.0;
  float max_param_a = 1.0;
  a = max(min_param_a, min(max_param_a, a));

  float y = 0;
  if (x<=a){
    y = a - sqrt(a*a - x*x);
  } else {
    y = a + sqrt(sq(1-a) - sq(x-1));
  }
  return y;
}

float sdfRoundedBox(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - b + r;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
}

float sdfSquircle(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - b + r;
    return pow( pow(max(q.x, 0.0), uNormFactor) + pow(max(q.y, 0.0), uNormFactor) , 1.0 / uNormFactor) + min(max(q.x, q.y),0.0) - r;
}

float SDF(vec2 p) {
    return sdfSquircle(p, uHalfSize, uCornerRadius);
}

float height_func(float t) {
    return pow(1.0 - pow(1.0 - t, uNormFactor), 1.0 / uNormFactor);
    // return doubleCircleSigmoid(t, 0.3);
}

void main() {
    vec2 p = vLocalPos * uHalfSize;
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
    float eta = 1.0 / refractive_index;
    vec3 I = vec3(0.0, 0.0, -1.0);
    // vec3 T = refract(I, normal_3D, eta);
    vec3 T = normalize(refract(I, normal_3D, eta));

    // Ray-plane intersection to get pixel offset
    float Pz = height_func(t) * uGlassThickness;
    vec2 offset = (T.xy / abs(T.z)) * Pz;

    // Sample background
    vec2 screen_uv = gl_FragCoord.xy / uResolution;
    vec2 refracted_uv = clamp(screen_uv + vec2(offset.x, -offset.y) / uResolution, 0.0, 1.0);

    // Debugging
    vec4 debug_col = vec4(0.0);
    if (d>=-bezel_width) {
        debug_col = vec4(1.0, 0.0, 0.0, 0.0);
    }

    vec4 bg = texture(bgTexture, refracted_uv);
    vec4 result_color = vec4(0.0);
    if (uDebugMode == true) {
        result_color = clamp(bg + debug_col, 0.0,1.0);
    } else {
        result_color = clamp(bg, 0.0,1.0);
    }
    FragColor = result_color;
}
