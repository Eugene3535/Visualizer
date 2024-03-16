#version 460 core

out vec4 FragColor;

in vec4 outColor;

void main()
{
    // FragColor = texture2D(texture0, tex_coord) * outColor;
    FragColor = vec4(0, 1, 1, 1);
}