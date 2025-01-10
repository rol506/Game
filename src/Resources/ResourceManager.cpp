#include "ResourceManager.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>

#include <glad/glad.h>

#include "../Renderer/ShaderProgram.h"
#include "../Renderer/Texture2D.h"
#include "../Renderer/Sprite2D.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_PNM
#include "stb_image.h"

#include <ft2build.h>
#include FT_FREETYPE_H

std::string ResourceManager::m_path;
ResourceManager::ShaderProgramsMap ResourceManager::m_shaderPrograms;
ResourceManager::TexturesMap ResourceManager::m_textures;
ResourceManager::SpritesMap ResourceManager::m_sprites;
std::unordered_map<char, Character> ResourceManager::m_charset;

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
  m_sprites.clear();
  m_charset.clear();
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

std::shared_ptr<RenderEngine::Sprite2D> ResourceManager::loadSprite(const std::string& spriteName, const std::string& shaderName, const std::string textureName, const std::string& subTextureName)
{
  auto texture = getTexture(textureName);
  if (!texture)
  {
    std::cout << "Can't find the texture: " << textureName << " for the sprite: " << spriteName << "\n";
    return nullptr;
  }

  auto shader = getShaderProgram(shaderName);
  if (!shader)
  {
    std::cout << "Can't find the shader program: " << shaderName << " for the sprite: " << spriteName << "\n";
    return nullptr;
  }

  std::shared_ptr<RenderEngine::Sprite2D> newSprite = m_sprites.emplace(spriteName,
      std::make_shared<RenderEngine::Sprite2D>(texture, subTextureName, shader)).first->second;;
  return newSprite;
}

std::shared_ptr<RenderEngine::Sprite2D> ResourceManager::getSprite(const std::string& spriteName)
{
  auto it = m_sprites.find(spriteName);
  if (it != m_sprites.end())
  {
    return it->second;
  }

  std::cout << "Can't find the sprite: " << spriteName << "\n";
  return nullptr;
}

std::unordered_map<char, Character>* ResourceManager::loadCharset(const std::string& pathToFont)
{
  FT_Library ft;
  if (FT_Init_FreeType(&ft))
  {
    std::cerr << "Could not init FreeType library!\n";
    return nullptr;
  }

  FT_Face face;
  if (FT_New_Face(ft, std::string(m_path + "/" + pathToFont).c_str(), 0, &face))
  {
    std::cerr << "Failed to load font: " << pathToFont << "\n";
  } else {
    FT_Set_Pixel_Sizes(face, 0, 128);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned int c=0;c<127+1;++c)
    {
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      {
        std::cout << "Failed to load Glyph: " << c << "\n";
        continue;
      }

      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                   face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                   face->glyph->bitmap.buffer);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      Character character = {texture, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                             face->glyph->bitmap_left, face->glyph->bitmap_top,
                             static_cast<unsigned int>(face->glyph->advance.x)};
      m_charset.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);
  return &m_charset;
}

std::unordered_map<char, Character>* ResourceManager::getCharset()
{
  return &m_charset;
}

