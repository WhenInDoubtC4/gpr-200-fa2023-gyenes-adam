#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>

#include "util/Camera.h"
//#include "util/Transformations.h"
#include "util/Global.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

//Projection will account for aspect ratio!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];

void InitCameraSettings(Util::Camera& camera, Util::CameraControls& cameraConrtrols)
{
	camera.position = ew::Vec3(0.f, 0.f, 5.f);
	camera.target = ew::Vec3(0.f, 0.f, 0.f);
	camera.fov = 60.f;
	camera.aspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.f;
	camera.isOrthographic = false;
	camera.ortographicHeight = 6.f;

	cameraConrtrols.yawDeg = -90.f;
	cameraConrtrols.pitchDeg = 0.f;
	cameraConrtrols.storeInitialMousePos = true;
	cameraConrtrols.mouseSensitivity = 0.1f;
	cameraConrtrols.movementSpeed = 5.f;
}

void moveCamera(GLFWwindow* window, Util::Camera* camera, Util::CameraControls* cameraControls, float deltaTime)
{
	//RMB not down
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
	{
		//Release cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		cameraControls->storeInitialMousePos = true;
		return;
	}

	//Hide cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	ew::Vec2 currentMousePos(mouseX, mouseY);

	if (cameraControls->storeInitialMousePos)
	{
		cameraControls->storeInitialMousePos = false;
		cameraControls->prevMousePos = currentMousePos;
	}

	//Get mouse delta for this frame
	ew::Vec2 deltaMousePos = currentMousePos - cameraControls->prevMousePos;

	//Add to yaw and pitch
	cameraControls->yawDeg += deltaMousePos.x * cameraControls->mouseSensitivity;
	cameraControls->pitchDeg -= deltaMousePos.y * cameraControls->mouseSensitivity;

	//Clamp pitch between -89 and 89 degrees
	cameraControls->pitchDeg = fmin(89.f, cameraControls->pitchDeg);
	cameraControls->pitchDeg = fmax(-89.f, cameraControls->pitchDeg);

	//Keep yaw between 0 and 360
	cameraControls->yawDeg -= 360.f * (int(cameraControls->yawDeg) / 360);

	//Store last mouse pos
	cameraControls->prevMousePos = currentMousePos;
	
	//Construct basis vectors
	float yawRad = Util::DegToRad(cameraControls->yawDeg); //theta
	float pitchRad = Util::DegToRad(cameraControls->pitchDeg); //phi

	ew::Vec3 tempUp(0.f, 1.f, 0.f);
	ew::Vec3 forward(cos(yawRad) * cos(pitchRad), sin(pitchRad), sin(yawRad) * cos(pitchRad));
	ew::Vec3 right = ew::Normalize(ew::Cross(forward, tempUp));
	ew::Vec3 up = ew::Normalize(ew::Cross(right, forward));

	if (glfwGetKey(window, GLFW_KEY_W)) camera->position += forward * cameraControls->movementSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S))	camera->position += -forward * cameraControls->movementSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A)) camera->position += -right * cameraControls->movementSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D)) camera->position += right * cameraControls->movementSpeed * deltaTime;

	camera->target = camera->position + forward;
}

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
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

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));

	Util::Camera camera;
	Util::CameraControls cameraControls;
	InitCameraSettings(camera, cameraControls);

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

	double prevTime = 0.;
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		double currentTime = glfwGetTime();
		float deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		moveCamera(window, &camera, &cameraControls, deltaTime);

		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		camera.aspectRatio = float(windowWidth) / float(windowHeight);

		//Set uniforms
		shader.use();
		shader.setMat4("_view", camera.ViewMatrix());
		shader.setMat4("_world", camera.ProjectionMatrix());

		//Set model matrix uniform
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());
			cubeMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Text("Cubes");
			for (size_t i = 0; i < NUM_CUBES; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}
			ImGui::Text("Camera");
			ImGui::DragFloat3("Position", &camera.position.x, 0.05f);
			ImGui::DragFloat3("Target", &camera.target.x, 0.05f);
			ImGui::Checkbox("Ortographic", &camera.isOrthographic);
			if (camera.isOrthographic)
			{
				ImGui::DragFloat("Ortho height", &camera.ortographicHeight, 0.05f);
			}
			else
			{
				ImGui::SliderFloat("FOV", &camera.fov, 20.f, 180.f);
			}
			ImGui::DragFloat("Near clip plane", &camera.nearPlane, 0.05f);
			ImGui::DragFloat("Far clip plane", &camera.farPlane, 0.2f);
			ImGui::Text("Yaw=%f", cameraControls.yawDeg);
			ImGui::Text("Pitch=%f", cameraControls.pitchDeg);
			ImGui::DragFloat("Mouse sensitivity", &cameraControls.mouseSensitivity, 0.01f, 0.01f, 1.f);
			ImGui::DragFloat("Movement speed", &cameraControls.movementSpeed, 0.1f, 0.1f, 50.f);
			if (ImGui::Button("Reset")) InitCameraSettings(camera, cameraControls);
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

