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

float timeScale = 1.f;

float dayHorizonColor[3] = { 0.647f, 0.886f, 0.961f };
float dayZenithColor[3] = { 0.302f, 0.478f, 0.659f };
float nightHorizonColor[3] = { 0.961f, 0.443f, 0.09f };
float nightZenithColor[3] = { 0.325f, 0.251f, 0.439f };

float sunPosition[2] = { 0.f, 0.f };
float sunColor[3] = { 1.f, 0.925f, 0.639f };
float sunRadius = 0.05f;
float sunSmoothness = 0.02f;
float sunFlareRadius = 0.3f;
float sunFlareSmoothness = 0.4f;
float sunFlareIntenisty = 0.8f;

float moonPosition[2] = { -0.8f, 0.5f };
float moonColor[3] = { 0.878f, 0.957f, 1.0f };
float moonRadius = 0.08f;
float moonSmoothness = 0.01f;

float hillsL1Color[3] = { 0.2f, 0.2f, 0.2f };
float hillsL2Color[3] = { 0.3f, 0.3f, 0.3f };
float treesColor[3] = { 0.4f, 0.4f, 0.4f };
float shadowDarkess = .1f;
float shadowIntenisty = .5f;
float shadowLength = 0.05f;

bool showImGUIDemoWindow = false;

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

		float time = glfwGetTime();

		//Set uniforms
		shader.setVec2("_resolution", float(SCREEN_WIDTH), float(SCREEN_HEIGHT));
		shader.setFloat("_time", time);
		shader.setFloat("_timeScale", timeScale);

		shader.setVec3("_dayHorizonColor", dayHorizonColor[0], dayHorizonColor[1], dayHorizonColor[2]);
		shader.setVec3("_dayZenithColor", dayZenithColor[0], dayZenithColor[1], dayZenithColor[2]);
		shader.setVec3("_nightHorizonColor", nightHorizonColor[0], nightHorizonColor[1], nightHorizonColor[2]);
		shader.setVec3("_nightZenithColor", nightZenithColor[0], nightZenithColor[1], nightZenithColor[2]);

		shader.setVec2("_sunPos", sunPosition[0], sunPosition[1]);
		shader.setVec3("_sunColor", sunColor[0], sunColor[1], sunColor[2]);
		shader.setFloat("_sunRadius", sunRadius);
		shader.setFloat("_sunSmoothness", sunSmoothness);
		shader.setFloat("_sunFlareRadius", sunFlareRadius);
		shader.setFloat("_sunFlareSmoothness", sunFlareSmoothness);
		shader.setFloat("_sunFlareIntensity", sunFlareIntenisty);

		shader.setVec2("_moonPos", moonPosition[0], moonPosition[1]);
		shader.setVec3("_moonColor", moonColor[0], moonColor[1], moonColor[2]);
		shader.setFloat("_moonRadius", moonRadius);
		shader.setFloat("_moonSmoothness", moonSmoothness);

		shader.setVec3("_hillsL1Color", hillsL1Color[0], hillsL1Color[1], hillsL1Color[2]);
		shader.setVec3("_hillsL2Color", hillsL2Color[0], hillsL2Color[1], hillsL2Color[2]);
		shader.setVec3("_treesColor", treesColor[0], treesColor[1], treesColor[2]);
		shader.setFloat("_shadowDarkness", shadowDarkess);
		shader.setFloat("_shadowIntensity", shadowIntenisty);
		shader.setFloat("_shadowLength", shadowLength);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
			ImGui::InputFloat("Time scale", &timeScale, 0.1f, 0.f);
			
			if (ImGui::CollapsingHeader("Sky"))
			{
				ImGui::ColorEdit3("Day horizon color", dayHorizonColor, ImGuiColorEditFlags_Float);
				ImGui::ColorEdit3("Day zenith color", dayZenithColor, ImGuiColorEditFlags_Float);
				ImGui::ColorEdit3("Night horizon color", nightHorizonColor, ImGuiColorEditFlags_Float);
				ImGui::ColorEdit3("Night zenith color", nightZenithColor, ImGuiColorEditFlags_Float);
			}

			if (ImGui::CollapsingHeader("Sun"))
			{
				ImGui::SliderFloat2("Sun position", sunPosition, -1.f, 1.f);
				ImGui::ColorEdit3("Sun color", sunColor, ImGuiColorEditFlags_Float);
				ImGui::SliderFloat("Sun radius", &sunRadius, 0.f, .5f);
				ImGui::SliderFloat("Sun smoothness", &sunSmoothness, 0.f, .5f);
				ImGui::SliderFloat("Sun flare radius", &sunFlareRadius, 0.f, 1.f);
				ImGui::SliderFloat("Sun flare smoothness", &sunFlareSmoothness, 0.f, 1.f);
				ImGui::SliderFloat("Sun flare intensity", &sunFlareIntenisty, 0.f, 1.f);
			}

			if (ImGui::CollapsingHeader("Moon"))
			{
				ImGui::SliderFloat2("Moon position", moonPosition, -1.f, 1.f);
				ImGui::ColorEdit3("Moon color", moonColor, ImGuiColorEditFlags_Float);
				ImGui::SliderFloat("Moon radius", &moonRadius, 0.f, .5f);
				ImGui::SliderFloat("Moon smoothness", &moonSmoothness, 0.f, .5f);
			}

			if (ImGui::CollapsingHeader("Foreground"))
			{
				ImGui::ColorEdit3("L1 hills color", hillsL1Color, ImGuiColorEditFlags_Float);
				ImGui::ColorEdit3("L2 hills color", hillsL2Color, ImGuiColorEditFlags_Float);
				ImGui::ColorEdit3("Trees color", treesColor, ImGuiColorEditFlags_Float);
				ImGui::SliderFloat("Shadow darkness", &shadowDarkess, 0.f, .5f);
				ImGui::SliderFloat("Shadow intensity", &shadowIntenisty, 0.f, 1.f);
				ImGui::SliderFloat("Shadow length", &shadowLength, 0.f, .5f);
			}

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

