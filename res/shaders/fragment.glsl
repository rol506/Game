#version 430 core

uniform sampler2D tex;

in vec2 texCoord;

out vec4 FragColor;

void main()
{
  //FragColor = vec4(0.0, 1.0, 0.0, 1.0);
  FragColor = texture(tex, texCoord);
}
