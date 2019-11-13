//
// Created by 李梦凡 on 2019/10/20.
// 视差贴图：砖墙（Q/E 调整高度）
//

//1. 视差贴图
//  1、 视差贴图属于位移贴图(Displacement Mapping)技术的一种，它对根据储存在纹理中的几何信息对顶点进行位移或偏移。一种实现的方式是比如有1000个顶点，根据纹理中的数据对平面特定区域的顶点的高度进行位移。
//      置换顶点有一个问题就是平面必须由很多顶点组成才能获得具有真实感的效果，否则看起来效果并不会很好。
//  2、 视差贴图背后的思想是修改纹理坐标使一个fragment的表面看起来比实际的更高或者更低，所有这些都根据观察方向和高度贴图。
//      视差贴图的目的是，在A位置上的fragment不再使用点A的纹理坐标而是使用点B的。随后我们用点B的纹理坐标采样，观察者就像看到了点B一样。
//      视差贴图背后的思想是修改纹理坐标使一个fragment的表面看起来比实际的更高或者更低，所有这些都根据观察方向和高度贴图。
//  3、 还是在切线空间中实现视差贴图。

//2. 视差贴图+法线贴图
//因为视差贴图生成表面位移了的幻觉，当光照不匹配时这种幻觉就被破坏了。法线贴图通常根据高度贴图生成，法线贴图和高度贴图一起用能保证光照能和位移想匹配。
//使用反色高度贴图（也叫深度贴图）去模拟深度比使用高度贴图模拟高度更容易。

#include <glad.h>
#include <glfw3.h>

//Warn has defined in 1_4_1
//#define STB_IMAGE_IMPLEMENTATION

#include "../utils/stb_image.h"

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

#include "../utils/shader.h"
#include "../utils/camera.h"

#include <iostream>
#include <vector>

void framebuffer_size_callback_5_5_1(GLFWwindow *window, int width, int height);

void mouse_callback_5_5_1(GLFWwindow *window, double xpos, double ypos);

void scroll_callback_5_5_1(GLFWwindow *window, double xoffset, double yoffset);

void processInput_5_5_1(GLFWwindow *window);

unsigned int loadTexture_5_5_1(const char *path);

void renderQuad_5_5_1();

// settings
const unsigned int SCR_WIDTH_5_5_1 = 1280;
const unsigned int SCR_HEIGHT_5_5_1 = 720;
float heightScale_5_5_1 = 0.1;

// camera
Camera camera_5_5_1(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX_5_5_1 = (float) SCR_WIDTH_5_5_1 / 2.0;
float lastY_5_5_1 = (float) SCR_HEIGHT_5_5_1 / 2.0;
bool firstMouse_5_5_1 = true;

// timing
float deltaTime_5_5_1 = 0.0f;
float lastFrame_5_5_1 = 0.0f;

int main_5_5_1() {
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_5_5_1, SCR_HEIGHT_5_5_1, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_5_5_1);
    glfwSetCursorPosCallback(window, mouse_callback_5_5_1);
    glfwSetScrollCallback(window, scroll_callback_5_5_1);

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
    Shader shader("../learnopengl/chapter_5/vshader_5_4.glsl",
                  "../learnopengl/chapter_5/fshader_5_5_1.glsl");

    // load textures
    // -------------
    unsigned int diffuseMap = loadTexture_5_5_1("../learnopengl/res/bricks2.jpg");
    unsigned int normalMap = loadTexture_5_5_1("../learnopengl/res/bricks2_normal.jpg");
    unsigned int heightMap = loadTexture_5_5_1("../learnopengl/res/bricks2_disp.jpg");
//    unsigned int diffuseMap = loadTexture_5_5_1("../learnopengl/res/toy_box_diffuse.png");
//    unsigned int normalMap = loadTexture_5_5_1("../learnopengl/res/toy_box_normal.png");
//    unsigned int heightMap = loadTexture_5_5_1("../learnopengl/res/toy_box_disp.png");

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("diffuseMap", 0);
    shader.setInt("normalMap", 1);
    shader.setInt("depthMap", 2);

    // lighting info
    // -------------
    glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime_5_5_1 = currentFrame - lastFrame_5_5_1;
        lastFrame_5_5_1 = currentFrame;

        // input
        // -----
        processInput_5_5_1(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure view/projection matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera_5_5_1.Zoom),
                                                (float) SCR_WIDTH_5_5_1 / (float) SCR_HEIGHT_5_5_1, 0.1f,
                                                100.0f);
        glm::mat4 view = camera_5_5_1.GetViewMatrix();
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // render parallax-mapped quad
        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::rotate(model, glm::radians((float) glfwGetTime() * -10.0f), glm::normalize(
//                glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show parallax mapping from multiple directions
        shader.setMat4("model", model);
        shader.setVec3("viewPos", camera_5_5_1.Position);
        shader.setVec3("lightPos", lightPos);
        shader.setFloat("heightScale", heightScale_5_5_1); // adjust with Q and E keys
        std::cout << heightScale_5_5_1 << std::endl;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, heightMap);
        renderQuad_5_5_1();

        // render light source (simply re-renders a smaller plane at the light's position for debugging/visualization)
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        shader.setMat4("model", model);
        renderQuad_5_5_1();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// renders a 1x1 quad in NDC with manually calculated tangent vectors
// ------------------------------------------------------------------
unsigned int quadVAO_5_5_1 = 0;
unsigned int quadVBO_5_5_1;

void renderQuad_5_5_1() {
    if (quadVAO_5_5_1 == 0) {
        // positions
        glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3(1.0f, -1.0f, 0.0f);
        glm::vec3 pos4(1.0f, 1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);


        float quadVertices[] = {
                // positions            // normal         // texcoords  // tangent                          // bitangent
                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z,
                bitangent1.x, bitangent1.y, bitangent1.z,
                pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z,
                bitangent1.x, bitangent1.y, bitangent1.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z,
                bitangent1.x, bitangent1.y, bitangent1.z,

                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z,
                bitangent2.x, bitangent2.y, bitangent2.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z,
                bitangent2.x, bitangent2.y, bitangent2.z,
                pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z,
                bitangent2.x, bitangent2.y, bitangent2.z
        };
        // configure plane VAO
        glGenVertexArrays(1, &quadVAO_5_5_1);
        glGenBuffers(1, &quadVBO_5_5_1);
        glBindVertexArray(quadVAO_5_5_1);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO_5_5_1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (11 * sizeof(float)));
    }
    glBindVertexArray(quadVAO_5_5_1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput_5_5_1(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_5_5_1.ProcessKeyboard(FORWARD, deltaTime_5_5_1);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_5_5_1.ProcessKeyboard(BACKWARD, deltaTime_5_5_1);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_5_5_1.ProcessKeyboard(LEFT, deltaTime_5_5_1);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_5_5_1.ProcessKeyboard(RIGHT, deltaTime_5_5_1);
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (heightScale_5_5_1 > 0.0f) {
            heightScale_5_5_1 -= 0.0005f;
        } else {
            heightScale_5_5_1 = 0.0f;
        }
    } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (heightScale_5_5_1 < 1.0f) {
            heightScale_5_5_1 += 0.0005f;
        } else {
            heightScale_5_5_1 = 1.0f;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback_5_5_1(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback_5_5_1(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse_5_5_1) {
        lastX_5_5_1 = xpos;
        lastY_5_5_1 = ypos;
        firstMouse_5_5_1 = false;
    }

    float xoffset = xpos - lastX_5_5_1;
    float yoffset = lastY_5_5_1 - ypos; // reversed since y-coordinates go from bottom to top

    lastX_5_5_1 = xpos;
    lastY_5_5_1 = ypos;

    camera_5_5_1.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback_5_5_1(GLFWwindow *window, double xoffset, double yoffset) {
    camera_5_5_1.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture_5_5_1(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1) {
            format = GL_RED;
        } else if (nrComponents == 3) {
            format = GL_RGB;
        } else if (nrComponents == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}