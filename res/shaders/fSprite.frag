#version 430 core

in vec2 texPos;

uniform sampler2D tex;
uniform bool outline;

out vec4 FragColor;

void main()
{
  FragColor = texture(tex, texPos);
}
