#version 430 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

void main()
{
  texCoord = aTexCoord;
  gl_Position = vec4(vertexPosition, 1.0);
}
