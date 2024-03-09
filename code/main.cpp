#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_s.h>
#include <camera.h>
#include <animator.h>
#include <model.h>
#include <ani_model.h>

#include <math.h>

#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadCubemap(vector<std::string> faces);

//setting
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

double Distance[9] = { 0 ,0.649,1.219,1.885,2.911,4.981,9.455,15.0,18.0 };
double Location[9][3]; // 实际位置
double Radius[9] = { 100000,2440*3,6050*3,6478*3,3392*3,72984,60536,25118,24528 }; 
double T[9] = { 0, 0.25, 0.67, 1, 1.8, 5, 10, 12, 14 };
double Angles[9] = { 0 };
double Angle= acos(-1) / 3; 

// camera
Camera camera(glm::vec3(50.0f, 3.0f, 50.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwSetWindowTitle(window,"Group 14");

    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    Shader skyboxShader("skybox.vs", "skybox.fs");
    //Shader aniShader("anishader.vs", "anishader.fs");
    Shader ourShader("shader.vs", "shader.fs");
    Shader instShader("instance.vs", "anishader.fs");
    Shader solarShader("shader.vs", "solarshader.fs");

    // load models
    //Model Model1("model/nanosuit/nanosuit.obj");
    //Model Model2("model/diablo3_pose/diablo3_pose.obj");
    Model planet("model/planet/planet/planet.obj");
    Model Solar("model/planet/Solar/Solar.obj");
    Model Mercury("model/planet/Mercury/Mercury.obj");
    Model Venus("model/planet/Venus/Venus.obj");
    Model Earth("model/planet/Earth/Earth.obj");
    Model Moon("model/planet/Moon/Moon.obj");
    Model Mars("model/planet/Mars/Mars.obj");
    Model Jupiter("model/planet/Jupiter/Jupiter.obj");
    Model Saturn("model/planet/Saturn/Saturn.obj");
    Model Uranus("model/planet/Uranus/Uranus.obj");
    Model Neptune("model/planet/Neptune/Neptune.obj");
    Model rock("model/planet/rock/rock.obj");
    
    //计算位置
    for (int i = 0; i < 9; ++i) {
        Distance[i] = Distance[i] * 1500000 / Radius[5];
    }
    for (int i = 0; i < 9; ++i) {
        double a = double((rand() % 100) / 100.0) * Angle;
        Angles[i] = a;
        Location[i][0] = Distance[i] * cos(a);
        Location[i][1] = (double((rand() % 100) / 100.0) * 2 - 1) * 1.0f;
        Location[i][2] = Distance[i] * sin(a);
    }
    double moon_angle = 0;

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    vector<std::string> faces
    {
        /*"skybox/right.jpg",
        "skybox/left.jpg",
        "skybox/top.jpg",
        "skybox/bottom.jpg",
        "skybox/front.jpg",
        "skybox/back.jpg"*/
        "skybox/Stellarium/right.png",
        "skybox/Stellarium/left.png",
        "skybox/Stellarium/top.png",
        "skybox/Stellarium/bottom.png",
        "skybox/Stellarium/front.png",
        "skybox/Stellarium/back.png"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    
    // generate a large list of semi-random model transformation matrices
    // ------------------------------------------------------------------
    unsigned int amount = 10000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    float radius = 5 * Radius[6] / Radius[7];
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.2f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(Location[6][0], Location[6][1], Location[6][2]));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
        model = glm::translate(model, glm::vec3(x, y, z));
        

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale*0.3f));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    // configure instanced array
    // -------------------------
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    // set transformation matrices as an instance vertex attribute (with divisor 1)
    // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
    // normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
    // -----------------------------------------------------------------------------------------------------------------------------------
    for (unsigned int i = 0; i < rock.meshes.size(); i++)
    {
        unsigned int VAO = rock.meshes[i].VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    //load animodel
    /*stbi_set_flip_vertically_on_load(true);
    aniModel aModel("model/dance/dancing_vampire.dae");
    Animation danceAnimation("model/dance/dancing_vampire.dae", &aModel);
    Animator animator(&danceAnimation);*/

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //输入
        processInput(window);
        //animator.UpdateAnimation(deltaTime);

        //渲染指令
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //静态
        ourShader.use();
        ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("lightPos", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("viewPos", camera.Position);
        
        //更新位置
        for (int i = 0; i < 9; ++i) {
            if (i == 6) continue;
            double a = 0.00003f * 2 * acos(-1) / T[i];
            Angles[i] += a;
            Location[i][0] = Distance[i] * cos(Angles[i]);
            Location[i][2] = Distance[i] * sin(Angles[i]);
        }

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        ourShader.setMat4("projection", projection);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        
        //model1
        /*model = glm::translate(model, glm::vec3(-5.0f, -10.0f, -12.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        Model1.Draw(ourShader);*/
        //model2
        /*model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.0f, -7.0f, -12.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        Model2.Draw(ourShader);*/
        //planet
        /*model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 7.0f, -12.0f)); 
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	
        ourShader.setMat4("model", model);
        planet.Draw(ourShader);*/
        //mercury
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(Location[1][0], Location[1][1], Location[1][2]));
        model = glm::scale(model, glm::vec3(Radius[1]/ Radius[7]));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        Mercury.Draw(ourShader);
        //venus
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(Location[2][0], Location[2][1], Location[2][2]));
        model = glm::scale(model, glm::vec3(Radius[2] / Radius[7]));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        Venus.Draw(ourShader);
        //earth
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(Location[3][0], Location[3][1], Location[3][2]));
        model = glm::scale(model, glm::vec3(Radius[3] / Radius[7]));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        Earth.Draw(ourShader);
        //moon
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(Location[3][0], Location[3][1], Location[3][2]));
        double moon_a = 0.00036f * 2 * acos(-1) / T[3];
        moon_angle += moon_a;
        double moon_d = 5.0 * Radius[3] / Radius[7];
        double location1 = moon_d * cos(moon_angle);
        double location2 = Location[3][1];
        double location3 = moon_d * sin(moon_angle);
        model = glm::translate(model, glm::vec3(location1, location2, location3));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.25f * Radius[3] / Radius[7]));
        ourShader.setMat4("model", model);
        Moon.Draw(ourShader);
        //mars
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(Location[4][0], Location[4][1], Location[4][2]));
        model = glm::scale(model, glm::vec3(Radius[4] / Radius[7]));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        Mars.Draw(ourShader);
        //jupiter
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(Location[5][0], Location[5][1], Location[5][2]));
        model = glm::scale(model, glm::vec3(Radius[5] / Radius[7]));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        Jupiter.Draw(ourShader);
        //saturn
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(Location[6][0], Location[6][1], Location[6][2]));
        model = glm::scale(model, glm::vec3(Radius[6] / Radius[7]));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        Saturn.Draw(ourShader);
        //uranus
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(Location[7][0], Location[7][1], Location[7][2]));
        model = glm::scale(model, glm::vec3(Radius[7] / Radius[7]));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        Uranus.Draw(ourShader);
        //neptune
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(Location[8][0], Location[8][1], Location[8][2]));
        model = glm::scale(model, glm::vec3(Radius[8] / Radius[7]));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        Neptune.Draw(ourShader);
        //solar
        solarShader.use();
        /*solarShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        solarShader.setVec3("lightPos", 0.0f, 7.0f, -24.0f);
        solarShader.setVec3("viewPos", camera.Position);*/
        solarShader.setMat4("projection", projection);
        solarShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(Radius[0] / Radius[7]));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        solarShader.setMat4("model", model);
        Solar.Draw(solarShader);
        
        //实例化模型
        instShader.use();
        instShader.setInt("texture_diffuse1", 0);
        instShader.setMat4("projection", projection);
        instShader.setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
        for (unsigned int i = 0; i < rock.meshes.size(); i++)
        {
            glBindVertexArray(rock.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rock.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }
        
        //动态
        /*aniShader.use();
        
        projection = glm::perspective(glm::radians(camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        aniShader.setMat4("projection", projection);
        aniShader.setMat4("view", view);

        auto transforms = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
            aniShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));// translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));// it's a bit too big for our scene, so scale it down
        aniShader.setMat4("model", model);
        aModel.Draw(aniShader);
        */
        //天空盒
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        //交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        camera.MovementSpeed = 50.0f;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        camera.MovementSpeed = 10.0f;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}