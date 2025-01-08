//
//  main.cpp
//  3D Object Drawing
//
//  Created by Waliul Islam Sumon
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "pointLight.h"
#include "directionalLight.h"
#include "sphere.h"

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b);
void drawTriangle(unsigned int& triangleVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b);
//void bed(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 alTogether);


// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;

// camera
Camera camera(glm::vec3(3.5f,2.0f, 8.0f));
float lastX = SCR_WIDTH / 3.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// direction of directional light
glm::vec3 directionalLight_direction = glm::vec3(-1.0f, 1.0f, -1.0f);

DirectionalLight directionalLight(-directionalLight_direction, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));


// positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(-1.5f,  1.2f,  2.5f),
    glm::vec3(6.0f,  1.2f,  2.5f),
    glm::vec3(6.0f,  1.2f,  5.5f),
    glm::vec3(-1.5f,  1.2f,  5.5f)
};

PointLight pointlight1(

    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1       // light number
);
PointLight pointlight2(

    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    2       // light number
);
PointLight pointlight3(

    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    3       // light number
);
PointLight pointlight4(

    pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    4       // light number
);


// light settings
bool directionalLightOn = false;
bool pointLightOn = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;


// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;


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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
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

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // --------------------------------------------------------------------- Cube

    float cube_vertices[] = {
        // positions      // normals
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);



    // second, configure the light's VAO ------------------------------------ Light Cube
    unsigned int lightCubeVAO, lightVBO, lightEBO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &lightVBO);
    glGenBuffers(1, &lightEBO);

    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    //------------------------------------------------- 3D Triangle
    float triangle_vertices[] = {
        // positions      // normals
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, -1.0f, -1.0f, 1.0f, 0.0f,

        0.5f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, -1.0f, -1.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f,

        1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    };

    unsigned int triangleVAO, triangleVBO;
    glGenVertexArrays(1, &triangleVAO);
    glGenBuffers(1, &triangleVBO);

    glBindVertexArray(triangleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    //ourShader.use();
    //lightingShader.use();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        pointlight1.setUpPointLight(lightingShader);
        pointlight2.setUpPointLight(lightingShader);
        pointlight3.setUpPointLight(lightingShader);
        pointlight4.setUpPointLight(lightingShader);

        directionalLight.setUpLight(lightingShader);

        // activate shader
        lightingShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        lightingShader.setMat4("projection", projection);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("view", view);


        // Modelling Transformation
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));


        ////Drawing a sphere
        //Sphere sphere1 = Sphere();
        //                 //r    g     b      values
        //sphere1.setColor(0.2f, 1.0f, 0.3f);
        //translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.8f, 0.0f, 0.0f));
        //scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.7f, 0.4f, 0.7f));
        //model = translateMatrix * scaleMatrix;
        //sphere1.drawSphere(lightingShader, model);


        ////Drawing a triangle
        //translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f, -0.2f));
        //scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.5f, 0.7f, 1.0f));
        //model = translateMatrix * scaleMatrix;
        //                                                 //r    g     b      values
        //drawTriangle(triangleVAO, lightingShader, model, 0.8f, 0.3f, 1.0f);


        ////Drawing a cube
        //translateMatrix = glm::translate(identityMatrix, glm::vec3(1.8f, -0.4f, -0.6f));
        //scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
        //model = translateMatrix * scaleMatrix;
        //                                         //r    g     b      values
        //drawCube(cubeVAO, lightingShader, model, 0.1f, 0.6f, 1.0f);


        // Also draw the lamp object(s)
        // The white cubes are lamp objects
        // As we can't see the point where the point lights are, we are representing the points with the white cubes
        // We don't need the lighting effects on the cubes, that's why we are using simple shader, with only one color element
        // fragmentShader.fs and vertexShader.fs are the simple shaders codes. 
        // SEE THESE TWO FILES!

          //walls--------------------------/
          
        //Drawing a cube bottom wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.7f, -0.7f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(13.5f, 0.2f, 7.8f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.6f, 0.6f, 0.7f );

        //Drawing a cube back wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.7f, -0.7f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(13.5f, 2.1f, 0.2f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.7f, 0.8f, 0.9f);

        //Drawing a cube left wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.7f, -0.7f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 2.1f, 7.8f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.7f, 0.8f, 0.9f);

        //Drawing a cube back wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.7f, -0.7f, 7.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(13.5f, 2.1f, 0.2f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.7f, 0.8f, 0.9f);



        //Drawing a cube right wall
        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.6, -0.7f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 2.1f, 7.8f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.7f, 0.8f, 0.9f);

        //Drawing a cube middle wall 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, -0.7f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 1.2f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, -0.7f, 3.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 1.6f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.1f, -0.7f, 7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 0.8f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);



        //Drawing a cube middle wall 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.65f, -0.7f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 1.2f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.65f, -0.7f, 3.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 1.6f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.65f, -0.7f, 7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 0.8f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        //Drawing a cube middle wall 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.6f, -0.7f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 1.2f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.6f, -0.7f, 3.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 1.6f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);


        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.6f, -0.7f, 7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 0.8f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        //Drawing a cube middle wall 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.85f, -0.7f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 1.2f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.85f, -0.7f, 3.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 1.6f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);


        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.85f, -0.7f, 7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 0.8f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        //Drawing a cube middle wall 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.35f, -0.7f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 1.2f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.35f, -0.7f, 3.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 1.6f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.35f, -0.7f, 7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 0.8f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);


        //Drawing a cube middle wall 7
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.1f, -0.7f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 1.2f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.1f, -0.7f, 3.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 1.6f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.1f, -0.7f, 7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 1.5f, 0.8f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.91f, 0.75f, 0.50f);


        //Drawing a cube back table
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.7f, 0.0f, 0.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.80f, 0.05f, 0.8f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.87f, 0.68f, 0.45f);

        //divider wall 
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.7f, 0.0f, 4.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.8f, 0.6f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.87f, 0.68f, 0.45f);



        //Drawing a cube middle table 
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.7f, 0.0f, 3.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.80f, 0.05f, 1.6f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.87f, 0.68f, 0.45f);

        //Drawing a cube front table 
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.7f, 0.0f, 7.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(10.80f, 0.05f, 0.8f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.87f, 0.68f, 0.45f);


        //Drawing a cube middle chair
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.2f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.2f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair four legs
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.62, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.62, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);



        //Drawing a cube middle right chair
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.2f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.2f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);


        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.2f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.2f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);


        //Drawing a cube middle right chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.2f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        //Drawing a cube middle right chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.2f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        //Drawing a cube middle right chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.2f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);


        //Drawing a cube middle right chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.2f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

      /*  ==============================================================================*/


        //Drawing a cube middle left chair 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.2f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle left chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.2f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle left chair four legs
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.52, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.52, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle left chair 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.2f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle left chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.2f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle left chair four legs
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.5f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.52, -0.5f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.52, -0.5f, 3.04f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.5f, 3.04f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle left chair 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.2f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle left chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.2f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle left chair four legs
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.52, -0.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.52, -0.5f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.5f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle left chair 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.2f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle left chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.2f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle left chair four legs
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.5f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.52, -0.5f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.52, -0.5f, 6.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.5f, 6.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);







        //Drawing a cube middle chair  2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.2f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.2f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair four legs
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.62, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.62, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle chair  2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.2f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.2f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair four legs
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.5f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.62, -0.5f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.62, -0.5f, 3.04f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.5f, 3.04f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle chair  2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.2f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.2f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair four legs
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.62, -0.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.62, -0.5f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.5f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle chair  2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.2f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.2f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair four legs
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.5f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.62, -0.5f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.62, -0.5f, 6.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, -0.5f, 6.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);


        //Drawing a cube middle chair four legs 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.2f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.2f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.5f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.08, -0.5f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.08, -0.5f, 3.04f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.5f, 3.04f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle chair four legs 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.2f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.2f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.08, -0.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.08, -0.5f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.5f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle chair four legs 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.2f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.2f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.5f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.08, -0.5f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.08, -0.5f, 6.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.8, -0.5f, 6.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle right chair four legs 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.2f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.2f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.83, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.83, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);


        //Drawing a cube middle right chair four legs 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.2f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.2f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.5f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.83, -0.5f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.83, -0.5f, 3.04f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.5f, 3.04f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle right chair four legs 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.2f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.2f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.83, -0.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.83, -0.5f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.5f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle right chair four legs 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.2f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.2f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.5f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.83, -0.5f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.83, -0.5f, 6.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.55, -0.5f, 6.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);


        //Drawing a cube middle right chair four legs 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.2f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.2f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.58, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.58, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle right chair four legs 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.2f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.2f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.5f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.58, -0.5f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.58, -0.5f, 3.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.5f, 3.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle right chair four legs 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.2f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.2f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.58, -0.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.58, -0.5f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.5f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle right chair four legs 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.2f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.2f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.5f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.58, -0.5f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.58, -0.5f, 6.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, -0.5f, 6.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);




        //Drawing a cube middle right chair four legs 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.2f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.2f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.28, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.28, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle right chair four legs 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.2f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.2f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.5f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.28, -0.5f, 2.8f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.28, -0.5f, 3.04f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.5f, 3.04f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle right chair four legs 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.2f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.2f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.28, -0.5f, 5.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.28, -0.5f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.5f, 5.24f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);

        //Drawing a cube middle right chair four legs 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.2f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.2f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.5f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.28, -0.5f, 6.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.28, -0.5f, 6.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, -0.5f, 6.84f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);


        //Drawing a cube middle left chair
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.2f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.26f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle left chair back rest
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.2f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.4f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.85f, 0.65f, 0.20f);
        //Drawing a cube middle left chair four legs
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.52, -0.5f, 1.16f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.52, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.8, -0.5f, 0.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.3f, 0.02f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 1.0f, 1.0f, 1.0f);


        //Drawing a cube middle table monitor
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.98, 0.1f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);
        //Drawing a cube middle table monitor stand
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.78, 0.0f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);
        
        //Drawing a cube middile table keyboard
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, 0.05f, 0.55f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile table CPU
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.26, 0.06f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);


        //Drawing a cube middle right table monitor
        translateMatrix = glm::translate(identityMatrix, glm::vec3( 0.73, 0.1f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle right table monitor
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.46, 0.1f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle right table monitor
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.23, 0.1f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);



        //Drawing a cube middle right table monitor
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.92, 0.1f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle back table monitor 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.92, 0.1f, 3.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle front table monitor 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.92, 0.1f, 4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube front table monitor 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.92, 0.1f, 7.71f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);



        //Drawing a cube middle back table monitor 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.23, 0.1f, 3.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle front table monitor 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.23, 0.1f, 4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle front table monitor 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.23, 0.1f, 7.71f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle back table monitor 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.46, 0.1f, 3.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle front table monitor 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.46, 0.1f, 4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle front table monitor 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.46, 0.1f, 7.71f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle back table monitor 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.73, 0.1f, 3.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle front table monitor 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.73, 0.1f, 4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle front table monitor 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.73, 0.1f, 7.71f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle back table monitor 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.98, 0.1f, 3.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle front table monitor 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.98, 0.1f, 4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle front table monitor 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.98, 0.1f, 7.71f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle back table monitor 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.74, 0.1f, 3.9f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle front table monitor 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.74, 0.1f, 4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middle front table monitor 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.74, 0.1f, 7.71f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube back table monitor stand 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.7, 0.0f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube middle back table monitor stand 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.54, 0.0f, 3.91f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube middle back table monitor stand 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.54, 0.0f, 4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube middle back table monitor stand 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.54, 0.0f, 7.72f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.78, 0.0f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);
        //Drawing a cube back table monitor stand 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.78, 0.0f, 3.91f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.78, 0.0f, 4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.78, 0.0f, 7.72f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3( 0.93, 0.0f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);
        //Drawing a cube back table monitor stand 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.93, 0.0f, 3.91f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.93, 0.0f, 4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.93, 0.0f, 7.72f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.66, 0.0f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.66, 0.0f, 3.91f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.66, 0.0f, 4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.66, 0.0f, 7.72f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.43, 0.0f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.43, 0.0f, 3.91f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.43, 0.0f, 4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.43, 0.0f, 7.72f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

         
        //Drawing a cube back table monitor stand 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.13, 0.0f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.13, 0.0f, 3.91f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.13, 0.0f, 4.1f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);

        //Drawing a cube back table monitor stand 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.13, 0.0f, 7.72f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);


        //Drawing a middle table mouse 1
        Sphere sphere14 = Sphere();
        //rgb
        sphere14.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.78, 0.05f, 3.45f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere14.drawSphere(lightingShader, model);

        //Drawing a middle table mouse 1
        Sphere sphere34 = Sphere();
        //rgb
        sphere34.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.78, 0.05f, 7.28f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere14.drawSphere(lightingShader, model);

        //Drawing a middle table mouse 1
        Sphere sphere24 = Sphere();
        //rgb
        sphere24.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.3, 0.05f, 4.58f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere24.drawSphere(lightingShader, model);

        //Drawing a middle table mouse 2
        Sphere sphere3 = Sphere();
        //rgb
        sphere3.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.54, 0.05f, 0.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere3.drawSphere(lightingShader, model);

        //Drawing a middle table mouse 2
        Sphere sphere25 = Sphere();
        //rgb
        sphere25.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.54, 0.05f, 4.58f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere25.drawSphere(lightingShader, model);

        //Drawing a middle table mouse 2
        Sphere sphere35 = Sphere();
        //rgb
        sphere25.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.94, 0.05f, 7.28f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere25.drawSphere(lightingShader, model);

        //Drawing a middle table mouse 2
        Sphere sphere13 = Sphere();
        //rgb
        sphere13.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.94, 0.05f, 3.45f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere13.drawSphere(lightingShader, model);



        //Drawing a middle right table mouse 3
        Sphere sphere4 = Sphere();
        //rgb
        sphere4.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.18, 0.05f, 0.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere4.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 3
        Sphere sphere26 = Sphere();
        //rgb
        sphere26.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(1.18, 0.05f, 4.58f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere4.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 3
        Sphere sphere15 = Sphere();
        //rgb
        sphere15.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.7, 0.05f, 3.45f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere15.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 3
        Sphere sphere45 = Sphere();
        //rgb
        sphere45.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.7, 0.05f, 7.28f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere45.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 4
        Sphere sphere6 = Sphere();
        //rgb
        sphere6.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.9, 0.05f, 0.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere4.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 4
        Sphere sphere27 = Sphere();
        //rgb
        sphere27.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.9, 0.05f, 4.58f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere27.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 4
        Sphere sphere16 = Sphere();
        //rgb
        sphere16.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5, 0.05f, 3.45f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere16.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 4
        Sphere sphere36 = Sphere();
        //rgb
        sphere36.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5, 0.05f, 7.28f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere36.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 5
        Sphere sphere7 = Sphere();
        //rgb
        sphere7.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.67, 0.05f, 0.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere4.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 5
        Sphere sphere28 = Sphere();
        //rgb
        sphere28.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.67, 0.05f, 4.58f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere4.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 5
        Sphere sphere17 = Sphere();
        //rgb
        sphere17.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.2, 0.05f, 3.45f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere14.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 5
        Sphere sphere37 = Sphere();
        //rgb
        sphere37.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.2, 0.05f, 7.28f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere14.drawSphere(lightingShader, model);


        //Drawing a middle right table mouse 6
        Sphere sphere8 = Sphere();
        //rgb
        sphere8.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.4, 0.05f, 0.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere4.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 6
        Sphere sphere29 = Sphere();
        //rgb
        sphere29.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.4, 0.05f, 4.58f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere4.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 6
        Sphere sphere18 = Sphere();
        //rgb
        sphere18.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.95, 0.05f, 3.45));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere18.drawSphere(lightingShader, model);

        //Drawing a middle right table mouse 6
        Sphere sphere38 = Sphere();
        //rgb
        sphere38.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.95, 0.05f, 7.28));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere38.drawSphere(lightingShader, model);

        //Drawing a cube middile left table keyboard 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.83, 0.05f, 0.55f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile left table keyboard 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.78, 0.05f, 4.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile left table keyboard 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.7, 0.05f, 3.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);


        //Drawing a cube middile left table keyboard 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.7, 0.05f, 7.22f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);


        //Drawing a cube middile table keyboard 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, 0.05f, 0.55f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile table keyboard 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.9, 0.05f, 4.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile table keyboard 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.85, 0.05f, 3.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);


        //Drawing a cube middile table keyboard 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-0.85, 0.05f, 7.22f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table keyboard 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.80, 0.05f, 0.55f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table keyboard 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.80, 0.05f, 4.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table keyboard 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.78, 0.05f, 3.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table keyboard 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.78, 0.05f, 7.22f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);



        //Drawing a cube middile right table keyboard 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5, 0.05f, 0.55f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table keyboard 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5, 0.05f, 4.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table keyboard 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.6, 0.05f, 3.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table keyboard 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.6, 0.05f, 7.22f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);


        //Drawing a cube middile right table keyboard 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, 0.05f, 0.55f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table keyboard 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.3, 0.05f, 4.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);


        //Drawing a cube middile right table keyboard 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.35, 0.05f, 3.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);


        //Drawing a cube middile right table keyboard 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(4.35, 0.05f, 7.22f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table keyboard 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, 0.05f, 0.55f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table keyboard 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0, 0.05f, 4.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table keyboard 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.05, 0.05f, 3.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);


        //Drawing a cube middile right table keyboard 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.05, 0.05f, 7.22f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);


        //Drawing a cube middile table CPU 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.18, 0.06f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile table CPU 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.18, 0.06f, 3.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile table CPU 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.18, 0.06f, 4.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);


        //Drawing a cube middile table CPU 1
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.18, 0.06f, 7.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile table CPU 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.26, 0.06f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile table CPU 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.26, 0.06f, 3.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile table CPU 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.26, 0.06f, 4.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile table CPU 2
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.26, 0.06f, 7.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.4, 0.06f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.4, 0.06f, 3.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.4, 0.06f, 4.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 3
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.4, 0.06f, 7.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.1, 0.06f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.1, 0.06f, 3.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.1, 0.06f, 4.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 4
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.1, 0.06f, 7.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.9, 0.06f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.9, 0.06f, 3.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.9, 0.06f, 4.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 5
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.9, 0.06f, 7.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);


        //Drawing a cube middile right table CPU 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.6, 0.06f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.6, 0.06f, 3.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.6, 0.06f, 4.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile right table CPU 6
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.6, 0.06f, 7.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);






        //Drawing a cube middle left table monitor
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.90, 0.1f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.45f, 0.3f, 0.015f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);
        //Drawing a cube middle left table monitor stand
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.7, 0.0f, 0.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 0.25f, 0.01f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.0f, 0.0f, 0.0f);
        //Drawing a middle left table mouse
        Sphere sphere5 = Sphere();
        //rgb
        sphere5.setColor(0.1f, 0.1f, 0.1f);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.45, 0.05f, 0.6f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f, 0.013f, 0.03f));
        model = translateMatrix * scaleMatrix;
        sphere5.drawSphere(lightingShader, model);
        //Drawing a cube middile left table keyboard
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.83, 0.05f, 0.55f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.3f, 0.02f, 0.12f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

        //Drawing a cube middile table CPU
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.18, 0.06f, 0.4f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);



        //Drawing a cube middle table 
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, -0.6f, 6.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.8f, 0.7f, 1.2f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.95f, 0.9f, 0.8f);

        //Drawing a cube middle table 
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.95f, 0.1f, 5.95f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.9f, 0.02f, 1.3f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.95f, 0.9f, 0.8f);
        //Screen
        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.58f, 0.2f, 3.5f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.08f, 1.1f, 2.0f));
        model = translateMatrix * scaleMatrix;
        drawCube(cubeVAO, lightingShader, model, 0.95f, 0.95f, 0.95f );

        
       //Teachers Monitor
       translateMatrix = glm::translate(identityMatrix, glm::vec3(8.2f, 0.17f, 6.2f));
       scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.015f, 0.3f, 0.45f));
       model = translateMatrix * scaleMatrix;
                                                //r    g     b      values
       drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);

       //Teachers Monitor stand
       translateMatrix = glm::translate(identityMatrix, glm::vec3(8.2f, 0.12f, 6.4f));
       scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.01f, 0.25f, 0.05f));
       model = translateMatrix * scaleMatrix;
       //r    g     b      values
       drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);


       ///Drawing a cube keyboard
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.4f, 0.12f, 6.2f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.12f, 0.02f, 0.3f));
        model = translateMatrix * scaleMatrix;
                                                 //r    g     b      values
        drawCube(cubeVAO, lightingShader, model, 0.2f, 0.2f, 0.2f);



        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            ourShader.setMat4("model", model);
            if (pointLightOn)
                ourShader.setVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
            else
                ourShader.setVec3("color", glm::vec3(0.25f, 0.25f, 0.25f));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f)
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
    lightingShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void drawTriangle(unsigned int& triangleVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f)
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
    lightingShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setMat4("model", model);

    glBindVertexArray(triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 24);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        if (directionalLightOn)
        {
            directionalLight.turnOff();
            directionalLightOn = !directionalLightOn;
        }
        else
        {
            directionalLight.turnOn();
            directionalLightOn = !directionalLightOn;
        }
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        if (pointLightOn)
        {
            pointlight1.turnOff();
            pointlight2.turnOff();
            pointlight3.turnOff();
            pointlight4.turnOff();
            pointLightOn = !pointLightOn;
        }
        else
        {
            pointlight1.turnOn();
            pointlight2.turnOn();
            pointlight3.turnOn();
            pointlight4.turnOn();
            pointLightOn = !pointLightOn;
        }
    }


    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (specularToggle)
        {

            pointlight1.turnSpecularOff();
            pointlight2.turnSpecularOff();
            pointlight3.turnSpecularOff();
            pointlight4.turnSpecularOff();

            specularToggle = !specularToggle;
        }
        else
        {

            pointlight1.turnSpecularOn();
            pointlight2.turnSpecularOn();
            pointlight3.turnSpecularOn();
            pointlight4.turnSpecularOn();
            specularToggle = !specularToggle;
        }
    }

    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        if (diffuseToggle)
        {

            pointlight1.turnDiffuseOff();
            pointlight2.turnDiffuseOff();
            pointlight3.turnDiffuseOff();
            pointlight4.turnDiffuseOff();
            diffuseToggle = !diffuseToggle;
        }
        else
        {

            pointlight1.turnDiffuseOn();
            pointlight2.turnDiffuseOn();
            pointlight3.turnDiffuseOn();
            pointlight4.turnDiffuseOn();
            diffuseToggle = !diffuseToggle;
        }
    }

    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        if (ambientToggle)
        {

            pointlight1.turnAmbientOff();
            pointlight2.turnAmbientOff();
            pointlight3.turnAmbientOff();
            pointlight4.turnAmbientOff();
            ambientToggle = !ambientToggle;
        }
        else
        {

            pointlight1.turnAmbientOn();
            pointlight2.turnAmbientOn();
            pointlight3.turnAmbientOn();
            pointlight4.turnAmbientOn();
            ambientToggle = !ambientToggle;
        }
    }
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
