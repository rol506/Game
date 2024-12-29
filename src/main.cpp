#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <unistd.h>

#include "Renderer/ShaderProgram.h"

int gWindowSizeX = 640;
int gWindowSizeY = 480;

static GLfloat vertices[] = {
  //X      Y     Z
  -0.5f, -0.5f, 0.0f,
   0.0f,  0.5f, 0.0f,
   0.5f, -0.5f, 0.0f
};

static const char vertexSource[] = {
  "#version 430 core\n"
  "layout (location=0) in vec3 vertexPosition;"
  "void main() {"
  "gl_Position = vec4(vertexPosition, 1.0);"
  "}"
};

static const char fragmentSource[] = {
  "#version 430 core\n"
  "out vec4 FragColor;"
  "void main() {"
  "FragColor = vec4(0.0, 1.0, 0.0, 1.0);"
  "}"
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
  gWindowSizeX = width;
  gWindowSizeY = height;
  glViewport(0, 0, width, height);
}

int main(void)
{
  GLFWwindow* window;

  char* path = (char*)malloc(1024);
  ssize_t r;
  r = readlink("/proc/self/exe", path, 1023);
  std::string executablePath = path;
  delete path;
  std::cout << "Executable path: " << executablePath << "\n";

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
  window = glfwCreateWindow(640, 480, "Game by rol506", NULL, NULL);
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

    /*GLuint vertex, fragment, shader;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, vertexSource, nullptr);
    glCompileShader(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, fragmentSource, nullptr);
    glCompileShader(fragment);

    shader = glCreateProgram();
    glAttachShader(shader, vertex);
    glAttachShader(shader, fragment);
    glLinkProgram(shader);*/

    RenderEngine::ShaderProgram shader = RenderEngine::ShaderProgram(vertexSource, fragmentSource);

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glClearColor(66.f/255.f, 170.f/255.f, 255.f/255.f, 1.0f);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
      /* Render here */
      glClear(GL_COLOR_BUFFER_BIT);

      shader.use();
      glDrawArrays(GL_TRIANGLES, 0, 3);

      /* Swap front and back buffers */
      glfwSwapBuffers(window);

      /* Poll for and process events */
      glfwPollEvents();
    }
  }

  glfwTerminate();

  std::cout << "Exited!" << std::endl;

  return 0;
}
