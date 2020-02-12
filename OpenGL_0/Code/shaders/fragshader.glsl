#version 330 core
in vec3 interpolatedColor;
out vec4 fColor;

void main()
{
    fColor = vec4(interpolatedColor, 1);
}
