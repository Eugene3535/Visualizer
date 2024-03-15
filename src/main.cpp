#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.hpp"
#include "graphics/Shader.hpp"
#include "loaders/MeshLoader.hpp"
#include "meshes/Grid.hpp"

const unsigned SCR_WIDTH = 1200;
const unsigned SCR_HEIGHT = 800;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLFWwindow* window;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float xoffset;
float yoffset;
bool camUpdate = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
bool IsKeyPressed(int key);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Grid", nullptr, nullptr);

    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

//  Callbacks
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);

    glDebugMessageCallback( [](GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
        {
            fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                    ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message );
        }, 0 );
#endif

//  Shaders
    Shader shader;
    {
        bool sucsess = (shader.compile("res/shaders/shader.vert", "res/shaders/shader.frag"));

        if(!sucsess)
        {
            std::cout << "Failed to compile shader\n";
            glfwTerminate();
            return -1;
        }
    }
    shader.bind(&shader);

    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


    glm::mat4() * glm::vec4();


    int MVP = shader.getUniformLocation("MVP");

    if(MVP == -1)
    {
        std::cout << "Failed to set uniform\n";
        glfwTerminate();
        return -1;
    }

    std::filesystem::path mesh_path("res/meshes/plane.txt");
    
    if(!std::filesystem::exists(mesh_path))
    {
        if(!std::filesystem::create_directory(std::filesystem::path("res/meshes")))
            return -1;;

        if(!MeshLoader().create_plane(mesh_path, 40, 40)) // grid size in cells
            return -1;
    }

    Grid grid;

    if(!grid.load_from_file(mesh_path))
        return -1;

    {// TEST CASE
        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;
        MeshLoader().load_model_from_file(std::filesystem::path("res/models/rabbit.obj"), vertices, indices);
    }

    Camera camera;

//  Main loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if(IsKeyPressed(GLFW_KEY_ESCAPE)) 
        {
            glfwSetWindowShouldClose(window, true);
            continue;
        }

        if (IsKeyPressed(GLFW_KEY_W))
            camera.move(Camera::FORWARD, deltaTime);

        if (IsKeyPressed(GLFW_KEY_S))
            camera.move(Camera::BACKWARD, deltaTime);

        if (IsKeyPressed(GLFW_KEY_A))
            camera.move(Camera::LEFT, deltaTime);

        if (IsKeyPressed(GLFW_KEY_D))
            camera.move(Camera::RIGHT, deltaTime);


        if (camUpdate)
        {
            camera.rotate(xoffset * 0.1f, yoffset * 0.1f);
            camUpdate = false;
        }

        Shader::bind(&shader);

        glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(projection * camera.getViewMatrix()));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        grid.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camUpdate = true;
    xoffset = xpos - lastX;
    yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;
}

bool IsKeyPressed(int key)
{
    if( ! window) 
        return false; // Not initialized

    return glfwGetKey(window, key) == GLFW_PRESS;
}