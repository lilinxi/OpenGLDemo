//
// Created by 李梦凡 on 2019/10/20.
// 文本渲染
//

//1. 字符字形的度量值
//每一个字形都放在一个水平的基准线(Baseline)上（即上图中水平箭头指示的那条线）。一些字形恰好位于基准线上（如’X’），而另一些则会稍微越过基准线以下（如’g’或’p’）（译注：即这些带有下伸部的字母，可以见这里）。这些度量值精确定义了摆放字形所需的每个字形距离基准线的偏移量，每个字形的大小，以及需要预留多少空间来渲染下一个字形。
//  width	    face->glyph->bitmap.width	位图宽度（像素）
//  height	    face->glyph->bitmap.rows	位图高度（像素）
//  bearingX	face->glyph->bitmap_left	水平距离，即位图相对于原点的水平位置（像素）
//  bearingY	face->glyph->bitmap_top	    垂直距离，即位图相对于基准线的垂直位置（像素）
//  advance	    face->glyph->advance.x	    水平预留值，即原点到下一个字形原点的水平距离（单位：1/64像素）

#include <glad.h>
#include <glfw3.h>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../utils/shader.h"

#include <iostream>
#include <map>
#include <string>

void framebuffer_size_callback_7_2(GLFWwindow *window, int width, int height);

void processInput_7_2(GLFWwindow *window);

void renderText_7_2(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

// settings
const unsigned int SCR_WIDTH_7_2 = 800;
const unsigned int SCR_HEIGHT_7_2 = 600;

/// Holds all state information relevant to a character as loaded using FreeType
//存储字体的一些度量值
struct Character_7_2 {
    GLuint TextureID;       // 字形纹理的ID
    glm::ivec2 Size;        // 字形大小
    glm::ivec2 Bearing;     // 从基准线到字形左部/顶部的偏移值
    GLuint Advance;         // 原点距下一个字形原点的距离
};

std::map<GLchar, Character_7_2> Characters_7_2;

unsigned int VAO_7_2, VBO_7_2;

int main_7_2() {
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH_7_2, SCR_HEIGHT_7_2, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_7_2);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

//    启用混合：字体的背景为透明的
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader("../learnopengl/chapter_7/vshader_7_2.glsl",
                  "../learnopengl/chapter_7/fshader_7_2.glsl");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH_7_2),
                                      0.0f, static_cast<GLfloat>(SCR_HEIGHT_7_2));
    shader.use();
    shader.setMat4("projection", projection);

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
//    初始化FreeType
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    // Load font as face
//    加载字体
    FT_Face face;
    if (FT_New_Face(ft, "../learnopengl/res/Arial.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    // Set size to load glyphs as
//    设置字体大小
//    此函数设置了字体面的宽度和高度，将宽度值设为0表示我们要从字体面通过给定的高度中动态计算出字形的宽度。
    FT_Set_Pixel_Sizes(face, 0, 48);

//    OpenGL要求所有的纹理都是4字节对齐的，即纹理的大小永远是4字节的倍数。通常这并不会出现什么问题，因为大部分纹理的宽度都为4的倍数并/或每像素使用4个字节，但是现在我们每个像素只用了一个字节，它可以是任意的宽度。通过将纹理解压对齐参数设为1，这样才能确保不会有对齐问题（它可能会造成段错误）。
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制

//    这里我们只生成ASCII字符集的前128个字符。对每一个字符，我们生成一个纹理并保存相关数据至Character结构体中，之后再添加至Characters这个映射表中。这样子，渲染一个字符所需的所有数据就都被储存下来备用了。
    for (GLubyte c = 0; c < 128; c++) {
        // 加载字符的字形
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // 生成纹理
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
//        这里将纹理的internalFormat和format设置为GL_RED。通过字形生成的位图是一个8位灰度图，它的每一个颜色都由一个字节来表示。
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        // 设置纹理选项
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 储存字符供之后使用
        Character_7_2 character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters_7_2.insert(std::pair<GLchar, Character_7_2>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // Destroy FreeType once we're finished
//    清理FreeType的资源。
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO_7_2);
    glGenBuffers(1, &VBO_7_2);
    glBindVertexArray(VAO_7_2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_7_2);
//    每个2D四边形需要6个顶点，每个顶点又是由一个4float向量（译注：一个纹理坐标和一个顶点坐标）组成，因此我们将VBO的内存分配为6 * 4个float的大小。由于我们会在绘制字符时经常更新VBO的内存，所以我们将内存类型设置为GL_DYNAMIC_DRAW。
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput_7_2(window);

        // render
        // ------
        glClearColor(0.2, 0.3, 0.3, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        renderText_7_2(shader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        renderText_7_2(shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback_7_2(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput_7_2(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void renderText_7_2(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
    // 激活对应的渲染状态
    s.use();
    s.setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_7_2);

    // 遍历文本中所有的字符
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character_7_2 ch = Characters_7_2[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
//        一些字符（如’p’或’q’）需要被渲染到基准线以下，因此字形四边形也应该被摆放在RenderText的y值以下。
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // 对每个字符更新VBO
        GLfloat vertices[6][4] = {
                {xpos,     ypos + h, 0.0, 0.0},
                {xpos,     ypos,     0.0, 1.0},
                {xpos + w, ypos,     1.0, 1.0},

                {xpos,     ypos + h, 0.0, 0.0},
                {xpos + w, ypos,     1.0, 1.0},
                {xpos + w, ypos + h, 1.0, 0.0}
        };
        // 在四边形上绘制字形纹理
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // 更新VBO内存的内容
        glBindBuffer(GL_ARRAY_BUFFER, VBO_7_2);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // 绘制四边形
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // 更新位置到下一个字形的原点，注意单位是1/64像素
        x += (ch.Advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}