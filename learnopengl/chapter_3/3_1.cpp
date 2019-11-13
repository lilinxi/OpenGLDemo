/*
//
// Created by 李梦凡 on 2019/10/15.
// 加载模型：开启/关闭线框
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

void framebuffer_size_callback_3_1(GLFWwindow *window, int width, int height);

void mouse_callback_3_1(GLFWwindow *window, double xpos, double ypos);

void scroll_callback_3_1(GLFWwindow *window, double xoffset, double yoffset);

void processInput_3_1(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH_3_1 = 800;
const unsigned int SCR_HEIGHT_3_1 = 600;

// camera
Camera camera_3_1(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX_3_1 = SCR_WIDTH_3_1 / 2.0f;
float lastY_3_1 = SCR_HEIGHT_3_1 / 2.0f;
bool firstMouse_3_1 = true;

// timing
float deltaTime_3_1 = 0.0f;
float lastFrame_3_1 = 0.0f;

int main() {
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_3_1, SCR_HEIGHT_3_1, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_3_1);
    glfwSetCursorPosCallback(window, mouse_callback_3_1);
    glfwSetScrollCallback(window, scroll_callback_3_1);

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
    Shader ourShader("../learnopengl/chapter_3/vshader_3_1.glsl", "../learnopengl/chapter_3/fshader_3_1.glsl");

    // load models
    // -----------
    Model ourModel("../learnopengl/res/nanosuit/nanosuit.obj");

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime_3_1 = currentFrame - lastFrame_3_1;
        lastFrame_3_1 = currentFrame;

        // input
        // -----
        processInput_3_1(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera_3_1.Zoom),
                                                (float) SCR_WIDTH_3_1 / (float) SCR_HEIGHT_3_1, 0.1f,
                                                100.0f);
        glm::mat4 view = camera_3_1.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,
                               glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));    // it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput_3_1(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_3_1.ProcessKeyboard(FORWARD, deltaTime_3_1);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_3_1.ProcessKeyboard(BACKWARD, deltaTime_3_1);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_3_1.ProcessKeyboard(LEFT, deltaTime_3_1);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_3_1.ProcessKeyboard(RIGHT, deltaTime_3_1);
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
//        开启线框模式
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
//        关闭线框模式
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback_3_1(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback_3_1(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse_3_1) {
        lastX_3_1 = xpos;
        lastY_3_1 = ypos;
        firstMouse_3_1 = false;
    }

    float xoffset = xpos - lastX_3_1;
    float yoffset = lastY_3_1 - ypos; // reversed since y-coordinates go from bottom to top

    lastX_3_1 = xpos;
    lastY_3_1 = ypos;

    camera_3_1.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback_3_1(GLFWwindow *window, double xoffset, double yoffset) {
    camera_3_1.ProcessMouseScroll(yoffset);
}
*/
