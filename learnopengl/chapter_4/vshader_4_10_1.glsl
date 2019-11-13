#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset;

out vec3 fColor;

void main()
{
    fColor = aColor;
//    我们不再使用gl_InstanceID，现在不需要索引一个uniform数组就能够直接使用offset属性了。
//    gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
//    使用gl_InstanceID逐步放大四边形
    vec2 pos = aPos * (gl_InstanceID / 100.0);
    gl_Position = vec4(pos + aOffset, 0.0, 1.0);
}