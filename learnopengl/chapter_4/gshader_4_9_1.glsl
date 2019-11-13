#version 330 core
layout (points) in;// 从顶点着色器接收的图元值
layout (triangle_strip, max_vertices = 5) out;// 几何着色器输出的图元类型，几何着色器同时希望我们设置一个它最大能够输出的顶点数量（如果你超过了这个值，OpenGL将不会绘制多出的顶点）。

//我们需要某种方式来获取前一着色器阶段的输出。GLSL提供给我们一个内建(Built-in)变量，在内部看起来（可能）是这样的：
//
//in gl_Vertex
//{
//    vec4  gl_Position;
//    float gl_PointSize;
//    float gl_ClipDistance[];
//} gl_in[];

//它被声明为一个接口块（Interface Block），它包含了几个很有意思的变量，其中最有趣的一个是gl_Position，它是和顶点着色器输出非常相似的一个向量。
//要注意的是，它被声明为一个数组，因为大多数的渲染图元包含多于1个的顶点，而几何着色器的输入是一个图元的所有顶点。
in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;

//将一个顶点->房子图元
void build_house(vec4 position)
{
    fColor = gs_in[0].color;// gs_in[0] since there's only one input vertex
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);// 1:bottom-left
    EmitVertex();// 每次我们调用EmitVertex时，gl_Position中的向量会被添加到图元中来。
    gl_Position = position + vec4(0.2, -0.2, 0.0, 0.0);// 2:bottom-right
    EmitVertex();
    gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0);// 3:top-left
    EmitVertex();
    gl_Position = position + vec4(0.2, 0.2, 0.0, 0.0);// 4:top-right
    EmitVertex();
    gl_Position = position + vec4(0.0, 0.4, 0.0, 0.0);// 5:top
    fColor = vec3(1.0, 1.0, 1.0);// 将最后一个顶点的颜色设置为白色，给屋顶落上一些雪。
    EmitVertex();
    EndPrimitive();// 当EndPrimitive被调用时，所有发射出的(Emitted)顶点都会合成为指定的输出渲染图元。
}

void main() {
    build_house(gl_in[0].gl_Position);
}