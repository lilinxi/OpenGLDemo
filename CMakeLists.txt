#1. https://tanwenbo.top/course/mac-shi-yongclion-pei-zhiopenglglewgladglfw.html
#brew install glew
#brew install glfw
#glad：http://glad.dav1d.de/
#add to include and CMake
#
#2. https://blog.csdn.net/feng_ye2015/article/details/86598071
##include <KHR/khrplatform.h>改为#include "../KHR/khrplatform.h"
#
#3. glad和glew：http://blog.shenyuanluo.com/GLGenbuffers.html
#GLAD 和 GLEW 的作用都是帮助在开发 OpenGL 时简化其在 运行时 获取函数的内存地址并将其保存在一个函数指针中供以使用的流程。
#
#4. glfw：GLFW是一个专门针对OpenGL的C语言库，它提供了一些渲染物体所需的最低限度的接口。它允许用户创建OpenGL上下文，定义窗口参数以及处理用户输入，这正是我们需要的。
#
#5. 关系
#glad<->glew:=gl+glu
#glfw取代glut（工具库：窗口+IO+工具）
#
#6. glm
#brew install glm
#https://glm.g-truc.net/0.9.8/index.html
#
#7. stb_image
#https://github.com/nothings/stb

cmake_minimum_required(VERSION 3.14)
project(OpenGLDemo)

set(CMAKE_CXX_STANDARD 14)

# 添加头文件
set(GLEW_H /usr/local/Cellar/glew/2.1.0/include/GL)
set(GLFW_H /usr/local/Cellar/glfw/3.3.2/include/GLFW)
set(GLAD_H /usr/local/include/glad)
#include freetype
set(FREETYPE_H /usr/local/Cellar/freetype/2.10.1/include/freetype2)
include_directories(${GLEW_H} ${GLFW_H} ${GLAD_H} ${FREETYPE_H})

# 添加目标链接
set(GLEW_LINK /usr/local/Cellar/glew/2.1.0/lib/libGLEW.2.1.dylib)
set(GLFW_LINK /usr/local/Cellar/glfw/3.3.2/lib/libglfw.3.dylib)
# method 1(assimp): brew install assimp
set(ASSIMP_LINK /usr/local/Cellar/assimp/4.1.0/lib/libassimp.4.1.0.dylib)
#brew install freetype
set(FREETYPE_LINK /usr/local/Cellar/freetype/2.10.1/lib/libfreetype.dylib)
link_libraries(${OPENGL} ${GLEW_LINK} ${GLFW_LINK} ${ASSIMP_LINK} ${FREETYPE_LINK})

# 执行编译命令
set(SOURCE_FILES "src/glad.c" main.cpp learnopengl/chapter_1/1_1.cpp learnopengl/chapter_1/1_2_1.cpp learnopengl/chapter_1/1_2_1_d.cpp learnopengl/chapter_1/1_2_2.cpp learnopengl/chapter_1/1_3_1.cpp learnopengl/chapter_1/1_3_2.cpp learnopengl/chapter_1/1_3_3.cpp learnopengl/utils/shader.h learnopengl/chapter_1/1_3_4.cpp learnopengl/utils/stb_image.h learnopengl/chapter_1/1_4_1.cpp learnopengl/chapter_1/1_4_2.cpp learnopengl/chapter_1/1_4_3.cpp learnopengl/chapter_1/1_5_1.cpp learnopengl/chapter_1/1_5_2.cpp learnopengl/chapter_1/1_5_3.cpp learnopengl/chapter_1/1_6_1.cpp learnopengl/chapter_1/1_6_2.cpp learnopengl/chapter_1/1_6_3.cpp learnopengl/chapter_1/1_7_1.cpp learnopengl/chapter_1/1_7_2.cpp learnopengl/chapter_1/1_7_3.cpp learnopengl/utils/camera.h learnopengl/chapter_1/1_7_4.cpp learnopengl/chapter_2/2_1.cpp learnopengl/chapter_2/2_2_1.cpp learnopengl/chapter_2/2_2_2.cpp learnopengl/chapter_2/2_3.cpp learnopengl/chapter_2/2_4_1.cpp learnopengl/chapter_2/2_4_2.cpp learnopengl/chapter_2/2_5_1.cpp learnopengl/chapter_2/2_5_2.cpp learnopengl/chapter_2/2_5_3.cpp learnopengl/chapter_2/2_5_4.cpp learnopengl/chapter_2/2_6.cpp learnopengl/utils/mesh.h learnopengl/utils/model.h learnopengl/chapter_3/3_1.cpp learnopengl/chapter_3/3_2.cpp learnopengl/chapter_4/4_1_1.cpp learnopengl/chapter_4/4_1_2.cpp learnopengl/chapter_4/4_1_3.cpp learnopengl/chapter_4/4_2.cpp learnopengl/chapter_4/4_3_1.cpp learnopengl/chapter_4/4_3_2.cpp learnopengl/chapter_4/4_4.cpp learnopengl/chapter_4/4_5_1.cpp learnopengl/chapter_4/4_5_2.cpp learnopengl/chapter_4/4_6_1.cpp learnopengl/chapter_4/4_6_2.cpp learnopengl/chapter_4/4_6_3.cpp learnopengl/chapter_4/4_6_4.cpp learnopengl/chapter_4/4_6_5.cpp learnopengl/chapter_4/4_6_6.cpp learnopengl/chapter_4/4_7.cpp learnopengl/chapter_4/4_8_1.cpp learnopengl/chapter_4/4_8_2.cpp learnopengl/chapter_4/4_8_3.cpp learnopengl/chapter_4/4_9_1.cpp learnopengl/chapter_4/4_9_2.cpp learnopengl/chapter_4/4_9_3.cpp learnopengl/chapter_4/4_10_1.cpp learnopengl/chapter_4/4_10_2.cpp learnopengl/chapter_4/4_11_1.cpp learnopengl/chapter_4/4_11_2.cpp learnopengl/chapter_5/5_1.cpp learnopengl/utils/filesystem.h learnopengl/chapter_5/5_2.cpp learnopengl/chapter_5/5_3_1.cpp learnopengl/chapter_5/5_3_2.cpp learnopengl/chapter_5/5_3_3.cpp learnopengl/chapter_5/5_3_4.cpp learnopengl/chapter_5/5_3_5.cpp learnopengl/chapter_5/5_4.cpp learnopengl/chapter_5/5_5_1.cpp learnopengl/chapter_5/5_5_2.cpp learnopengl/chapter_5/5_5_3.cpp learnopengl/chapter_5/5_6.cpp learnopengl/chapter_5/5_7.cpp learnopengl/chapter_5/5_8.cpp learnopengl/chapter_5/5_9.cpp learnopengl/chapter_6/6_1.cpp learnopengl/chapter_7/7_1.cpp learnopengl/chapter_7/7_2.cpp)
add_executable(OpenGLDemo ${SOURCE_FILES})
if (APPLE)
    target_link_libraries(OpenGLDemo "-framework OpenGL") # 直接渲染模式
    #    target_link_libraries(OpenGLDemo "-framework GLUT") # glut
    #    method 2(assimp)
    #    https://github.com/assimp/assimp/releases/tag/v3.3.1
    #    error assimp-3.3.1/code/D3MFImporter.cpp:230
    #    1. cmake cmake GUI https://cmake.org/download/ configure, configure, generate
    #    2. make
    #    3. link
    #    target_link_libraries(OpenGLDemo "path/libassimp.dylib")
endif ()