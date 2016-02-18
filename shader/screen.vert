#version 330 core

in vec2 vertPos;

out vec2 fragPos;

void main() {
	gl_Position = vec4(vertPos, 0.0, 1.0);
	fragPos = vertPos;
}
