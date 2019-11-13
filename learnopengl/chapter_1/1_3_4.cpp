//
// Created by 李梦凡 on 2019/10/12.
// 绘制三角形：使用着色器工具类
//

#include <glad.h>
#include <glfw3.h>
#include <iostream>
#include "../utils/shader.h"

using namespace std;

void framebuffer_size_callback_1_3_4(GLFWwindow *window, int width, int height);

void processInput_1_3_4(GLFWwindow *window);

const unsigned int SCR_WIDTH_1_3_4 = 800;
const unsigned int SCR_HEIGHT_1_3_4 = 600;

int main_1_3_4() {
//    1. init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

//    2. create window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_1_3_4, SCR_HEIGHT_1_3_4, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_1_3_4);

//    3. load glad
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

//    4. build and compile our shader program
    Shader ourShader("../learnopengl/chapter_1/vshader_1_3_4.glsl", "../learnopengl/chapter_1/fshader_1_3_4.glsl");

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
        processInput_1_3_4(window);
//        6.2 render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
//        6.2.1 激活着色器
        ourShader.use();
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

void processInput_1_3_4(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback_1_3_4(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
