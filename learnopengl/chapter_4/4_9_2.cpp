/*
//
// Created by 李梦凡 on 2019/10/18.
// 爆破物体
//

#include <glad.h>
#include <glfw3.h>

#include <../glm/glm.hpp>
#include <../glm/gtc/matrix_transform.hpp>

#include "../utils/shader.h"
#include "../utils/camera.h"

//Warn 只能include一个model
#include "../utils/model.h"

#include <iostream>

void framebuffer_size_callback_4_9_2(GLFWwindow *window, int width, int height);

void mouse_callback_4_9_2(GLFWwindow *window, double xpos, double ypos);

void scroll_callback_4_9_2(GLFWwindow *window, double xoffset, double yoffset);

void processInput_4_9_2(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH_4_9_2 = 1280;
const unsigned int SCR_HEIGHT_4_9_2 = 720;

// camera
Camera camera_4_9_2(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX_4_9_2 = (float) SCR_WIDTH_4_9_2 / 2.0;
float lastY_4_9_2 = (float) SCR_HEIGHT_4_9_2 / 2.0;
bool firstMouse_4_9_2 = true;

// timing
float deltaTime_4_9_2 = 0.0f;
float lastFrame_4_9_2 = 0.0f;

int main_4_9_2() {
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_4_9_2, SCR_HEIGHT_4_9_2, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_4_9_2);
    glfwSetCursorPosCallback(window, mouse_callback_4_9_2);
    glfwSetScrollCallback(window, scroll_callback_4_9_2);

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
    Shader shader("../learnopengl/chapter_4/vshader_4_9_2.glsl",
                  "../learnopengl/chapter_4/fshader_4_9_2.glsl",
                  "../learnopengl/chapter_4/gshader_4_9_2.glsl");

    // load models
    // -----------
    Model nanosuit("../learnopengl/res/nanosuit/nanosuit.obj");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime_4_9_2 = currentFrame - lastFrame_4_9_2;
        lastFrame_4_9_2 = currentFrame;

        // input
        // -----
        processInput_4_9_2(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera_4_9_2.Zoom),
                                                (float) SCR_WIDTH_4_9_2 / (float) SCR_HEIGHT_4_9_2, 0.1f, 100.0f);
        glm::mat4 view = camera_4_9_2.GetViewMatrix();;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,
                               glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); // it's a bit too big for our scene, so scale it down
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);

        // add time component to geometry shader in the form of a uniform
        shader.setFloat("time", glfwGetTime());

        // draw model
        nanosuit.Draw(shader);

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
void processInput_4_9_2(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_4_9_2.ProcessKeyboard(FORWARD, deltaTime_4_9_2);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_4_9_2.ProcessKeyboard(BACKWARD, deltaTime_4_9_2);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_4_9_2.ProcessKeyboard(LEFT, deltaTime_4_9_2);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_4_9_2.ProcessKeyboard(RIGHT, deltaTime_4_9_2);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback_4_9_2(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback_4_9_2(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse_4_9_2) {
        lastX_4_9_2 = xpos;
        lastY_4_9_2 = ypos;
        firstMouse_4_9_2 = false;
    }

    float xoffset = xpos - lastX_4_9_2;
    float yoffset = lastY_4_9_2 - ypos; // reversed since y-coordinates go from bottom to top

    lastX_4_9_2 = xpos;
    lastY_4_9_2 = ypos;

    camera_4_9_2.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback_4_9_2(GLFWwindow *window, double xoffset, double yoffset) {
    camera_4_9_2.ProcessMouseScroll(yoffset);
}*/
