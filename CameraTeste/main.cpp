#pragma once

// Bibliotecas OpenGL e GLM
#include <glad/glad.h>
#include <glm/glm/glm.hpp>

// Bibliotecas do projeto
#include "gameObject.h"
#include "main.h"
#include "shader.h"
#include "input.h"
#include "camera.h"
#include "light.h"
#include "teapot.h"
#include "scene.h"
#include "material.h"

// Outras bibliotecas
#include <iostream>
/** TODO:
1. GameObject
    2. Create function pointers for `beforeUpdate` and `afterUpdate`.
    3. Implement EBO optimization.
    4. Load data from files:
        4.0 Constructor should accept a file path instead of vertices and vertex count.
        4.1 Method to load vertices from files.
        4.2 Method to load normals from files.
        4.3 The must have EBO at somepoint
    5. Add texture options, including repeat, mipmap settings, and others.
    6. Determine if some elements can be moved to smaller, more specialized classes. https://www.youtube.com/watch?v=IMZMLvIwa-k

2. Material
    Done for now;
3. Create a default config file move the size of the window there default elements and everthing this is done to be easy to be set , maybe create
**/


// Constantes de tempo e janela
Material DEFAULT_MATERIAL;

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;
const double NEAR_FRUSTUM = 0.1f;
const double FAR_FRUSTUM = 1000000000;
const int AMOUNT_OF_SAMPLES_TO_COUNT_FPS = 2000;
const glm::mat4 PROJECTION = glm::perspective(glm::radians(55.0f), (float)WIDTH / HEIGHT, (float)NEAR_FRUSTUM, (float)FAR_FRUSTUM);

// Vari�veis de tempo
float DeltaTime = 0.0f;
float LastTime = 0.0f;

// Vari�veis de controle de posi��o do mouse
double LastXPos = -100000, LastYPos = -100000;

// Vari�veis de sensibilidade de movimento
float Speed = 8.0f;
float DefaultSpeed = 18.0f;
float VerticalSens = 100.0f;
float HorizontalSens = 100.0f;

// Janela e shaders
GLFWwindow* Window;
Shader* DEFAULT_SHADER_REFERENCE;


// C�meras
Camera cameraInst = Camera(glm::vec3(5, 2, -3), true);

// Matrizes de visualiza��o
glm::mat4 View;
int whereToChangeColor = 0;
bool adding = true;
void dinamicLightUpdate(GameObject* obj)
{

    float rate = 1.0f;  
    
    Light* l = (Light*)obj;
    glm::vec3 addToColor = l->getColor();

    addToColor[whereToChangeColor] += rate * DeltaTime * (adding ? 1 : -1);

  
    if (addToColor[whereToChangeColor] >= 1.0f)
    {
        addToColor[whereToChangeColor] = 1.0f;
        adding = false;
    }
    else if (addToColor[whereToChangeColor] <= 0.1f)
    {
        addToColor[whereToChangeColor] = 0.1f;
        adding = true;
    }
    if (addToColor[whereToChangeColor] == 1.0f || addToColor[whereToChangeColor] == 0.1f)
    {
        whereToChangeColor++;
        if (whereToChangeColor >= 3)
        {
            whereToChangeColor = 0;
        }
    }

    l->setColor(addToColor);

}

int FpsSampleCounter = 0;
float DeltatimeMean = 0.0f;
void printVec3(const glm::vec3& vec) 
{
    std::cout << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << "\n";
}

void objUpdate(GameObject *obj)
{
    if (isKeyPressed(Window, GLFW_KEY_Z) && isKeyPressed(Window, GLFW_KEY_LEFT_SHIFT))
    {
        obj->setRot(obj->getRot() + glm::vec3(0.0f, 0.0f , Speed * 40.0f * DeltaTime));
    }
    else if (isKeyPressed(Window, GLFW_KEY_Z))
    {
        obj->setScale(obj->getScale() + glm::vec3(0.0f, 0.0f, Speed * DeltaTime));
    }

    if (isKeyPressed(Window, GLFW_KEY_X) && isKeyPressed(Window, GLFW_KEY_LEFT_SHIFT))
    {
        obj->setRot(obj->getRot() + glm::vec3(Speed * 40.0f * DeltaTime, 0.0f , 0.0f));
    }
    else if (isKeyPressed(Window, GLFW_KEY_X))
    {
        obj->setScale(obj->getScale() + glm::vec3(Speed * DeltaTime, 0.0f, 0.0f));
    }

    if (isKeyPressed(Window, GLFW_KEY_Y) && isKeyPressed(Window, GLFW_KEY_LEFT_SHIFT))
    {
        obj->setRot(obj->getRot() + glm::vec3(0.0f, Speed * 40.0f * DeltaTime, 0.0f));
    }
    else if (isKeyPressed(Window, GLFW_KEY_Y))
    {
        obj->setScale(obj->getScale() + glm::vec3(0.0f, Speed * DeltaTime, 0.0f));
    }

    if (isKeyPressed(Window, GLFW_KEY_UP))
    {
        obj->setPos(obj->getPos() + glm::vec3(0.0f, 0.0f, Speed * DeltaTime));
    }

    if (isKeyPressed(Window, GLFW_KEY_DOWN))
    {
        obj->setPos(obj->getPos() + glm::vec3(0.0f, 0.0f, -Speed * DeltaTime));
    }

    if (isKeyPressed(Window, GLFW_KEY_RIGHT))
    {
        obj->setPos(obj->getPos() + glm::vec3(Speed * DeltaTime, 0.0f, 0.0f));
    }

    if (isKeyPressed(Window, GLFW_KEY_LEFT))
    {
        obj->setPos(obj->getPos() + glm::vec3(-Speed * DeltaTime, 0.0f, 0.0f));
    }

    
}

void keyInput()
{
    
    if (isKeyPressed(Window, GLFW_KEY_S))
    {
        glm::vec3 newPos = cameraInst.getPos() + glm::fvec1(-Speed * DeltaTime) * cameraInst.foward();
        cameraInst.setPos(newPos);
    }
    if (isKeyPressed(Window, GLFW_KEY_W))
    {
        glm::vec3  newPos = cameraInst.getPos() + glm::fvec1(Speed * DeltaTime) * cameraInst.foward();
        cameraInst.setPos(newPos);
       
    }
    if (isKeyPressed(Window, GLFW_KEY_D))
    {

        glm::vec3  newPos = cameraInst.getPos() + glm::fvec1(Speed * DeltaTime) * cameraInst.right();
        cameraInst.setPos(newPos);
    }
    if (isKeyPressed(Window, GLFW_KEY_A))
    {
        glm::vec3  newPos = cameraInst.getPos() + glm::fvec1(-Speed * DeltaTime) * cameraInst.right();
        cameraInst.setPos(newPos);
        

    }
    if (isKeyPressed(Window, GLFW_KEY_E))
    {
        cameraInst.setPos(cameraInst.getPos() + glm::vec3(0, Speed * DeltaTime, 0));
    }
    if (isKeyPressed(Window, GLFW_KEY_Q))
    {
        cameraInst.setPos(cameraInst.getPos() + glm::vec3(0, -Speed * DeltaTime, 0));
    }
    if (Speed <= DefaultSpeed && isKeyPressed(Window, GLFW_KEY_LEFT_SHIFT))
    {
        Speed  = DefaultSpeed * 8.0f;
    }
    else if (Speed <= DefaultSpeed && isKeyPressed(Window, GLFW_KEY_LEFT_CONTROL))
    {
        Speed = DefaultSpeed * 0.25f;

    }
    else
    {
        Speed = DefaultSpeed;
    }

    

}
void cameraMovement()
{
    double xpos, ypos;
    glfwGetCursorPos(Window, &xpos, &ypos);

    if (LastXPos == -100000 || LastYPos == -100000)
    {
        LastXPos = xpos; LastYPos = ypos;
        return;
    }

    cameraInst.horizontalRotation(HorizontalSens* DeltaTime* (LastXPos - xpos));
    cameraInst.verticalRotation(HorizontalSens * DeltaTime * (LastYPos - ypos));
    LastXPos = xpos; LastYPos = ypos;
}

void objAftherUpdate(GameObject* obj) 
{
    if (isKeyPressed(Window, GLFW_KEY_LEFT_CONTROL) && isKeyPressed(Window, GLFW_KEY_LEFT_SHIFT) && isKeyPressed(Window, GLFW_KEY_T))
    {
        cameraInst.setPos(obj->getPos() - glm::vec3(0, -1, -8.5));
        cameraInst.setTarget(obj->getPos());

    }
}

int init(GLFWwindow** window)
{
    if (!glfwInit())
    {
        printf("Failed to init glfw\n");
        glfwTerminate();
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /*Telling GLFW we want to use the core-profile means we'll get access to a smaller subset of OpenGL features without backwards-compatible features we no longer need    Mac OS X you need to add glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); to your initialization code for it to work*/
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    *window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpengl", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(*window);
    //Avisando ao GLFW que queremos que essa funcao seja chamada toda vez que a janela eh redimensionada
    glfwSetFramebufferSizeCallback(*window, framebufferSizeCallback);
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    /*******************Set GLFW********************/
    /*******************Set GLAD********************/

    //O Glad gerencia ponteiros para funcoes do OpenGL, entao precisamos inicializar o GLAD antes the usarmos funcoes do OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))/*GLFW nos da glfwGetProcAddress que define a funcao correta baseado em qual sistema operacional estamos compilando para .*/
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }
    /*******************Set GLAD********************/

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);
    return 0;
}
/*******************/
int main()
{

    if (init(&Window) == -1)
    {
        return -1;
    }
    
  
    float vertices[] = {
        // Posi��es
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    float texture[] = {
        // Coordenadas de textura
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    

    Shader DefaultShader("Shaders/vertexShaderDefault.glsl", "Shaders/fragmentShaderDefault.glsl");
    Shader LightShader("Shaders/vertexShaderDefault.glsl", "Shaders/fragmentShaderLight.glsl");
    DEFAULT_SHADER_REFERENCE = &DefaultShader;
    const unsigned int numVertices = sizeof(vertices) / sizeof(vertices[0]);


    #pragma region Lights
    Light l1 = Light(glm::vec3(25.0f, 100.0f, -25.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.65f, 0.011f), false, 150.0f);
    Light l2 = Light(glm::vec3(0.0f, 145.0f, -800.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.9f, 0.7f), false, 8000.0f);
    Light l3 = Light(glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.8f, 1.0f), false, 25.0f);
    Light l4 = Light(glm::vec3(-240.0f, 10.0f, 240.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.8f, 0.6f), false, 25.0f);
    Light l5 = Light(glm::vec3(-330.0f, 100.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), false, 25.0f);
    Light l6 = Light(glm::vec3(0.0f, 8.0f, 240.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.3f), false, 25.4f);
    Light l7 = Light(glm::vec3(0.0f, 820.0f, 350.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), false, 30.5f);
    Light l8 = Light(glm::vec3(180.0f, 50.0f, -100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.8f, 0.6f), false, 50.0f);
    Light l9 = Light(glm::vec3(25.0f, 5.0f, -105.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.011f, 0.011f, 1.0f), false, 14.6f);
    Light l10 = Light(glm::vec3(25.0f, 5.0f, -70.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.011f, 0.011f), false, 14.6f);
    Light* lights[10] = { &l1, &l2, &l3, &l4, &l5,&l6,&l7,&l8,&l9,&l10 };
    l7.setUpdateFunc(&dinamicLightUpdate);
    #pragma endregion

    #pragma region GameObjects
    // Material(ambientV = DEFAULT_AMBIENT,diffuseV = DEFAULT_DIFFUSE,colorV = DEFAULT_COLOR,roughnessAmt = DEFAULT_ROUGHNESS,amtOfSpecular = DEFAULT_SPECULAR_AMOUNT)
    //Material(const glm::vec4& colorV, const float& roughnessAmt, const float& amtOfSpecular)
    Material veryShine(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), 20.0f, 25.0f);

    Material veryShineBlue(glm::vec4(0.0999f, 0.09991f, 0.35f, 1.0f), 20, 250);

    Material veryRoughGray(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), 750, 0.5);

    Material veryRoughBlue(glm::vec4(0.0999f, 0.09991f, 0.4f, 1.0f), 750, 0.5);

    GameObject obj = GameObject(glm::vec3(0.0f, 0.0f, -40.0f), glm::vec3(0.0f, 0.0f, 0.0f), vertices, numVertices, &DefaultShader, true);

    const char* pathss[] = { "TextureImages/scene.jpg" };
    obj.setTextures(texture, sizeof(texture), pathss, 1);
    obj.setScale(obj.getScale()* glm::fvec1(2));

    GameObject teapot = GameObject(glm::vec3(5.0f, 5.0f, -5.0f), glm::vec3(0.0f, 0.0f, 180.0f), teapotVertices, teapot_count, DEFAULT_SHADER_REFERENCE, false);
    teapot.setUpdateFunc(&objUpdate);
    teapot.setAftherUpdateFunc(&objAftherUpdate);
    teapot.setMaterial(&veryRoughBlue);

    GameObject ground = GameObject(glm::vec3(0.0f, -1.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), vertices, numVertices, &DefaultShader, true);
    ground.setScale(glm::vec3(500.0f, 1.0f, 500.0f));
    ground.setMaterial(&veryRoughGray);

    GameObject wall = GameObject(glm::vec3(0.0f, 248.5f, -250.0f), glm::vec3(0.0f, 0.0f, 0.0f), vertices, numVertices, &DefaultShader, true);
    wall.setScale(glm::vec3(500.0f, 500.0f, 1.0f));
    wall.setMaterial(&veryShine);

    GameObject wall2 = GameObject(glm::vec3(250.0f, 248.5f, 0.0f), glm::vec3(0.0f, 90.0f, 0.0f), vertices, numVertices, &DefaultShader, true);
    wall2.setScale(glm::vec3(500.0f, 500.0f, 1.0f));
    wall2.setMaterial(&veryRoughGray);

    GameObject bigBlock = GameObject(glm::vec3(100.0f, 100.5f, -100.0f), glm::vec3(0.0f, 0.0f, 0.0f), vertices, numVertices, &DefaultShader, true);
    bigBlock.setScale(glm::vec3(50.0f, 50.0f, 50.0f));
    bigBlock.setMaterial(&veryShine);
    bigBlock.setTextures(texture, sizeof(texture), pathss, 1);

    GameObject bigBlock2 = GameObject(glm::vec3(-100.0f, 100.5f, -100.0f), glm::vec3(0.0f, 0.0f, 0.0f), vertices, numVertices, &DefaultShader, true);
    bigBlock2.setScale(glm::vec3(50.0f, 50.0f, 50.0f));
    bigBlock2.setMaterial(&veryShineBlue);

    GameObject* arr[17] = { &obj, &teapot, &ground, &wall, &bigBlock, &bigBlock2, &wall2};

    // Adicionando representa��o das luzes
    for (int i = 0; i < 10; i++) 
    {
        //lights[i]->getPos() 
        lights[i]->enablePhysicalRepresentation(vertices, numVertices, &LightShader);
        lights[i]->setScale(lights[i]->getScale()* glm::fvec1(lights[i]->getIntensity()/20));
        if (i == 0)
        {
           
            const char* paths[] = { "TextureImages/glowstone.jpg" };
            lights[i]->setTextures(texture, sizeof(texture), paths, 1);
            
        }
       
        //lights[i]->setTextures()
        arr[i + 7] = lights[i];
    }

    l2.setModelMatixMode(1);// set different type of model matrix to rotate around origin
    #pragma endregion
    float lightSpeed = -55.0f;
    Scene mainScene(arr, 17, &cameraInst, PROJECTION);

    while (!glfwWindowShouldClose(Window))
    {
        processInput(Window);
       
        glClearColor(0.1f, 0.1f, 0.3f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float timeValue = glfwGetTime();
        DeltaTime = timeValue - LastTime;
        LastTime = timeValue;
            
        l2.setRot(l2.getRot() + glm::vec3(100.0f * DeltaTime,0.0f, 0.0f));


        fpsLog();


        keyInput();
        cameraMovement();
        // Check position and adjust speed direction
        if (l1.getPos().z <= -150.0f && lightSpeed < 0)
        {
            lightSpeed *= -1; // Reverse direction when hitting the lower bound
        }
        else if (l1.getPos().z >= -30.0f && lightSpeed > 0)
        {
            lightSpeed *= -1; // Reverse direction when hitting the upper bound
        }

        // Update the position with the adjusted speed
        l1.setPos(l1.getPos() + glm::vec3(0.0f, 0.0f, DeltaTime * lightSpeed));


        mainScene.render();
       


        glfwSwapBuffers(Window);
        glfwPollEvents();

    }
    

    glfwTerminate();
   /* for (int i = 3; i < 13; i++)
    {
        delete arr[i];
    }*/
    return 0;
}

void fpsLog()
{
    DeltatimeMean = (DeltatimeMean * 0.9f + DeltaTime * 0.1f) / 2.0f;
    FpsSampleCounter++;
    if (FpsSampleCounter >= AMOUNT_OF_SAMPLES_TO_COUNT_FPS)
    {
        FpsSampleCounter = 0;
        std::cout << "Fps: " << int(1 / DeltaTime) << "\n";
    }
}
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
