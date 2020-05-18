

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>
#include <classes\shader_m.h>
#include <classes\camera.h>
#include <Material.h>
#include <model.h>
#include <Model2t.h>
#include <light.h>
#include <cube.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

extern float cube[];

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void drawModelsAtPositions(Model2t* model, int positionCount, glm::vec3* positionArray);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 1024;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


//lighting


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // world space positions of our cubes
    // pass projection matrix to shader (note that in this case it could change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


    //Materials:
    Material defaultMaterial(
        glm::vec3(1.0f, 0.5f, 0.31f)
        , glm::vec3(1.0f, 0.5f, 0.31f)
        , glm::vec3(0.5f, 0.5f, 0.5f)
        , 64);

    Material whiteMaterial(
        glm::vec3(1.0f, 1.0f, 1.0f)
        , glm::vec3(1.0f, 1.0f, 1.0f)
        , glm::vec3(1.0f, 1.0f, 1.0f)
        , 32);

    //light:
    Material lightIntensity(
          glm::vec3(0.4f, 0.4f, 0.4f)
        , glm::vec3(0.5f, 0.5f, 0.5f)
        , glm::vec3(1.0f, 1.0f, 1.0f)
        , 64);

    Light light(
        cube
        , sizeof(cube)
        , "textures/error.png"
        , "shaders/vShaderLight.vs"
        , "shaders/fShaderLight.fs"
        , &projection
        , &camera
        , lightIntensity
    );

    Model2t crate(
        cube
        , sizeof(cube)
        , "textures/container2.png"
        , "textures/container2_specular.png"
        , "shaders/vShader.vs"
        , "shaders/fShader.fs"
        , &projection
        , &camera
        , &light
        , defaultMaterial
    );

    Model stone(
        cube
        , sizeof(cube)
        , "textures/stone.png"
        , "shaders/vShader.vs"
        , "shaders/fShader.fs"
        , &projection
        , &camera
        , &light
        , whiteMaterial
    );

    glm::vec3 cubePositions[125];
    glm::vec3 offset(-2.5f, -1.5f, -2.5f);
    for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++) {
        cubePositions[i].x = (i % 5) + offset.x;
        cubePositions[i].y = (i / 5 % 5) + offset.y;
        cubePositions[i].z = (i / 25 % 5) + offset.z;
    }

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        glm::mat4 model = glm::mat4(1.0f);

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawModelsAtPositions(&crate, sizeof(cubePositions) / sizeof(glm::vec3), cubePositions);

        stone.setTranslation(0.0f, -3.0f, 0.0f);
        stone.setScale(100.0f, 0.1f, 100.0f);
        stone.draw();

        ////Update Light Position
        glm::vec3 lightPos(cos(glfwGetTime() / 2) * 5, 1.5f, sin(glfwGetTime()/ 2)  * 5);
        light.setTranslation(lightPos);
        light.setScale(0.1f, 0.1f, 0.1f);
        light.draw();

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

void drawModelsAtPositions(Model2t* model, int positionCount, glm::vec3* positionArray)
{

    for (unsigned int i = 0; i < positionCount; i++)
    {
        model->setTranslation(positionArray[i]);
        model->setRotation((float)glfwGetTime(), 0.3f, 0.5f, 1.0f);
        model->setScale(0.5f, 0.5f, 0.5f);
        model->draw();

    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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
    camera.ProcessMouseScroll(yoffset);
}

