#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    vec4 texColor = texture(texture1, TexCoords); // 使用贴图的透明度
    if(texColor.a < 0.1) // 透明度低于一定值时舍弃
        discard;
    FragColor = texColor;
}