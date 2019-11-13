//
// Created by 李梦凡 on 2019/10/17.
// 接口块
//

//GLSL为我们提供了一个叫做接口块(Interface Block)的东西，来方便我们组合这些变量。接口块的声明和struct的声明有点相像，不同的是，现在根据它是一个输入还是输出块(Block)，使用in或out关键字来定义的。
//
//  1、 Vertex Shader
//
//#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoords;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//
//out VS_OUT
//{
//    vec2 TexCoords;
//} vs_out;
//
//void main()
//{
//    gl_Position = projection * view * model * vec4(aPos, 1.0);
//    vs_out.TexCoords = aTexCoords;
//}
//
//  2、 Fragment Shader
//
//#version 330 core
//out vec4 FragColor;
//
//in VS_OUT
//{
//    vec2 TexCoords;
//} fs_in;
//
//uniform sampler2D texture;
//
//void main()
//{
//    FragColor = texture(texture, fs_in.TexCoords);
//}