#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <util/shader.h>

#include "util/Texture.h"

struct Vertex {
	float x, y, z;
	float u, v;
};

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned short* indicesData, int numIndices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

Vertex vertices[4] = {
	{-1.0, -1.0, 0.0, 0.0, 0.0},
	{1.0, -1.0, 0.0, 1.0, 0.0},
	{1.0, 1.0, 0.0, 1.0, 1.0},
	{-1.0, 1.0, 0.0, 0.0, 1.0}
};
unsigned short indices[6] = {
	0, 1, 2,
	2, 3, 0
};

//Background params
float waterColor[3] = { 0.631f, 0.898f, 1.f };
float waveSpeed = .1f;
float warpIntensity = .2f;

//Character params
float characterScale = .4f;
float characterMinOffset[2] = { -1.f, -1.f };
float characterMaxOffset[2] = { 1.f, 1.f };

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textures", NULL, NULL);
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

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint brickTexture = Util::loadTexture("assets/Bricks059_2K-JPG_Color.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);
	GLuint noiseTexture = Util::loadTexture("assets/noiseTexture.png", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);
	GLuint characterTexture = Util::loadTexture("assets/character.png", GL_CLAMP_TO_EDGE, GL_NEAREST);

	Util::Shader backgroundShader("assets/background.vert", "assets/background.frag");
	Util::Shader characterShader("assets/character.vert", "assets/character.frag");

	unsigned int quadVAO = createVAO(vertices, 4, indices, 6);

	glBindVertexArray(quadVAO);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float time = glfwGetTime();

		//Draw background
		backgroundShader.exec();
		SET_SHADER_TEXTURE(backgroundShader, "_backgroundTexture", brickTexture, 0);
		SET_SHADER_TEXTURE(backgroundShader, "_noiseTexture", noiseTexture, 1);
		backgroundShader.setFloat("_time", time);
		backgroundShader.setVec3("_waterColor", waterColor[0], waterColor[1], waterColor[2]);
		backgroundShader.setFloat("_waveSpeed", waveSpeed);
		backgroundShader.setFloat("_warpIntenisty", warpIntensity);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

		//Draw character
		characterShader.exec();
		SET_SHADER_TEXTURE(characterShader, "_texture", characterTexture, 2);
		characterShader.setFloat("_time", time);
		characterShader.setFloat("_scale", characterScale);
		characterShader.setVec2("_minOffset", characterMinOffset[0], characterMinOffset[1]);
		characterShader.setVec2("_maxOffset", characterMaxOffset[0], characterMaxOffset[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");

			if (ImGui::CollapsingHeader("Backround"))
			{
				ImGui::ColorEdit3("Water color", waterColor, ImGuiColorEditFlags_Float);
				ImGui::SliderFloat("Wave speed", &waveSpeed, 0.f, 1.f);
				ImGui::SliderFloat("Warp intensity", &warpIntensity, 0.f, 1.f);
			}

			if (ImGui::CollapsingHeader("Character"))
			{
				ImGui::SliderFloat("Character scale", &characterScale, 0.f, 1.f);
				ImGui::SliderFloat2("Character min offset", characterMinOffset, -1.f, 0.f);
				ImGui::SliderFloat2("Character max offset", characterMaxOffset, 0.f, 1.f);
			}
			
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned short* indicesData, int numIndices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Vertex Buffer Object 
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*numVertices, vertexData, GL_STATIC_DRAW);

	//Element Buffer Object
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * numIndices, indicesData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex,x));
	glEnableVertexAttribArray(0);

	//UV attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, u)));
	glEnableVertexAttribArray(1);

	return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

