#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 phongColor;
in vec3 vertCoordinates;
in vec2 texCoords;

// Specify the Uniforms of the fragment shaders
// uniform vec3 lightPosition; // for example
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;

uniform sampler2D uniSampler;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main()
{
    vec4 texColor = texture(uniSampler, texCoords);
    vec3 FragPos = vec3(-10, 10, 0);
    vec3 P = vec3(modelViewTransform * vec4(vertCoordinates, 1.0));
    vec3 N = normalize(normalTransform * phongColor);
    vec3 L = normalize(FragPos - P);
    vec3 R = -reflect(L, N);
    vec3 V = normalize(-P);
    vec3 lightColor = vec3(1, 1, 1);
    vec3 materialColor = vec3(1, 1, 1);
    float material[3] = float[3](0.2, 0.8, 0.6);
    vec3 ambient = lightColor * materialColor.x * material[0];
    vec3 diffuse = max(0.0, dot(N, L)) * lightColor * materialColor.y * material[1];
    vec3 specular = material[2] * materialColor.z * pow(max(0.0, dot(R, V)), 2) * lightColor;

    fColor = texColor * vec4(ambient + diffuse + specular,1.0);
}
