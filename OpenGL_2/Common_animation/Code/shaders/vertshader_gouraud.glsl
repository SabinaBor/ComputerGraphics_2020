#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 normColor_in;
layout (location = 2) in vec2 texCoordinates_in;


// Specify the Uniforms of the vertex shader
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;

//uniform vec3 lightPosition;

// Specify the output of the vertex stage
out vec3 gouraudColor;
out vec2 texCoords;

void main()
{
    gl_Position = projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0);
    vec3 FragPos = vec3(-10, 10, 0);
    vec3 P = vec3(modelViewTransform * vec4(vertCoordinates_in, 1.0));
    vec3 N = normalize(normalTransform * normColor_in);
    vec3 L = normalize(FragPos - P);
    vec3 R = -reflect(L, N);
    vec3 V = normalize(-P);
    vec3 lightColor = vec3(1, 1, 1);
    vec3 materialColor = vec3(1, 1, 1);
    float material[3] = float[3](0.2, 0.8, 0.6);
    vec3 ambient = lightColor * materialColor.x * material[0];
    vec3 diffuse = max(0.0, dot(N, L)) * lightColor * materialColor.y * material[1];
    vec3 specular = material[2] * materialColor.z * pow(max(0.0, dot(R, V)), 2) * lightColor;

    gouraudColor = ambient + diffuse + specular;
    texCoords = texCoordinates_in;
}

