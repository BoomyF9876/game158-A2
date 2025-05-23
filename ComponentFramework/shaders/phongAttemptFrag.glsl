#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;

// These are coming IN from the vert shader
in vec3 lightDir;
in vec3 normalDir;

void main() {
    //                    r    b    g    alpha
    vec4 mainColor = vec4(1.0, 0.4, 0.4, 0.0);

    float ambientFactor = 0.2;
    vec4 ambient = mainColor * ambientFactor;

    vec4 lightColor = vec4(1 , 1, 1, 0);
    
    // Lucas says, if the angle between the
    // light dir and the normal is less than
    // 90 degrees, then it is in the LIGHT!

    // JP taught us how to find the angle using the dot
    // A dot B = cos(angle), if A and B are normalized
    float normalDotLight = dot(normalDir, lightDir);
    float angleRadians = acos(normalDotLight);
    float angleDegrees = angleRadians * 180 / 3.141592;

    vec4 diffuse = vec4(0.0, 0, 0, 0);
    float diffuseFactor = 0.7;

    if(angleDegrees <= 90){
        diffuse = mainColor * diffuseFactor;
    }
    // Lucas's method above, kinda looks like toon shading
    // How do we shade gradually...
    // The cosine looks nice an gradual. Use that!
    if(angleDegrees <= 90){
        float cosine = normalDotLight;
       diffuse = mainColor * diffuseFactor * cosine;
    }





    fragColor = ambient + diffuse;
}