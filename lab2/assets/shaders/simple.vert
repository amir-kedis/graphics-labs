#version 330

out vec4 vertex_color;

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform float time;

void main() {
  vertex_color = color;
  gl_Position = vec4(position, 1.0);//vec4(position + vec3(sin(time), 0.0, 0.0), 1.0);
}