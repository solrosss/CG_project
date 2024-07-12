# **OpenGL实践**

## 用OpenGL搭建并渲染一个场景

在这个选题中，我们使用OpenGL搭建并渲染了**一个太阳系的场景，包含太阳和八大行星**。为了支持这个场景，我们实现了以下功能：

- **模型文件加载**：使用Assimp库加载静态模型文件
- **动画模型加载**：使用Assimp库加载骨骼动画模型文件
- **天空盒背景**：使用立方体贴图实现天空盒背景
- **Blinn-Phong光照**：实现Blinn-Phong光照明模型
- **交互摄像机**：通过鼠标的拖动可以实现模型的旋转，鼠标滚轮实现拉近效果
- **实例化**：将大量重复的对象实例化，优化渲染性能

### 模型文件加载

使用Assimp库加载静态模型文件，首先下载Assimp库的源码，使用Cmake创建项目并编译，该功能的难点在于环境的配置，正确配置好环境后，Assimp库会将所有的模型数据加载至Assimp的通用数据结构中，我们只要解析这个通用的数据结构，就可以加载我们想要的模型了。

![image-20231209190232070](\demo\模型文件加载.png)

### 天空盒背景

使用立方体贴图实现天空盒背景。

天空盒就是一个包围摄像头的立方体，并在它的六个面上贴图，为了让天空盒正确成为“远景”，我们使用一些小技巧（令z分量等于w分量）让深度测试每次都认为天空盒的纹理是“更远的”。

![image-20231209165820892](demo\天空盒背景.png)

### 动画模型加载

使用Assimp库加载骨骼动画模型文件。

在正确加载模型的基础上，我们要额外实现以下功能以实现骨骼动画加载：在载入时保留骨骼信息以及关键帧；在两个关键帧之间插值，形成动画。

![image22](\demo\动画模型加载.gif)

### Blinn-Phong光照

实现Blinn-Phong光照明模型

与Assignment3中实现方法一样，在着色器中加入环境光照(Ambient Lighting)、漫反射光照(Diffuse Lighting)，由于我们搭建的场景是太阳系，镜面光照(Specular Lighting)占比成分会相对比较小。

其中，在这个场景中，显然太阳就是光源，但我们很难真的实现一个巨大的“球光源”，这里我们直接在太阳中心位置放置一个点光源，便可以实现光照效果了。在这里太阳的片段着色器中，我们是在获取了对应的纹理颜色后直接乘以一个光照强度。这样，将八大行星的光照效果以及太阳的“自发光”效果结合起来后，最后呈现的效果就和直接实现一个“球体光源”一样了。

![BP](\demo\BP光照.jpg)

### 交互摄像机

通过鼠标的拖动可以实现模型的旋转，鼠标滚轮实现拉近效果

通过glfwGetKey()函数捕捉键盘事件，实现摄像机的**上下左右前后移动和加减速**，鼠标操作同理使用glfwSetCursorPosCallback()和glfwSetScrollCallback()实现**转动视角和缩放**即可。

### 实例化

将大量重复的对象实例化，优化渲染性能

实例化就是将数据一次性发送给GPU，然后使用一个绘制函数让OpenGL利用这些数据绘制多个物体，使用实例化数组实现土星的环。

我们为每个碎石生成一个变换矩阵，用作它们的模型矩阵。变换矩阵将碎石位移到小行星带中的某处，接下来，我们应用一个随机的缩放，并且以一个旋转向量为轴进行一个随机的旋转。最终的变换矩阵不仅能将这些碎石变换到行星的周围，而且会让它看起来更自然。这里我们同时创造了一万颗碎石并将其散布在土星的周围，创造出了一个土星环。

![instance](\demo\实例化.jpg)

## 最终效果

使用上述功能我们搭建并渲染了一个太阳系！

我们的太阳系包括了太阳和八大行星，当然还有月亮，其中我们对他们的距离的大小做了一点艺术性的加工，让我们能够“一览众星小”。我们还模拟了他们的公转（当然公转周期也加工过），其中月球还会绕地球公转。

他们看起来是下面这样的，最中心是太阳，从近到远分别是水星、金星、地球-月亮、火星、木星、土星、天王星、海王星。

![final](\demo\Final.jpg)

## 附录

- /code下存放项目相关代码
- /release下存放可执行文件`openGL.exe`、相关资源文件以及相关dll文件。
- /demo下存放完成项目过程中的部分结果截图及录屏，最终效果见`Final.jpg`和`FinalScene.mp4`。（**注意**：由于时间关系，这些demo的分辨率可能不同，但都是**4:3**，`/release/openGL.exe`运行得到的窗口为**1200X900**）

