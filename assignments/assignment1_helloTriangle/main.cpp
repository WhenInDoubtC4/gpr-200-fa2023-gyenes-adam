#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

constexpr int TRIANGLE_VERT_COUNT = 3;
constexpr int TRIANGLE_VERT_DATA_STRIDE = 7;

constexpr float TRIANGLE_VERT_DATA[TRIANGLE_VERT_COUNT * TRIANGLE_VERT_DATA_STRIDE] = {
	//X     Y     Z			R     G     B     A
	-0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f, 1.f,	//Bottom left
	 0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f, 1.f,	//Bottom right
	 0.0f,  0.5f, 0.0f,		0.0f, 0.0f, 1.0f, 1.f,	//Top center
};

constexpr char SHADER_TIME_UNIFORM_NAME[] = "_Time";

constexpr char VERT_SHADER_SOURCE[] = R"(
#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec4 vColor;
uniform float _Time;

out vec4 Color;

void main() 
{
	Color = vColor;
	vec3 offset = vec3(0, sin(vPos.x + _Time), 0) * 0.5;
	gl_Position = vec4(vPos + offset, 1.0);
}
)";

constexpr char FRAG_SHADER_SOURCE[] = R"(
#version 450

in vec4 Color;
uniform float _Time;

out vec4 FragColor;

void main() 
{
	FragColor = Color * abs(sin(_Time));
}
)";

GLuint createVAO(const float* vertexData, const int numVertices)
{
	//Setup vertex buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * TRIANGLE_VERT_DATA_STRIDE * numVertices, vertexData, GL_STATIC_DRAW);

	//Setup vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//Position attribute
	constexpr int positionAttributeIndex = 0;
	glVertexAttribPointer(positionAttributeIndex, 3, GL_FLOAT, GL_FALSE, sizeof(float) * TRIANGLE_VERT_DATA_STRIDE, nullptr);
	glEnableVertexAttribArray(positionAttributeIndex);

	//Vertex color attribute
	constexpr int vertexColorAttributeIndex = 1;
	auto vertexColorAttributeByteOffset = reinterpret_cast<const void*>(sizeof(float) * 3);
	glVertexAttribPointer(vertexColorAttributeIndex, 4, GL_FLOAT, GL_FALSE, sizeof(float) * TRIANGLE_VERT_DATA_STRIDE, vertexColorAttributeByteOffset);
	glEnableVertexAttribArray(vertexColorAttributeIndex);

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

	GLuint shaderProgram = createShaderProgram(VERT_SHADER_SOURCE, FRAG_SHADER_SOURCE);
	GLuint triangleVAO = createVAO(TRIANGLE_VERT_DATA, TRIANGLE_VERT_COUNT);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat time = glfwGetTime();
		GLint timeUniformLocation = glGetUniformLocation(shaderProgram, SHADER_TIME_UNIFORM_NAME);
		glUniform1f(timeUniformLocation, time);

		glUseProgram(shaderProgram);
		glBindVertexArray(triangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}