//
// Created by 李梦凡 on 2019/10/9.
// 创建窗口
//

//1. glfwWindowHint
//https://www.glfw.org/docs/latest/window.html#window_hints

#include <glad.h>
#include <glfw3.h>
#include <iostream>

using namespace std;

//改变窗口大小需要改变视口大小的回调函数
void framebuffer_size_callback_1_1(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

//处理输入
void processInput_1_1(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main_1_1() {
//    1. 实例化GLFW窗口
    glfwInit();
//    设置版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    使用核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Max OS X
//    2. 创建窗口对象
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
//    3. 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }
//    4. 定义渲染窗口的视口大小
    glViewport(0, 0, 800, 600);
//    注册回调函数
//    当窗口被第一次显示的时候framebuffer_size_callback也会被调用。对于视网膜(Retina)显示屏，width和height都会明显比原输入值更高一点。
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_1_1);
//    5. 渲染循环
    while (!glfwWindowShouldClose(window)) { //在每次循环的开始前检查一次GLFW是否被要求退出
//        5.1 处理输入
        processInput_1_1(window);
//        5.2 渲染指令
//        可以使用 | 运算符组合不同的缓冲标志位，表明需要清除的缓冲：颜色，深度，累积，模板缓冲
        glClearColor(0.2, 0.3, 0.3, 1);
        glClear(GL_COLOR_BUFFER_BIT);
//        5.3 交换颜色缓冲
        glfwSwapBuffers(window);
//        5.4 检查有没有触发什么事件
        glfwPollEvents();
    }
//    6. 释放资源
    glfwTerminate();
    return 0;
}