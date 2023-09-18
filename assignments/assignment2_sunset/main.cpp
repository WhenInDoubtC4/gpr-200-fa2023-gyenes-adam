#include <util/Shader.h>

#include <stdio.h>
#include <math.h>

#include <ew/ewMath/ewMath.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

GLuint createVAO(const float* vertexData, int numVertices, const unsigned int* indiciesData, int numIndicies);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

constexpr int VERT_DATA_STRIDE = 3;
constexpr int VERT_COUNT = 6;

constexpr float VERT_DATA[VERT_COUNT * VERT_DATA_STRIDE] = {
	//X    //Y    //Z
	-0.8f, -0.8f, 0.f, //Bottom left
	0.8f, -0.8f, 0.f, //Bottom right
	0.8f, 0.8f, 0.f, //Top right
	-0.8f, 0.8f, 0.f //Top left
};

constexpr unsigned int VERT_INDICIES[VERT_COUNT] = {
	0 /*Bottom left*/, 1 /*Bottom right*/, 2 /*Top right*/,
	2 /*Top right*/, 3 /*Top left*/, 0 /*Bottom left*/,
};

constexpr char VERT_SHADER_FILEPATH[] = "assets\\vertexShader.vert";
constexpr char FRAG_SHADER_FILEPATH[] = "assets\\fragmentShader.frag";

float triangleColor[3] = { 1.0f, 0.5f, 0.0f };
float triangleBrightness = 1.0f;
bool showImGUIDemoWindow = true;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	Shader shader(VERT_SHADER_FILEPATH, FRAG_SHADER_FILEPATH);
	GLuint vao = createVAO(VERT_DATA, VERT_COUNT, VERT_INDICIES, VERT_COUNT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	shader.exec();
	glBindVertexArray(vao);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		shader.setVec3("_Color", triangleColor[0], triangleColor[1], triangleColor[2]);
		shader.setFloat("_Brightness", triangleBrightness);

		//glDrawArrays(GL_TRIANGLES, 0, VERT_COUNT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
			ImGui::ColorEdit3("Color", triangleColor);
			ImGui::SliderFloat("Brightness", &triangleBrightness, 0.0f, 1.0f);
			ImGui::End();
			if (showImGUIDemoWindow) {
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

GLuint createVAO(const float* vertexData, int numVertices, const unsigned int* indiciesData, int numIndicies) 
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Define a new buffer id
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * VERT_DATA_STRIDE, vertexData, GL_STATIC_DRAW);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndicies, indiciesData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, VERT_DATA_STRIDE, GL_FLOAT, GL_FALSE, sizeof(float) * VERT_DATA_STRIDE, nullptr);
	glEnableVertexAttribArray(0);

	return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

