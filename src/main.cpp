#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define APIENTRY __stdcall
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <chrono>

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
#include "Renderer/Text.h"
#include "Physics/PhysicsEngine.h"

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::ivec2 gWindowSize(640, 480);
glm::mat4 gProjection(1.0f);

float playerSpeed = 1.f;

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

void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "\n[OpenGL debug message] (" << id << "): " <<  message << "\n";

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << "\n";

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << "\n";
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << "\n";
    std::cout << "[OpenGL debug message]\n";
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

  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, false);
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

  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
  {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    std::cout << "OpenGL debug context loaded!\n"; 
  }

  //glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  {
    //preload all resources
    std::cout << "Loading resources...\n";
    ResourceManager::loadCharset("res/fonts/AovelSansRounded-rdDL.ttf");
    auto shader = ResourceManager::loadShaders("SpriteShader", "res/shaders/vSprite.vert", "res/shaders/fSprite.frag");
    auto textShader = ResourceManager::loadShaders("TextShader", "res/shaders/vText.vert", "res/shaders/fText.frag");
    auto texture = ResourceManager::loadTexture("SpriteTexture", "res/textures/rol506_logo.jpg");
    auto sprite = ResourceManager::loadSprite("Sprite", "SpriteShader", "SpriteTexture");
    auto sprite2 = ResourceManager::loadSprite("Sprite2", "SpriteShader", "SpriteTexture");
    std::cout << "Resources are loaded!\n";

    gProjection = glm::ortho(0.0f, static_cast<float>(gWindowSize.x), 0.0f, static_cast<float>(gWindowSize.y), -100.f, 100.f);
    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, -3.0f, 0.0f));

    sprite->setScale(glm::vec2(50));
    sprite->setPosition(glm::vec2(0.5f * SCR_COORD_TO_WORLD_X, 0.5f * SCR_COORD_TO_WORLD_Y));
    sprite2->setScale(glm::vec2(50));
    sprite2->setPosition(glm::vec2(0.2f * SCR_COORD_TO_WORLD_X, 0.2f * SCR_COORD_TO_WORLD_Y));
    sprite2->isStatic = true;

    RenderEngine::Text fpsCounter("0 FPS", glm::vec2(0.0f, 0.0f), 0.1f, glm::vec3(0.0f, 0.0f, 0.0f), textShader);
    fpsCounter.setPosition(glm::vec2(5, 500 - fpsCounter.getHeight() * 1.5f));
    fpsCounter.setBackground(0.0f, 1.0f, 0.0f, 0.5f);

    RenderEngine::Text positionText("000.0, 000.0", glm::vec2(0.0f, 0.0f), 0.1f, glm::vec3(0.0f), textShader);
    positionText.setPosition(glm::vec2(0.0f, 500 - fpsCounter.getHeight() * 3.f));

    PhysicsEngine::init();
    PhysicsEngine::addDynamicObject(sprite);
    PhysicsEngine::addDynamicObject(sprite2); 

    int fps = 0;
    auto timeStart = std::chrono::high_resolution_clock::now();
    auto timeEnd = std::chrono::high_resolution_clock::now();
    double deltaTime = 0.f;
    double FPSTimer = 0.f;

    textShader->use();
    textShader->setMat4(gProjection, "projectionMatrix");
    textShader->setMat4(view, "viewMatrix");

    shader->use();
    shader->setMat4(gProjection, "projectionMatrix");
    shader->setMat4(view, "viewMatrix");
    RenderEngine::Renderer::setDepthTest(true);
    RenderEngine::Renderer::setClearColor(66.f/255.f, 170.f/255.f, 255.f/255.f, 1.0f);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
      RenderEngine::Renderer::clear();

      //TIMING
      timeStart = std::chrono::high_resolution_clock::now();
      deltaTime = std::chrono::duration<double, std::milli>(timeStart - timeEnd).count();
      FPSTimer += deltaTime;
      fps += 1;

      if (FPSTimer >= 1000.f)
      {
        fpsCounter.setText(std::to_string(fps) + " FPS");
        fps = 0;
        FPSTimer = 0.f;
      }

      fpsCounter.render();

      //convert mouse coords
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      xpos /= gWindowSize.x;
      ypos /= gWindowSize.y;
      ypos -= 1.0f;
      ypos *= -1.f;

      //PHYSICS

      PhysicsEngine::update(deltaTime);

      //RENDER

      sprite->render(0);
      sprite2->render(0);
      positionText.setText(std::to_string(sprite->velocity.x) + " \n" + std::to_string(sprite->velocity.y));
      positionText.render();

      //INPUT

      sprite2->setPosition(glm::vec2(xpos * SCR_COORD_TO_WORLD_X, ypos * SCR_COORD_TO_WORLD_Y) -
        glm::vec2(sprite2->getScale().x * SPRITE_SCALE_TO_WORLD_X/2.f, sprite2->getScale().y * SPRITE_SCALE_TO_WORLD_Y/2.f));

      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      {
        //sprite->setTargetPosition(sprite->getTargetPosition() + glm::vec2(0.0f, 1.0f) * playerSpeed * static_cast<float>(deltaTime));
      }
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      {
        //sprite->setTargetPosition(sprite->getTargetPosition() + glm::vec2(0.0f, -1.0f) * playerSpeed * static_cast<float>(deltaTime));
      }
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      {
        //sprite->setTargetPosition(sprite->getTargetPosition() + glm::vec2(-1.0f, 0.0f) * playerSpeed * static_cast<float>(deltaTime));
        sprite->velocity -= glm::vec2(1.0f, 0.0f) * playerSpeed;
      }
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      {
        //sprite->setTargetPosition(sprite->getTargetPosition() + glm::vec2(1.0f, 0.0f) * playerSpeed * static_cast<float>(deltaTime));
        sprite->velocity += glm::vec2(1.0f, 0.0f) * playerSpeed;
      }
      if (sprite->isGrounded && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      {
        sprite->velocity.y += 15.f;
      }

      //TIMING
      timeEnd = std::chrono::high_resolution_clock::now();

      /* Swap front and back buffers */
      glfwSwapBuffers(window);

      /* Poll for and process events */
      glfwPollEvents();
    }
  }

  PhysicsEngine::shutdown();
  ResourceManager::unloadAllResources();
  glfwTerminate();

  std::cout << "Exited!" << std::endl;

  return 0;
}
