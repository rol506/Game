#include "Renderer/VertexBufferLayout.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

//get path
#ifdef __linux__
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#endif

#include "Resources/ResourceManager.h"
#include "Renderer/ShaderProgram.h"
#include "Renderer/Texture2D.h"
#include "Renderer/VertexArray.h"
#include "Renderer/IndexBuffer.h"

#include <glm/vec2.hpp>

glm::ivec2 gWindowSize(640, 480);

static GLfloat vertices[] = {
  
  // 1 - 2
  // | / |
  // 0 - 3

  //X      Y     Z     U     V
  -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 1
  -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, // 2
   0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // 3
   0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 4 
};

static GLuint indices[] = {
  0, 1, 2,
  0, 2, 3,
};

static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
}

static void glfwFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  gWindowSize.x = width;
  gWindowSize.y = height;
  glViewport(0, 0, width, height);
}

int main(void)
{
  GLFWwindow* window;
  std::string executablePath;

//get executable path
#ifdef __linux__
  char* path = (char*)malloc(1024);
  ssize_t r;
  r = readlink("/proc/self/exe", path, 1023);
  executablePath = path;
  delete path;
#elif _WIN32

  char* path = (char*)malloc(1024);
  GetModuleFileName(NULL, path, 1024);
  executablePath = path;
  delete path;

#endif

  std::cout << "Executable path: " << executablePath << "\n";

  ResourceManager::setExecutablePath(executablePath);

  /* Initialize the library */
  if (!glfwInit())
  {
    std::cout << "Failed to load GLFW!\n";
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(gWindowSize.x, gWindowSize.y, "Game by rol506", NULL, NULL);
  if (!window)
  {
    std::cout << "Failed to create window!\n";
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  if (!gladLoadGL())
  {
    std::cout << "Failed to load GlaD!\n";
    glfwTerminate();
    return -1;
  }

  glfwSetKeyCallback(window, glfwKeyCallback);
  glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);

  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << "\n";

  {
    std::shared_ptr<RenderEngine::ShaderProgram> shader = ResourceManager::loadShaders("DefaultShader", "res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
    std::shared_ptr<RenderEngine::Texture2D> texture = ResourceManager::loadTexture("DefaultTexture", "res/textures/rol506_logo.jpg");

    RenderEngine::VertexArray VAO;
    RenderEngine::VertexBuffer VBO;
    VBO.init(vertices, sizeof(vertices)); //already bound

    RenderEngine::VertexBufferLayout layout;
    layout.reserve(2);
    layout.addElementLayoutFloat(3, false);
    layout.addElementLayoutFloat(2, false);

    VAO.addBuffer(VBO, layout); //already bound
    
    RenderEngine::IndexBuffer EBO;
    EBO.init(indices, 6); //already bound

    shader->use();
    shader->setInt(0, "tex");
    glActiveTexture(GL_TEXTURE0);
    texture->bind();

    glClearColor(66.f/255.f, 170.f/255.f, 255.f/255.f, 1.0f);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
      /* Render here */
      glClear(GL_COLOR_BUFFER_BIT);

      shader->use();
      texture->bind();
      VAO.bind();
      glDrawElements(GL_TRIANGLES, EBO.getCount(), GL_UNSIGNED_INT, nullptr);

      /* Swap front and back buffers */
      glfwSwapBuffers(window);

      /* Poll for and process events */
      glfwPollEvents();
    }
  }

  ResourceManager::unloadAllResources();
  glfwTerminate();

  std::cout << "Exited!" << std::endl;

  return 0;
}
