#version 430 core

layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec2 aTexPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float layer;

out vec2 texPos;

void main()
{
  texPos = aTexPos;

  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, layer, 1.0);
}
