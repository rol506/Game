#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace RenderEngine
{
  class ShaderProgram;
  class Texture2D;
  class Sprite2D;
}

struct Character
{
  unsigned int textureID;
  unsigned int sizeX, sizeY;
  int bearingX, bearingY;
  unsigned int advance;
};

class ResourceManager
{
public:

  ResourceManager() = delete;

  static void setExecutablePath(const std::string& executablePath);
  static void unloadAllResources();

  static std::shared_ptr<RenderEngine::ShaderProgram> loadShaders(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath);
  static std::shared_ptr<RenderEngine::ShaderProgram> getShaderProgram(const std::string& shaderName);

  static std::shared_ptr<RenderEngine::Texture2D> loadTexture(const std::string& textureName, const std::string& texturePath);
  static std::shared_ptr<RenderEngine::Texture2D> getTexture(const std::string& textureName);
  static std::shared_ptr<RenderEngine::Texture2D> loadTextureAtlas(const std::string& textureName, const std::string& texturePath,
                                                                   const unsigned int width, const unsigned int height, const std::vector<std::string>& subTexturesNames);

  static std::shared_ptr<RenderEngine::Sprite2D> loadSprite(const std::string& spriteName, const std::string& shaderName, const std::string textureName, const std::string& subTextureName="DEFAULT");
  static std::shared_ptr<RenderEngine::Sprite2D> getSprite(const std::string& spriteName);

  static std::unordered_map<char, Character>* loadCharset(const std::string& pathToFont);
  static std::unordered_map<char, Character>* getCharset();

private:

  static std::string getFileString(const std::string& relativeFilePath);

  static std::string m_path;
  static std::unordered_map<char, Character> m_charset;

  typedef std::unordered_map<std::string, std::shared_ptr<RenderEngine::ShaderProgram>> ShaderProgramsMap;
  static ShaderProgramsMap m_shaderPrograms;

  typedef std::unordered_map<std::string, std::shared_ptr<RenderEngine::Texture2D>> TexturesMap;
  static TexturesMap m_textures;

  typedef std::unordered_map<std::string, std::shared_ptr<RenderEngine::Sprite2D>> SpritesMap;
  static SpritesMap m_sprites;

};
