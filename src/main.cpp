#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define APIENTRY __stdcall
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

//get path
#ifdef __linux__
#include <unistd.h>
#elif _WIN32
//#include <windows.h>
#endif

#include "Resources/ResourceManager.h"
#include "Renderer/Renderer.h"
#include "System/Socket.h"
#include "Renderer/Sprite2D.h"

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::ivec2 gWindowSize(640, 480);
glm::mat4 gProjection(1.0f);

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

  gProjection = glm::ortho(0.0f, static_cast<float>(gWindowSize.x), 0.0f, static_cast<float>(gWindowSize.y), 0.1f, 100.f);

  glViewport(0, 0, width, height);
}

int main(int argc, const char** argv)
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


  if (argc > 1 && std::string(argv[1]) == "-send"){

    System::Socket socket(true);
    std::string host; int port;
    std::cout << "Enter remote host: ";
    std::cin >> host;
    std::cout << "Enter remote host port: ";
    std::cin >> port;
    if (socket.connect(host, port))
    {
      char message[2048];
      std::cout << "Enter a message (2048 chars max): ";
      std::cin.get();
      std::cin.getline(message, 2048);
      socket.sendMessage(message);
    }
  }

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

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glfwSetKeyCallback(window, glfwKeyCallback);
  glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);

  std::cout << "OpenGL version: " << RenderEngine::Renderer::getVersionStr() << "\n";
  std::cout << "OpenGL renderer: " << RenderEngine::Renderer::getRendererStr() << "\n";

  {
    auto shader = ResourceManager::loadShaders("SpriteShader", "res/shaders/vSprite.vert", "res/shaders/fSprite.frag");
    auto texture = ResourceManager::loadTexture("SpriteTexture", "res/textures/rol506_logo.jpg");
    auto sprite = ResourceManager::loadSprite("Sprite", "SpriteShader", "SpriteTexture");

    gProjection = glm::ortho(0.0f, static_cast<float>(gWindowSize.x), 0.0f, static_cast<float>(gWindowSize.y), -100.f, 100.f);
    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, -3.0f, 0.0f));

    sprite->setPosition(glm::vec2(gWindowSize.x/2.f, gWindowSize.y/2.f));
    sprite->setRotation(45.f);

    shader->use();
    shader->setMat4(gProjection, "projectionMatrix");
    shader->setMat4(view, "viewMatrix");
    RenderEngine::Renderer::setDepthTest(true);
    RenderEngine::Renderer::setClearColor(66.f/255.f, 170.f/255.f, 255.f/255.f, 1.0f);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
      /* Render here */
      RenderEngine::Renderer::clear();

      sprite->render(1);

      if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
      {
        sprite->setScale(glm::vec2(sprite->getScale().x + 1, sprite->getScale().y + 1));
      }
      if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
      {
        sprite->setScale(glm::vec2(sprite->getScale().x - 1, sprite->getScale().y - 1));
      }
      if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
      {
        sprite->setRotation(sprite->getRotation() + 1);
      }

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
