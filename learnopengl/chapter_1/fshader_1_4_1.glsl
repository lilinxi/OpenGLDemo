#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture; // 片段着色器也应该能访问纹理对象，GLSL有一个供纹理对象使用的内建数据类型，叫做采样器(Sampler)。

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}