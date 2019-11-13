//1. https://tanwenbo.top/course/mac-shi-yongclion-pei-zhiopenglglewgladglfw.html
//brew install glew
//brew install glfw
//glad：http://glad.dav1d.de/
//include and CMake

//2. https://blog.csdn.net/feng_ye2015/article/details/86598071
//#include <KHR/khrplatform.h>改为#include "../KHR/khrplatform.h"

//3. glad和glew：http://blog.shenyuanluo.com/GLGenbuffers.html
//GLAD 和 GLEW 的作用都是帮助在开发 OpenGL 时简化其在 运行时 获取函数的内存地址并将其保存在一个函数指针中供以使用的流程。

//4. glfw：GLFW是一个专门针对OpenGL的C语言库，它提供了一些渲染物体所需的最低限度的接口。它允许用户创建OpenGL上下文，定义窗口参数以及处理用户输入，这正是我们需要的。

#include <glew.h>
#include <glfw3.h>
#include <iostream>

using namespace std;

void key_callback_0(GLFWwindow *window, int key, int scancode, int action, int mode) {
    //如果按下ESC，把windowShouldClose设置为True，外面的循环会关闭应用
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    std::cout << "ESC" << mode;
}

int main_0(void) {
    //初始化GLFW库
    if (!glfwInit())
        return -1;
    //创建窗口以及上下文
    GLFWwindow *window = glfwCreateWindow(640, 480, "hello world", NULL, NULL);
    if (!window) {
        //创建失败会返回NULL
        glfwTerminate();
    }
    //建立当前窗口的上下文
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback_0); //注册回调函数
    //循环，直到用户关闭窗口
    while (!glfwWindowShouldClose(window)) {
        /*******轮询事件*******/
        glfwPollEvents();

        /*******渲染*******/
        //选择清空的颜色RGBA
        glClearColor(0.2, 0.3, 0.3, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        //开始画一个三角形
        glBegin(GL_TRIANGLES);
        glColor3f(1, 0, 0); //Red
        glVertex3f(0, 1, 1);

        glColor3f(0, 1, 0); //Green
        glVertex3f(-1, -1, 0);

        glColor3f(0, 0, 1); //Blue
        glVertex3f(1, -1, 0);
        //结束一个画图步骤
        glEnd();

        glBegin(GL_POLYGON);
        //再画个梯形，需要注意笔顺
        glColor3f(0.5, 0.5, 0.5); //Grey
        glVertex2d(0.5, 0.5);
        glVertex2d(1, 1);
        glVertex2d(1, 0);
        glVertex2d(0.5, 0);
        glEnd();


        /******交换缓冲区，更新window上的内容******/
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}