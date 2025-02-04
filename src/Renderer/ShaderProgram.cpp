#include "ShaderProgram.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace RenderEngine
{
  ShaderProgram::ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource)
  {
    GLuint vertex;
    if (!createShader(vertex, vertexSource, GL_VERTEX_SHADER))
    {
      std::cerr << "Failed to create vertex shader!\n";
      return;
    }

    GLuint fragment;
    if (!createShader(fragment, fragmentSource, GL_FRAGMENT_SHADER))
    {
      glDeleteShader(vertex);
      std::cerr << "Failed to create fragment shader!\n";
      return;
    }

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    glLinkProgram(m_ID);

    int success;
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success)
    {
      GLchar infoLog[1024];
      glGetProgramInfoLog(m_ID, 1024, nullptr, infoLog);
      std::cout << "Failed to create shader program: " << infoLog << "\n";
    } else {
      m_isCompiled = true;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }
  
  ShaderProgram::~ShaderProgram()
  {
    glDeleteProgram(m_ID);
  }

  bool ShaderProgram::createShader(GLuint& shaderID, const std::string& shaderSource, const GLenum shaderType)
  {
    shaderID = glCreateShader(shaderType);
    const char* code = shaderSource.c_str();
    glShaderSource(shaderID, 1, &code, nullptr);

    glCompileShader(shaderID);

    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

    if (!success)
    {
      GLchar infoLog[1024];
      glGetShaderInfoLog(shaderID, 1024, nullptr, infoLog);
      std::cerr << "Failed to create shader: " << infoLog << "\n";
      return false;
    }

    return true;
  }
  
  void ShaderProgram::use() const
  {
    glUseProgram(m_ID);
  }

  ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shader) noexcept
  {
    if (this != &shader)
    {
      glDeleteProgram(m_ID);
      m_ID = shader.m_ID;
      m_isCompiled = shader.m_isCompiled;

      shader.m_ID = 0;
      shader.m_isCompiled = false;
    }

    return *this;
  }

  ShaderProgram::ShaderProgram(ShaderProgram&& shader) noexcept
  {
    m_ID = shader.m_ID;
    m_isCompiled = shader.m_isCompiled;

    shader.m_ID = 0;
    shader.m_isCompiled = false; 
  }

  void ShaderProgram::setInt(const int value, const std::string& name)
  {
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
  }

  void ShaderProgram::setFloat(const float value, const std::string& name)
  {
    glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
  }

  void ShaderProgram::setMat4(const glm::mat4& matrix, const std::string& name)
  {
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
  }

  void ShaderProgram::setVec3(const glm::vec3& vector, const std::string& name)
  {
    glUniform3f(glGetUniformLocation(m_ID, name.c_str()), vector.x, vector.y, vector.z);
  }

  void ShaderProgram::setVec4(const glm::vec4& vector, const std::string& name)
  { 
    glUniform4f(glGetUniformLocation(m_ID, name.c_str()), vector.x, vector.y, vector.z, vector.w);
  }
}
