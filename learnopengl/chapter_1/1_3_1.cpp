//
// Created by 李梦凡 on 2019/10/12.
// 绘制三角形：着色器传递颜色信息
//

//1. 当我们特别谈论到顶点着色器的时候，每个输入变量也叫顶点属性(Vertex Attribute)。我们能声明的顶点属性是有上限的，它一般由硬件来决定。OpenGL确保至少有16个包含4分量的顶点属性可用，但是有些硬件或许允许更多的顶点属性，你可以查询GL_MAX_VERTEX_ATTRIBS来获取具体的上限：

//2. GLSL定义了in和out关键字专门来实现这个目的。每个着色器使用这两个关键字设定输入和输出，只要一个输出变量与下一个着色器阶段的输入匹配，它就会传递下去。

#include <glad.h>
#include <glfw3.h>
#include <iostream>

using namespace std;

void framebuffer_size_callback_1_3_1(GLFWwindow *window, int width, int height);

void processInput_1_3_1(GLFWwindow *window);

const unsigned int SCR_WIDTH_1_3_1 = 800;
const unsigned int SCR_HEIGHT_1_3_1 = 600;

const char *vertexShaderSource_1_3_1 = "#version 330 core\n"
                                       "layout (location = 0) in vec3 aPos; // 位置变量的属性位置值为0\n"
                                       "\n"
                                       "out vec4 vertexColor; // 为片段着色器指定一个颜色输出\n"
                                       "\n"
                                       "void main()\n"
                                       "{\n"
                                       "    gl_Position = vec4(aPos, 1.0); // 注意我们如何把一个vec3作为vec4的构造器的参数\n"
                                       "    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // 把输出变量设置为暗红色\n"
                                       "}";

const char *fragmentShaderSource_1_3_1 = "#version 330 core\n"
                                         "out vec4 FragColor;\n"
                                         "\n"
                                         "in vec4 vertexColor; // 从顶点着色器传来的输入变量（名称相同、类型相同）\n"
                                         "\n"
                                         "void main()\n"
                                         "{\n"
                                         "    FragColor = vertexColor;\n"
                                         "}";

int main_1_3_1() {
//    1. init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

//    2. create window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_1_3_1, SCR_HEIGHT_1_3_1, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_1_3_1);

//    3. load glad
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }
//    通常情况下它至少会返回16个，大部分情况下是够用了。
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    cout << "Maximum nr of vertex attributes supported: " << nrAttributes << endl;

//    4. build and compile our shader program
//    4.1 vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource_1_3_1, NULL);
    glCompileShader(vertexShader);
//    check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }
//    4.2 fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource_1_3_1, NULL);
    glCompileShader(fragmentShader);
//    check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }
//    4.3 attach and link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
//    check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }
//    4.4 delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

//    5. store triangle points
    float vertices[] = {
            -0.5f, -0.5f, 0.0f, // left
            0.5f, -0.5f, 0.0f, // right
            0.0f, 0.5f, 0.0f  // top
    };
//    5.1 创建顶点数组对象，顶点缓冲对象
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
//    5.2 绑定缓冲
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(VAO);
//    5.3 链接顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
//    5.4 unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

//    6. 渲染循环
    while (!glfwWindowShouldClose(window)) {
//        6.1. input
        processInput_1_3_1(window);
//        6.2 render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
//        6.2.1 激活着色器
        glUseProgram(shaderProgram);
//        6.2.2 绘制
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
//        6.4 交换颜色缓冲
        glfwSwapBuffers(window);
//        6.5 检查有没有触发什么事件
        glfwPollEvents();
    }

//    7. delete resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
//    glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

void processInput_1_3_1(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback_1_3_1(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
