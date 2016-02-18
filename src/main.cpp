#include <atomic>
#include <cstdlib>
#include <iostream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.h"
#include "utils.h"

using glm::dvec2;
using glm::inverse;
using glm::lookAt;
using glm::mat4;
using glm::pow;
using glm::value_ptr;
using glm::vec2;
using glm::vec3;
using std::cerr;
using std::endl;

static dvec2 viewScale(double);
static void keyCallback(GLFWwindow *, int, int, int, int);
static void scrollCallback(GLFWwindow *, double, double);
static void resizeCallback(GLFWwindow *, int, int);


#define ATTRIB_VERTPOS 0

static GLfloat vertices[] = {
	-1.0f,  1.0f,
	-1.0f, -1.0f,
	 1.0f,  1.0f,
	 1.0f, -1.0f
};


double autoZoom = 0.0;
std::atomic<double> scrollOffset(0.0);
int width = 800, height = 600;

int main(int argc, char *argv[])
{
	// initialize GLFW
	if (!glfwInit()) {
		cerr << "Could not initialize GLFW." << endl;
		return EXIT_FAILURE;
	}
	// set some options for glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// create window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(
				width, height, "Mandelbrot",
				nullptr, nullptr);
	if (window == nullptr) {
		cerr << "Could not create window." << endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}
	// make the context current
	glfwMakeContextCurrent(window);
	// link OpenGL library
	glewExperimental = GL_TRUE;
	GLenum ret = glewInit();
	if (ret != GLEW_OK) {
		cerr << "Could not initialize GLEW: "
			 << glewGetErrorString(ret) << endl;
		return EXIT_FAILURE;
	}

	// check if shader support double-precision
	bool singlePrecision = false;
	if (glfwExtensionSupported("GL_ARB_gpu_shader_fp64") != GL_TRUE) {
		cerr << "Double precision is not supported. Single precision is used." << endl;
		singlePrecision = true;
	}

	// initialize resources you need
	// --- shaders
	GLuint vert = createShader("Vertex Shader", shader::screen_vert, GL_VERTEX_SHADER);

	GLuint frag;
	if (singlePrecision)
		frag = createShader("Fragment Shader (float)", shader::mb_32_frag, GL_FRAGMENT_SHADER);
	else
		frag = createShader("Fragment Shader (double)", shader::mb_64_frag, GL_FRAGMENT_SHADER);


	GLuint program = createProgram({vert, frag});
	glBindAttribLocation(program, ATTRIB_VERTPOS, "vertPos");
	linkProgram(program);
	validateProgram(program);
	glDeleteShader(vert);
	glDeleteShader(frag);

	// --- vertex buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// --- vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(ATTRIB_VERTPOS);
	glVertexAttribPointer(ATTRIB_VERTPOS, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// register callbacks
	glfwSetKeyCallback(window, &keyCallback);
	glfwSetScrollCallback(window, &scrollCallback);
	glfwSetWindowSizeCallback(window, &resizeCallback);

	// get uniform locations
	GLint uni_pos = glGetUniformLocation(program, "pos");
	GLint uni_scale = glGetUniformLocation(program, "scale");
	GLint uni_ratio = glGetUniformLocation(program, "ratio");

	// some other variables
	dvec2 pos = dvec2(0.0, 0.0);
	double zoom = 1.0;

	double lastTime = glfwGetTime();
	dvec2 lastMouse;
	glfwGetCursorPos(window, &lastMouse.x, &lastMouse.y);

	// repeat this loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// draw mandelbrot
		int fwidth, fheight;
		glfwGetFramebufferSize(window, &fwidth, &fheight);
		glViewport(0, 0, fwidth, fheight);

		glUseProgram(program);
		glBindVertexArray(vao);
		if (singlePrecision) {
			glUniform2fv(uni_pos, 1, glm::value_ptr(vec2(pos)));
			glUniform2fv(uni_scale, 1, glm::value_ptr(vec2(viewScale(zoom))));
		} else {
			glUniform2dv(uni_pos, 1, glm::value_ptr(pos));
			glUniform2dv(uni_scale, 1, glm::value_ptr(viewScale(zoom)));
		}
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// poll events
		glfwPollEvents();

		// update
		// --- get delta time
		double dt = lastTime;
		lastTime = glfwGetTime();
		dt = lastTime - dt;
		// --- get mouse movements
		dvec2 dmouse = lastMouse;
		glfwGetCursorPos(window, &lastMouse.x, &lastMouse.y);
		dmouse = lastMouse - dmouse;
		// --- update position
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			pos += 2.0 * viewScale(zoom) * dvec2(
					-dmouse.x / width, dmouse.y / height);
		} else {
			dvec2 movement;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS
					|| glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
				movement.y += 1.0;
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS
					|| glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
				movement.y -= 1.0;
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS
					|| glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
				movement.x -= 1.0;
			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS
					|| glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				movement.x += 1.0;
			}
			pos += dt * viewScale(zoom) * movement;
		}
		// --- update zoom
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			autoZoom += 2.0 * dt;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			autoZoom -= 2.0 * dt;
		}
		zoom += 0.3 * dt * autoZoom;

		dvec2 rmouse(lastMouse.x / width, 1.0 - lastMouse.y / height);
		rmouse = 2.0 * rmouse - 1.0;
		double newZoom = zoom + 0.3 * scrollOffset.exchange(0.0);
		pos += rmouse * viewScale(zoom) - rmouse * viewScale(newZoom);
		zoom = newZoom;

		// swap front and back buffers
		glfwSwapBuffers(window);
	}

	// clean resources of OpenGL
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(program);

	// exit program
	glfwTerminate();
	return EXIT_SUCCESS;
}

dvec2 viewScale(double zoom) {
	double scale = 2.4 * pow(2, -zoom);
	return dvec2(((double) width / height) * scale, scale);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		autoZoom = !autoZoom;
	}
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	scrollOffset = scrollOffset + yoffset;
}

void resizeCallback(GLFWwindow *window, int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight;
}
