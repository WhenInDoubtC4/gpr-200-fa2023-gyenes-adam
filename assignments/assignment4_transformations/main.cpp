#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/ewMath/vec3.h>
#include <ew/procGen.h>

#include "util/Shader.h"
#include "util/Transformations.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

//Square aspect ratio for now. We will account for this with projection later.
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;

constexpr size_t CUBE_COUNT = 4;
constexpr int GRID_COL_COUNT = 2;
const ew::Vec3 GRID_TOP_LEFT = ew::Vec3(-0.5f, 0.5f, 0.f);
const ew::Vec3 GRID_BOTTOM_RIGHT = ew::Vec3(0.5f, -0.5f, 0.f);
Util::Transform modelTransform[CUBE_COUNT];

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

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	//Equally distribute cube positions
	int xIndex = 0;
	int yIndex = 0;
	const float xStep = ((GRID_BOTTOM_RIGHT.x - GRID_TOP_LEFT.x) * 2.f) / GRID_COL_COUNT;
	const float yStep = ((GRID_TOP_LEFT.y - GRID_BOTTOM_RIGHT.y) * 2.f) / (CUBE_COUNT / GRID_COL_COUNT);
	for (size_t i = 0; i < CUBE_COUNT; i++)
	{
		xIndex = i % GRID_COL_COUNT;
		yIndex = i / GRID_COL_COUNT;

		modelTransform[i].position = ew::Vec3(GRID_TOP_LEFT.x + xStep * xIndex, GRID_BOTTOM_RIGHT.y + yStep * yIndex);
	}
	Util::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set model matrix uniform
		for (size_t i = 0; i < CUBE_COUNT; i++)
		{
			shader.exec();
			shader.setMat4("_model", modelTransform[i].getModelMat());
			cubeMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Transform");
			for (size_t i = 0; i < CUBE_COUNT; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform"))
				{
					ImGui::DragFloat3("Position", &modelTransform[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &modelTransform[i].rotateDeg.x, 1.f, 0.f, 360.f);
					ImGui::DragFloat3("Scale", &modelTransform[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

