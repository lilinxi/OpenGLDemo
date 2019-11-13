//
// Created by 李梦凡 on 2019/10/12.
// 绘制三角形：程序传入颜色信息（随时间变色）
//

//1. Uniform是一种从CPU中的应用向GPU中的着色器发送数据的方式，但uniform和顶点属性有些不同。首先，uniform是全局的(Global)。全局意味着uniform变量必须在每个着色器程序对象中都是独一无二的，而且它可以被着色器程序的任意着色器在任意阶段访问。第二，无论你把uniform值设置成什么，uniform会一直保存它们的数据，直到它们被重置或更新。

#include <glad.h>
#include <glfw3.h>
#include <iostream>
#include <cmath>

using namespace std;

void framebuffer_size_callback_1_3_2(GLFWwindow *window, int width, int height);

void processInput_1_3_2(GLFWwindow *window);

const unsigned int SCR_WIDTH_1_3_2 = 800;
const unsigned int SCR_HEIGHT_1_3_2 = 600;

const char *vertexShaderSource_1_3_2 = "#version 330 core\n"
                                       "layout (location = 0) in vec3 aPos;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                       "}\0";

const char *fragmentShaderSource_1_3_2 = "#version 330 core\n"
                                         "out vec4 FragColor;\n"
                                         "\n"
                                         "uniform vec4 ourColor; // 在OpenGL程序代码中设定这个变量\n"
                                         "\n"
                                         "void main()\n"
                                         "{\n"
                                         "    FragColor = ourColor;\n"
                                         "}";

int main_1_3_2() {
//    1. init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

//    2. create window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_1_3_2, SCR_HEIGHT_1_3_2, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_1_3_2);

//    3. load glad
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

//    4. build and compile our shader program
//    4.1 vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource_1_3_2, NULL);
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
    glShaderSource(fragmentShader, 1, &fragmentShaderSource_1_3_2, NULL);
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
        processInput_1_3_2(window);
//        6.2 render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
//        6.2.1 激活着色器
        glUseProgram(shaderProgram);
//        6.2.2 更新uniform颜色
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
//        我们用glGetUniformLocation查询uniform ourColor的位置值。如果glGetUniformLocation返回-1就代表没有找到这个位置值。
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
//        我们可以通过glUniform4f函数设置uniform值。
//        注意，查询uniform地址不要求你之前使用过着色器程序，但是更新一个uniform之前你必须先使用程序（调用glUseProgram)，因为它是在当前激活的着色器程序中设置uniform的。
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
//        6.2.3 绘制
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

void processInput_1_3_2(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback_1_3_2(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
