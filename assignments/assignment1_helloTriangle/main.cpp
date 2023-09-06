#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

float triangleVerts[3 * 3] = {
	-0.5f, -0.5f, 0.0f,	//Bottom left
	 0.5f, -0.5f, 0.0f,	//Bottom right
	 0.0f,  0.5f, 0.0f,	//Top center
};

const char vertShaderSource[] = R"(
#version 450
layout(location = 0) in vec3 vPos;
void main() {
	gl_Position = vec4(vPos,1.0);
}
)";

const char fragShaderSource[] = R"(
#version 450
out vec4 FragColor;
void main() {
	FragColor = vec4(1.0);
}
)";

GLuint createVAO(float* vertexData, int numVertices)
{
	//Setup vertex buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVertices, vertexData, GL_STATIC_DRAW);

	//Setup vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
	glEnableVertexAttribArray(0);

	return vao;
}

GLuint createShader(GLenum shaderType, const char* shaderSource)
{
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, 0);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char log[512];
		glGetShaderInfoLog(shader, 512, NULL, log);
		printf("Failed to compile shader: %s", log);
	}

	return shader;
}

GLuint createShaderProgram(const char* vertShaderSource, const char* fragShaderSource)
{
	GLuint shaderProgram = glCreateProgram();

	GLuint vertShader = createShader(GL_VERTEX_SHADER, vertShaderSource);
	GLuint fragShader = createShader(GL_FRAGMENT_SHADER, fragShaderSource);

	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);

	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Failed to link shader program: %s", infoLog);
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return shaderProgram;
}

int main() {
	printf("Initializing...");
	if (!glfwInit()) 
	{
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (window == NULL) 
	{
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) 
	{
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	GLuint shaderProgram = createShaderProgram(vertShaderSource, fragShaderSource);
	GLuint triangleVAO = createVAO(triangleVerts, 3);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(triangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}