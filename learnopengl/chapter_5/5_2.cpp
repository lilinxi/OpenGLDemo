//
// Created by 李梦凡 on 2019/10/19.
// Gamma校正（空格开启），sRGB纹理（Q开启），双曲线衰减（E开启）
//

//1. Gamma2.2
//大多数监视器是阴极射线管显示器（CRT）。这些监视器有一个物理特性就是两倍的输入电压产生的不是两倍的亮度。输入电压产生约为输入电压的2.2次幂的亮度，这叫做监视器Gamma。
//2.2通常是是大多数显示设备的大概平均gamma值。基于gamma2.2的颜色空间叫做sRGB颜色空间。每个监视器的gamma曲线都有所不同，但是gamma2.2在大多数监视器上表现都不错。出于这个原因，游戏经常都会为玩家提供改变游戏gamma设置的选项，以适应每个监视器。

//2. Gamma校正示例
//暗红色(0.5,0.0,0.0)。在将颜色显示到监视器之前，我们先对颜色应用Gamma校正曲线。线性的颜色显示在监视器上相当于降低了2.2次幂的亮度，所以倒数就是1/2.2次幂。Gamma校正后的暗红色就会成为(0.5,0.0,0.0)1/2.2=(0.5,0.0,0.0)0.45=(0.73,0.0,0.0)。校正后的颜色接着被发送给监视器，最终显示出来的颜色是(0.73,0.0,0.0)2.2=(0.5,0.0,0.0)。你会发现使用了Gamma校正，监视器最终会显示出我们在应用中设置的那种线性的颜色。

//3. 在且只在最后执行Gamma校正
//gamma校正将把线性颜色空间转变为非线性空间，所以在最后一步进行gamma校正是极其重要的。如果你在最后输出之前就进行gamma校正，所有的后续操作都是在操作不正确的颜色值。例如，如果你使用多个帧缓冲，你可能打算让两个帧缓冲之间传递的中间结果仍然保持线性空间颜色，只是给发送给监视器的最后的那个帧缓冲应用gamma校正。

//4. 最简单实现方式（SRGB）
//sRGB最初设计的目的是作为生成在因特网以及万维网上浏览的图像的通用色彩空间，最后选择的是使用Gamma校准系数为2.2的色彩空间，即CRT显示器在这种情况下的平均线性电压响应。
//使用OpenGL内建的sRGB帧缓冲。
//glEnable(GL_FRAMEBUFFER_SRGB);

//5. 实现方式二
//自己在像素着色器中进行gamma校正（以下代码的实现方式）

//6. sRGB纹理
//因为不是所有纹理都是在sRGB空间中的所以当你把纹理指定为sRGB纹理时要格外小心。比如diffuse纹理，这种为物体上色的纹理几乎都是在sRGB空间中的。而为了获取光照参数的纹理，像specular贴图和法线贴图几乎都在线性空间中，所以如果你把它们也配置为sRGB纹理的话，光照就坏掉了。指定sRGB纹理时要当心。

//7. 衰减调整
//真实物理世界：float attenuation = 1.0 / (distance * distance);
//效果比较好的：float attenuation = 1.0 / distance; 因为经过了Gamma2.2
//Gamma校正后应使用：float attenuation = 1.0 / (distance * distance); 再矫正回来

//最优效果：空格，Q，E全开

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

void framebuffer_size_callback_5_2(GLFWwindow *window, int width, int height);

void mouse_callback_5_2(GLFWwindow *window, double xpos, double ypos);

void scroll_callback_5_2(GLFWwindow *window, double xoffset, double yoffset);

void processInput_5_2(GLFWwindow *window);

unsigned int loadTexture_5_2(const char *path, bool gammaCorrection);

// settings
const unsigned int SCR_WIDTH_5_2 = 1280;
const unsigned int SCR_HEIGHT_5_2 = 720;
bool gammaEnabled_5_2 = false;
bool gammaKeyPressed_5_2 = false;
bool textureGammaEnabled_5_2 = false;
bool textureGammaKeyPressed_5_2 = false;
bool atteEnabled_5_2 = false; // true:双曲线衰减，false:线性衰减
bool atteKeyPressed_5_2 = false;

// camera
Camera camera_5_2(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX_5_2 = (float) SCR_WIDTH_5_2 / 2.0;
float lastY_5_2 = (float) SCR_HEIGHT_5_2 / 2.0;
bool firstMouse_5_2 = true;

// timing
float deltaTime_5_2 = 0.0f;
float lastFrame_5_2 = 0.0f;

int main_5_2() {
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_5_2, SCR_HEIGHT_5_2, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_5_2);
    glfwSetCursorPosCallback(window, mouse_callback_5_2);
    glfwSetScrollCallback(window, scroll_callback_5_2);

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    // -------------------------
    Shader shader("../learnopengl/chapter_5/vshader_5_1.glsl",
                  "../learnopengl/chapter_5/fshader_5_2.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float planeVertices[] = {
            // positions            // normals         // texcoords
            10.0f, -0.5f, 10.0f, 0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
            -10.0f, -0.5f, 10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f,

            10.0f, -0.5f, 10.0f, 0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
            -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f,
            10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 10.0f, 10.0f
    };
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glBindVertexArray(0);

    // load textures
    // -------------
    unsigned int floorTexture = loadTexture_5_2("../learnopengl/res/wood.png", false);
    unsigned int floorTextureGammaCorrected = loadTexture_5_2("../learnopengl/res/wood.png", true);

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("floorTexture", 0);

    // lighting info
    // -------------
    glm::vec3 lightPositions[] = {
            glm::vec3(-3.0f, 0.0f, 0.0f),
            glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(3.0f, 0.0f, 0.0f)
    };
    glm::vec3 lightColors[] = {
            glm::vec3(0.25),
            glm::vec3(0.50),
            glm::vec3(0.75),
            glm::vec3(1.00)
    };

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime_5_2 = currentFrame - lastFrame_5_2;
        lastFrame_5_2 = currentFrame;

        // input
        // -----
        processInput_5_2(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw objects
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera_5_2.Zoom),
                                                (float) SCR_WIDTH_5_2 / (float) SCR_HEIGHT_5_2, 0.1f,
                                                100.0f);
        glm::mat4 view = camera_5_2.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // set light uniforms
        glUniform3fv(glGetUniformLocation(shader.ID, "lightPositions"), 4, &lightPositions[0][0]);
        glUniform3fv(glGetUniformLocation(shader.ID, "lightColors"), 4, &lightColors[0][0]);
        shader.setVec3("viewPos", camera_5_2.Position);
        shader.setInt("gamma", gammaEnabled_5_2);
        shader.setInt("atte", atteEnabled_5_2);
        // floor
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
//        纹理图像实在太亮了，发生这种情况是因为，它们实际上进行了两次gamma校正！想一想，当我们基于监视器上看到的情况创建一个图像，我们就已经对颜色值进行了gamma校正，所以再次显示在监视器上就没错。由于我们在渲染中又进行了一次gamma校正，图片就实在太亮了。
        if (!textureGammaEnabled_5_2) {
            glBindTexture(GL_TEXTURE_2D, floorTexture);
        } else {
            glBindTexture(GL_TEXTURE_2D, gammaEnabled_5_2 ? floorTextureGammaCorrected : floorTexture);
        }
        glDrawArrays(GL_TRIANGLES, 0, 6);

        std::cout << (gammaEnabled_5_2 ? "Gamma enabled" : "Gamma disabled") << std::endl;
        std::cout << (textureGammaEnabled_5_2 ? "textureGamma enabled" : "textureGamma disabled") << std::endl;
        std::cout << (atteEnabled_5_2 ? "atte enabled" : "atte disabled") << std::endl;

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput_5_2(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_5_2.ProcessKeyboard(FORWARD, deltaTime_5_2);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_5_2.ProcessKeyboard(BACKWARD, deltaTime_5_2);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_5_2.ProcessKeyboard(LEFT, deltaTime_5_2);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_5_2.ProcessKeyboard(RIGHT, deltaTime_5_2);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !gammaKeyPressed_5_2) {
        gammaEnabled_5_2 = !gammaEnabled_5_2;
        gammaKeyPressed_5_2 = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        gammaKeyPressed_5_2 = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !textureGammaKeyPressed_5_2) {
        textureGammaEnabled_5_2 = !textureGammaEnabled_5_2;
        textureGammaKeyPressed_5_2 = true;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
        textureGammaKeyPressed_5_2 = false;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !atteKeyPressed_5_2) {
        atteEnabled_5_2 = !atteEnabled_5_2;
        atteKeyPressed_5_2 = true;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
        atteKeyPressed_5_2 = false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback_5_2(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback_5_2(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse_5_2) {
        lastX_5_2 = xpos;
        lastY_5_2 = ypos;
        firstMouse_5_2 = false;
    }

    float xoffset = xpos - lastX_5_2;
    float yoffset = lastY_5_2 - ypos; // reversed since y-coordinates go from bottom to top

    lastX_5_2 = xpos;
    lastY_5_2 = ypos;

    camera_5_2.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback_5_2(GLFWwindow *window, double xoffset, double yoffset) {
    camera_5_2.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture_5_2(char const *path, bool gammaCorrection) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrComponents == 1) {
            internalFormat = dataFormat = GL_RED;
        } else if (nrComponents == 3) {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        } else if (nrComponents == 4) {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
//        sRGB纹理
//        因为不是所有纹理都是在sRGB空间中的所以当你把纹理指定为sRGB纹理时要格外小心。比如diffuse纹理，这种为物体上色的纹理几乎都是在sRGB空间中的。而为了获取光照参数的纹理，像specular贴图和法线贴图几乎都在线性空间中，所以如果你把它们也配置为sRGB纹理的话，光照就坏掉了。指定sRGB纹理时要当心。
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
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
