#include "ResourceManager.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <regex.h>
#include <sstream>

#include "../Renderer/ShaderProgram.h"

std::string ResourceManager::m_path;
ResourceManager::ShaderProgramsMap ResourceManager::m_shaderPrograms;

std::string ResourceManager::getFileString(const std::string& relativeFilePath)
{
  std::ifstream f;
  f.open(m_path + "/" + relativeFilePath, std::ios::in | std::ios::binary);
  if (!f.is_open())
  {
    std::cout << "Can't open file: " << relativeFilePath << "\n";
    return std::string{};
  }

  std::stringstream buf;
  buf << f.rdbuf();
  return buf.str();
}

void ResourceManager::setExecutablePath(const std::string& executablePath)
{
  std::size_t found = executablePath.find_last_of("/\\");
  m_path = executablePath.substr(0, found);
}

void ResourceManager::unloadAllResources()
{
  //CLEAR ALL RESOURCES
  m_shaderPrograms.clear();
}

std::shared_ptr<RenderEngine::ShaderProgram> ResourceManager::loadShaders(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath)
{
  std::string vertex = getFileString(vertexPath);
  if (vertex.empty())
  {
    std::cout << "No vertex shader!\n";
    return nullptr;
  }

  std::string fragment = getFileString(fragmentPath);
  if (fragment.empty())
  {
    std::cout << "No fragment shader!\n";
    return nullptr;
  }

  std::shared_ptr<RenderEngine::ShaderProgram> newShader = m_shaderPrograms.emplace(shaderName, std::make_shared<RenderEngine::ShaderProgram>(vertex, fragment))
    .first->second;

  if (newShader->isCompiled())
  {
    return newShader;
  }

  std::cout << "Failed to load shader program: \n    Vertex: " << vertexPath << "\n    Fragment: " << fragmentPath << "\n";
  return nullptr;
}

std::shared_ptr<RenderEngine::ShaderProgram> ResourceManager::getShaderProgram(const std::string& shaderName)
{
  auto it = m_shaderPrograms.find(shaderName);
  if (it != m_shaderPrograms.end())
  {
    return it->second;
  }

  std::cout << "Can't find the shader program " << shaderName << "\n";
  return nullptr;
}
