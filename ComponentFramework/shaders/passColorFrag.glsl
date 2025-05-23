#version 450
#extension GL_ARB_separate_shader_objects : enable

// Scott taught me that this will default
// to vec4(0, 0, 0, 0) if I don't set it in render
uniform vec4 color;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = color;
}