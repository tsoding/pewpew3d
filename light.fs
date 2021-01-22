#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

void main() {
    finalColor = mix(
        vec4(0.0, 0.0, 0.0, 1.0),
        fragColor,
        min(gl_FragCoord.w, 1.0));
}
