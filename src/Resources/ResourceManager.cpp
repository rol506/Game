#include "ResourceManager.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>

#include "../Renderer/ShaderProgram.h"
#include "../Renderer/Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_PNM
#include "stb_image.h"

std::string ResourceManager::m_path;
ResourceManager::ShaderProgramsMap ResourceManager::m_shaderPrograms;
ResourceManager::TexturesMap ResourceManager::m_textures;

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
  m_textures.clear();
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

std::shared_ptr<RenderEngine::Texture2D> ResourceManager::loadTexture(const std::string& textureName, const std::string& texturePath)
{
  int channels = 0;
  int width = 0;
  int height = 0;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* pixels = stbi_load(std::string(m_path + "/" + texturePath).c_str(), &width, &height, &channels, 0);

  if (!pixels)
  {
    std::cerr << "Can't load image: " << texturePath << "\n";
    return nullptr;
  }

  std::shared_ptr<RenderEngine::Texture2D> newTexture = m_textures.emplace(textureName, std::make_shared<RenderEngine::Texture2D>(width, height, pixels, channels,
        GL_LINEAR, GL_CLAMP_TO_EDGE)).first->second;

  stbi_image_free(pixels);
  return newTexture;
}

std::shared_ptr<RenderEngine::Texture2D> ResourceManager::getTexture(const std::string& textureName)
{
  auto it = m_textures.find(textureName);
  if (it != m_textures.end())
  {
    return it->second;
  }
  std::cerr << "Can't find the texture " << textureName << "\n";
  return nullptr;
}

std::shared_ptr<RenderEngine::Texture2D> ResourceManager::loadTextureAtlas(const std::string& textureName, const std::string& texturePath,
                                                                   const unsigned int width, const unsigned int height, const std::vector<std::string>& subTexturesNames)
{
  auto texture = loadTexture(std::move(textureName), std::move(texturePath));
  if (texture)
  {
    const unsigned int textureWidth = texture->width();
    const unsigned int textureHeight = texture->height();
    unsigned int currentTextureOffsetX = 0;
    unsigned int currentTextureOffsetY = textureHeight;

    for (auto& currentSubTextureName : subTexturesNames)
    {
      glm::vec2 leftBottomUV(static_cast<float>(currentTextureOffsetX)/textureWidth, static_cast<float>(currentTextureOffsetY - height)/textureHeight);
      glm::vec2 rightTopUV(static_cast<float>(currentTextureOffsetX + width)/textureWidth, static_cast<float>(currentTextureOffsetY)/textureHeight);
      texture->addSubTexture(std::move(currentSubTextureName), leftBottomUV, rightTopUV);

      currentTextureOffsetX += width;
      if (currentTextureOffsetX >= textureWidth)
      {
        currentTextureOffsetX = 0;
        currentTextureOffsetX -= height;
      }
    }
  }
  return texture;
}

