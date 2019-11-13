/*
//
// Created by 李梦凡 on 2019/10/15.
// 加载模型：开启/关闭线框+聚光/软化/衰减
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

void framebuffer_size_callback_3_2(GLFWwindow *window, int width, int height);

void mouse_callback_3_2(GLFWwindow *window, double xpos, double ypos);

void scroll_callback_3_2(GLFWwindow *window, double xoffset, double yoffset);

void processInput_3_2(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH_3_2 = 800;
const unsigned int SCR_HEIGHT_3_2 = 600;

// camera
Camera camera_3_2(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX_3_2 = SCR_WIDTH_3_2 / 2.0f;
float lastY_3_2 = SCR_HEIGHT_3_2 / 2.0f;
bool firstMouse_3_2 = true;

// timing
float deltaTime_3_2 = 0.0f;
float lastFrame_3_2 = 0.0f;

int main_3_2() {
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_3_2, SCR_HEIGHT_3_2, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_3_2);
    glfwSetCursorPosCallback(window, mouse_callback_3_2);
    glfwSetScrollCallback(window, scroll_callback_3_2);

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
    Shader ourShader("../learnopengl/chapter_3/vshader_3_2.glsl", "../learnopengl/chapter_3/fshader_3_2.glsl");

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
        deltaTime_3_2 = currentFrame - lastFrame_3_2;
        lastFrame_3_2 = currentFrame;

        // input
        // -----
        processInput_3_2(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();
        // be sure to activate shader when setting uniforms/drawing objects
        ourShader.use();
        ourShader.setVec3("light.position", camera_3_2.Position);
        ourShader.setVec3("light.direction", camera_3_2.Front);
        ourShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("viewPos", camera_3_2.Position);

        // light properties
        ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
//        光覆盖距离：50
        ourShader.setFloat("light.constant", 1.0f);
        ourShader.setFloat("light.linear", 0.09f);
        ourShader.setFloat("light.quadratic", 0.032f);

        // material properties
        ourShader.setFloat("shininess", 32.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera_3_2.Zoom), (float) SCR_WIDTH_3_2 / (float) SCR_HEIGHT_3_2, 0.1f,
                                                100.0f);
        glm::mat4 view = camera_3_2.GetViewMatrix();
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
void processInput_3_2(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_3_2.ProcessKeyboard(FORWARD, deltaTime_3_2);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_3_2.ProcessKeyboard(BACKWARD, deltaTime_3_2);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_3_2.ProcessKeyboard(LEFT, deltaTime_3_2);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_3_2.ProcessKeyboard(RIGHT, deltaTime_3_2);
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
void framebuffer_size_callback_3_2(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback_3_2(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse_3_2) {
        lastX_3_2 = xpos;
        lastY_3_2 = ypos;
        firstMouse_3_2 = false;
    }

    float xoffset = xpos - lastX_3_2;
    float yoffset = lastY_3_2 - ypos; // reversed since y-coordinates go from bottom to top

    lastX_3_2 = xpos;
    lastY_3_2 = ypos;

    camera_3_2.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback_3_2(GLFWwindow *window, double xoffset, double yoffset) {
    camera_3_2.ProcessMouseScroll(yoffset);
}*/
