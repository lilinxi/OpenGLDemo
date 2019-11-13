/*
//
// Created by 李梦凡 on 2019/10/18.
// 法向量可视化
//

//1. 法向量可视化
//    1、 检测法向量错误
//    2、 给物体添加毛发

//2. 法向量可视化（思路）
//我们首先不使用几何着色器正常绘制场景。然后再次绘制场景，但这次只显示通过几何着色器生成法向量。几何着色器接收一个三角形图元，并沿着法向量生成三条线——每个顶点一个法向量。

#include <glad.h>
#include <glfw3.h>

#include <../glm/glm.hpp>
#include <../glm/gtc/matrix_transform.hpp>

#include "../utils/shader.h"
#include "../utils/camera.h"

//Warn 只能include一个model
#include "../utils/model.h"

#include <iostream>

void framebuffer_size_callback_4_9_3(GLFWwindow *window, int width, int height);

void mouse_callback_4_9_3(GLFWwindow *window, double xpos, double ypos);

void scroll_callback_4_9_3(GLFWwindow *window, double xoffset, double yoffset);

void processInput_4_9_3(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH_4_9_3 = 1280;
const unsigned int SCR_HEIGHT_4_9_3 = 720;

// camera
Camera camera_4_9_3(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX_4_9_3 = (float) SCR_WIDTH_4_9_3 / 2.0;
float lastY_4_9_3 = (float) SCR_HEIGHT_4_9_3 / 2.0;
bool firstMouse_4_9_3 = true;

// timing
float deltaTime_4_9_3 = 0.0f;
float lastFrame_4_9_3 = 0.0f;

int main_4_9_3() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_4_9_3, SCR_HEIGHT_4_9_3, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_4_9_3);
    glfwSetCursorPosCallback(window, mouse_callback_4_9_3);
    glfwSetScrollCallback(window, scroll_callback_4_9_3);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shader("../learnopengl/chapter_4/vshader_4_9_3_1.glsl",
                  "../learnopengl/chapter_4/fshader_4_9_3_1.glsl");
    Shader normalShader("../learnopengl/chapter_4/vshader_4_9_3_2.glsl",
                        "../learnopengl/chapter_4/fshader_4_9_3_2.glsl",
                        "../learnopengl/chapter_4/gshader_4_9_3_2.glsl");

    // load models
    // -----------
    Model nanosuit("../learnopengl/res/nanosuit/nanosuit.obj");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime_4_9_3 = currentFrame - lastFrame_4_9_3;
        lastFrame_4_9_3 = currentFrame;

        // input
        // -----
        processInput_4_9_3(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH_4_9_3 / (float)SCR_HEIGHT_4_9_3, 1.0f, 100.0f);
        glm::mat4 view = camera_4_9_3.GetViewMatrix();;
        glm::mat4 model = glm::mat4(1.0f);shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);

        // draw model as usual
        nanosuit.Draw(shader);

        // then draw model with normal visualizing geometry shader
        normalShader.use();
        normalShader.setMat4("projection", projection);
        normalShader.setMat4("view", view);
        normalShader.setMat4("model", model);

        nanosuit.Draw(normalShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput_4_9_3(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_4_9_3.ProcessKeyboard(FORWARD, deltaTime_4_9_3);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_4_9_3.ProcessKeyboard(BACKWARD, deltaTime_4_9_3);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_4_9_3.ProcessKeyboard(LEFT, deltaTime_4_9_3);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_4_9_3.ProcessKeyboard(RIGHT, deltaTime_4_9_3);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback_4_9_3(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback_4_9_3(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse_4_9_3) {
        lastX_4_9_3 = xpos;
        lastY_4_9_3 = ypos;
        firstMouse_4_9_3 = false;
    }

    float xoffset = xpos - lastX_4_9_3;
    float yoffset = lastY_4_9_3 - ypos; // reversed since y-coordinates go from bottom to top

    lastX_4_9_3 = xpos;
    lastY_4_9_3 = ypos;

    camera_4_9_3.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback_4_9_3(GLFWwindow *window, double xoffset, double yoffset) {
    camera_4_9_3.ProcessMouseScroll(yoffset);
}*/
