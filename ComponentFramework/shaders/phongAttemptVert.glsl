#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 inVertex;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUVCoord;

layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 modelMatrix;


// For diffuse color we need
// light direction from the vert position to the light position
// and the normal
out vec3 lightDir;
out vec3 normalDir;

void main() {

    // Minas says we should put the light up to the right 
    // And positive z
    vec3 lightPos = vec3(5, 5, 5);

    // A to B is B - A
    // Vertex to light is light - vertex
    lightDir = lightPos - inVertex;
    // Best to have directions normalized so they have a mag of one
    lightDir = normalize(lightDir);

    // Send over the normals to the frag as well
    normalDir = inNormal;


    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inVertex, 1.0);
}