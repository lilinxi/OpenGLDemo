//
// Created by 李梦凡 on 2019/10/17.
// GLSL 的内建变量
//

//1. 顶点着色器变量
//gl_Position：顶点着色器的裁剪空间输出位置向量。
//gl_PointSize(float)：设置点的宽高（像素）。
//gl_VertexID：正在绘制顶点的当前ID。（当（使用glDrawElements）进行索引渲染的时候，这个变量会存储正在绘制顶点的当前索引。当（使用glDrawArrays）不使用索引进行绘制的时候，这个变量会储存从渲染调用开始的已处理顶点数量。）
//
//2. 片段着色器变量
//gl_FragCoord：x，y 为屏幕边界，z 为实际深度值。
//gl_FrontFacing(bool)：判断正向面和背向面
//gl_FragCoord：设置深度值。（如果着色器没有写入值到gl_FragDepth，它会自动取用gl_FragCoord.z的值。）
//    默认会导致提前深度检测禁用。
//    在片段着色器的顶部使用深度条件(Depth Condition)重新声明gl_FragDepth变量：（这个特性只在OpenGL 4.2版本或以上才提供。）
//        layout (depth_<condition>) out float gl_FragDepth;
//        any	        默认值。提前深度测试是禁用的，你会损失很多性能
//        greater	    你只能让深度值比gl_FragCoord.z更大
//        less	        你只能让深度值比gl_FragCoord.z更小
//        unchanged	    如果你要写入gl_FragDepth，你将只能写入gl_FragCoord.z的值