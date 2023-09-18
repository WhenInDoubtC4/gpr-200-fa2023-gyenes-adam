#include <util/Shader.h>

#include <stdio.h>
#include <math.h>

#include <ew/ewMath/ewMath.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

struct Vertex
{
	float x;
	float y;
	float z;

	float u;
	float v;
};

GLuint createVAO(const Vertex* vertexData, int numVertices, const unsigned int* indiciesData, int numIndicies);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

constexpr Vertex VERT_DATA[4] = {
	{ -.8f, -.8f, 0.f, 0.f, 0.f }, //Bottom left
	{ .8f, -.8f, 0.f, 1.f, 0.f }, //Bottom right
	{ .8f, .8f, 0.f, 1.f, 1.f }, //Top right
	{ -.8f, .8f, 0.f, 0.f, 1.f }, //Top left
};

constexpr unsigned int VERT_INDICIES[6] = {
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
	GLuint vao = createVAO(VERT_DATA, 4, VERT_INDICIES, 6);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

GLuint createVAO(const Vertex* vertexData, int numVertices, const unsigned int* indiciesData, int numIndicies)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Define a new buffer id
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertexData, GL_STATIC_DRAW);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndicies, indiciesData, GL_STATIC_DRAW);

	//Position attribute
	const int positionAttibuteIndex = 0;
	glVertexAttribPointer(positionAttibuteIndex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, x)));
	glEnableVertexAttribArray(positionAttibuteIndex);

	//UV coord attribute
	const int uvAttributeIndex = 1;
	glVertexAttribPointer(uvAttributeIndex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, u)));
	glEnableVertexAttribArray(uvAttributeIndex);

	return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

