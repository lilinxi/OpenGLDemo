#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
    //    折射
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(Position - cameraPos);
    //    折射可以使用GLSL的内建refract函数来轻松实现，它需要一个法向量、一个观察方向和两个材质之间的折射率(Refractive Index)。
    vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}