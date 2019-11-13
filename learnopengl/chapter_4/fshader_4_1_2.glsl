#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    //    FragColor = texture(texture1, TexCoords);
    //    gl_FragCoord的x和y分量代表了片段的屏幕空间坐标（其中(0, 0)位于左下角）。gl_FragCoord中也包含了一个z分量，它包含了片段真正的深度值。
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}