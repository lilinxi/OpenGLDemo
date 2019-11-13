//
// Created by 李梦凡 on 2019/10/12.
// 绘制三角形：顶点自带颜色信息（颜色插值）
//

//1. 把颜色数据加进顶点数据中。

#include <glad.h>
#include <glfw3.h>
#include <iostream>

using namespace std;

void framebuffer_size_callback_1_3_3(GLFWwindow *window, int width, int height);

void processInput_1_3_3(GLFWwindow *window);

const unsigned int SCR_WIDTH_1_3_3 = 800;
const unsigned int SCR_HEIGHT_1_3_3 = 600;

//由于现在有更多的数据要发送到顶点着色器，我们有必要去调整一下顶点着色器，使它能够接收颜色值作为一个顶点属性输入。需要注意的是我们用layout标识符来把aColor属性的位置值设置为1：
const char *vertexShaderSource_1_3_3 = "#version 330 core\n"
                                       "layout (location = 0) in vec3 aPos;   // 位置变量的属性位置值为 0 \n"
                                       "layout (location = 1) in vec3 aColor; // 颜色变量的属性位置值为 1\n"
                                       "\n"
                                       "out vec3 ourColor; // 向片段着色器输出一个颜色\n"
                                       "\n"
                                       "void main()\n"
                                       "{\n"
                                       "    gl_Position = vec4(aPos, 1.0);\n"
                                       "    ourColor = aColor; // 将ourColor设置为我们从顶点数据那里得到的输入颜色\n"
                                       "}";

const char *fragmentShaderSource_1_3_3 = "#version 330 core\n"
                                         "out vec4 FragColor;  \n"
                                         "in vec3 ourColor;\n"
                                         "\n"
                                         "void main()\n"
                                         "{\n"
                                         "    FragColor = vec4(ourColor, 1.0);\n"
                                         "}";

int main_1_3_3() {
//    1. init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

//    2. create window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_1_3_3, SCR_HEIGHT_1_3_3, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_1_3_3);

//    3. load glad
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

//    4. build and compile our shader program
//    4.1 vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource_1_3_3, NULL);
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
    glShaderSource(fragmentShader, 1, &fragmentShaderSource_1_3_3, NULL);
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
            // 位置              // 颜色
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // 右下
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // 左下
            0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // 顶部
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
// 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
// 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
//    5.4 unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

//    6. 渲染循环
    while (!glfwWindowShouldClose(window)) {
//        6.1. input
        processInput_1_3_3(window);
//        6.2 render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
//        6.2.1 激活着色器
        glUseProgram(shaderProgram);
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

void processInput_1_3_3(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback_1_3_3(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
