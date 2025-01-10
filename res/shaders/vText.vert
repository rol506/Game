#version 430 core

layout (location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
  TexCoords = vertex.zw;
  gl_Position = projectionMatrix * viewMatrix * vec4(vertex.xy, 1.0, 1.0);
}
