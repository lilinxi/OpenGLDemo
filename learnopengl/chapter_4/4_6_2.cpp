//
// Created by 李梦凡 on 2019/10/17.
// 立方体贴图：天空盒（优化）
//

//目前我们是首先渲染天空盒，之后再渲染场景中的其它物体。这样子能够工作，但不是非常高效。如果我们先渲染天空盒，我们就会对屏幕上的每一个像素运行一遍片段着色器，即便只有一小部分的天空盒最终是可见的。可以使用提前深度测试(Early Depth Testing)轻松丢弃掉的片段能够节省我们很多宝贵的带宽。
//    1、 透视除法是在顶点着色器运行之后执行的，将gl_Position的xyz坐标除以w分量。我们又从深度测试小节中知道，相除结果的z分量等于顶点的深度值。使用这些信息，我们可以将输出位置的z分量等于它的w分量，让z分量永远等于1.0，这样子的话，当透视除法执行之后，z分量会变为w / w = 1.0。
//      最终的标准化设备坐标将永远会有一个等于1.0的z值：最大的深度值。结果就是天空盒只会在没有可见物体的地方渲染了（只有这样才能通过深度测试，其它所有的东西都在天空盒前面）。
//    2、 我们还要改变一下深度函数，将它从默认的GL_LESS改为GL_LEQUAL。深度缓冲将会填充上天空盒的1.0值，所以我们需要保证天空盒在值小于或等于深度缓冲而不是小于时通过深度测试。

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

using namespace std;

void framebuffer_size_callback_4_6_2(GLFWwindow *window, int width, int height);

void mouse_callback_4_6_2(GLFWwindow *window, double xpos, double ypos);

void scroll_callback_4_6_2(GLFWwindow *window, double xoffset, double yoffset);

void processInput_4_6_2(GLFWwindow *window);

unsigned int loadTexture_4_6_2(const char *path);

unsigned int loadCubemap_4_6_2(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH_4_6_2 = 800;
const unsigned int SCR_HEIGHT_4_6_2 = 600;

// camera
Camera camera_4_6_2(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX_4_6_2 = SCR_WIDTH_4_6_2 / 2.0f;
float lastY_4_6_2 = SCR_HEIGHT_4_6_2 / 2.0f;
bool firstMouse_4_6_2 = true;

// timing
float deltaTime_4_6_2 = 0.0f;    // time between current frame and last frame
float lastFrame_4_6_2 = 0.0f;

int main_4_6_2() {
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_4_6_2, SCR_HEIGHT_4_6_2, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_4_6_2);
    glfwSetCursorPosCallback(window, mouse_callback_4_6_2);
    glfwSetScrollCallback(window, scroll_callback_4_6_2);

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

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("../learnopengl/chapter_4/vshader_4_1_1.glsl", "../learnopengl/chapter_4/fshader_4_1_1.glsl");
    Shader skyboxShader("../learnopengl/chapter_4/vshader_4_6_2.glsl", "../learnopengl/chapter_4/fshader_4_6_1.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
            // Back face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
            // Front face
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f, // top-right
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, // top-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
            // Left face
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
            // Right face
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-left
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // top-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
            // Top face
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // bottom-right
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f  // bottom-left
    };
    float skyboxVertices[] = {
            // positions
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
    };

    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

    // load and create a texture
    // -------------------------
    unsigned int containerTexture = loadTexture_4_6_2("../learnopengl/res/container.jpg");
    vector<std::string> faces{
            "../learnopengl/res/skybox/right.jpg",
            "../learnopengl/res/skybox/left.jpg",
            "../learnopengl/res/skybox/top.jpg",
            "../learnopengl/res/skybox/bottom.jpg",
            "../learnopengl/res/skybox/front.jpg",
            "../learnopengl/res/skybox/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap_4_6_2(faces);

    // shader configuration
    // --------------------
    ourShader.use();
    ourShader.setInt("ourTexture", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime_4_6_2 = currentFrame - lastFrame_4_6_2;
        lastFrame_4_6_2 = currentFrame;

        // input
        // -----
        processInput_4_6_2(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw scene as normal
        // activate shader
        ourShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera_4_6_2.Zoom),
                                                (float) SCR_WIDTH_4_6_2 / (float) SCR_HEIGHT_4_6_2, 0.1f,
                                                100.0f);
        ourShader.setMat4("projection", projection);
        glm::mat4 view = camera_4_6_2.GetViewMatrix();
        ourShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        ourShader.setMat4("model", model);
        // cubes
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, containerTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw skybox as last
//        将它从默认的GL_LESS改为GL_LEQUAL。深度缓冲将会填充上天空盒的1.0值，所以我们需要保证天空盒在值小于或等于深度缓冲而不是小于时通过深度测试。
        glDepthFunc(
                GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera_4_6_2.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &skyboxVAO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput_4_6_2(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_4_6_2.ProcessKeyboard(FORWARD, deltaTime_4_6_2);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_4_6_2.ProcessKeyboard(BACKWARD, deltaTime_4_6_2);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_4_6_2.ProcessKeyboard(LEFT, deltaTime_4_6_2);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_4_6_2.ProcessKeyboard(RIGHT, deltaTime_4_6_2);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback_4_6_2(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback_4_6_2(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse_4_6_2) {
        lastX_4_6_2 = xpos;
        lastY_4_6_2 = ypos;
        firstMouse_4_6_2 = false;
    }

    float xoffset = xpos - lastX_4_6_2;
    float yoffset = lastY_4_6_2 - ypos; // reversed since y-coordinates go from bottom to top

    lastX_4_6_2 = xpos;
    lastY_4_6_2 = ypos;

    camera_4_6_2.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback_4_6_2(GLFWwindow *window, double xoffset, double yoffset) {
    camera_4_6_2.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture_4_6_2(char const *path) {
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

unsigned int loadCubemap_4_6_2(vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
//    绑定到GL_TEXTURE_CUBE_MAP
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
//            GL_TEXTURE_CUBE_MAP_POSITIVE_X	右
//            GL_TEXTURE_CUBE_MAP_NEGATIVE_X	左
//            GL_TEXTURE_CUBE_MAP_POSITIVE_Y	上
//            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	下
//            GL_TEXTURE_CUBE_MAP_POSITIVE_Z	后
//            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	前
//            可以从GL_TEXTURE_CUBE_MAP_POSITIVE_X开始遍历它们，在每个迭代中对枚举值加1，遍历了整个纹理目标。
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    三个维度：STR
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}