#version 430 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec3 textColor;
uniform vec4 backgroundColor;

void main()
{
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
  FragColor = vec4(textColor, 1.0) * sampled;
  if (FragColor.a <= 0.0)
  {
    //FragColor = backgroundColor;
    discard;
  }
}
