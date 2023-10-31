#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>

#include "util/ProcGen.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;

struct AppSettings {
	const char* shadingModeNames[6] = { "Solid Color","Normals","UVs","Texture","Lit","Texture Lit"};
	int shadingModeIndex;

	ew::Vec3 bgColor = ew::Vec3(0.1f);
	ew::Vec3 shapeColor = ew::Vec3(1.0f);
	bool wireframe = true;
	bool drawAsPoints = false;
	bool backFaceCulling = true;

	//Euler angles (degrees)
	ew::Vec3 lightRotation = ew::Vec3(0, 0, 0);
}appSettings;

ew::Camera camera;
ew::CameraController cameraController;

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
	glPointSize(3.0f);
	glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_LINE : GL_FILL);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	//Plane
	float planeWidth = 1.f;
	float planeHeight = 1.f;
	int planeSubdivisions = 5;
	ew::MeshData planeMeshData = Util::createPlane(planeWidth, planeHeight, planeSubdivisions);
	ew::Mesh planeMesh(planeMeshData);
	ew::Transform planeTransform;
	planeTransform.position = ew::Vec3(-3.f, 0.f, 0.f);

	//Cylinder
	float cylinderHeight = 2.f;
	float cylinderRadius = 1.f;
	int cylinderSegments = 8;
	ew::MeshData cylinderMeshData = Util::createCylidner(cylinderHeight, cylinderRadius, cylinderSegments);
	ew::Mesh cylinderMesh(cylinderMeshData);
	ew::Transform cylinderTransform;
	cylinderTransform.position = ew::Vec3(-6.f, 0.f, 0.f);

	//Sphere
	float sphereRadius = 1.f;
	int sphereSegments = 8;
	ew::MeshData sphereMeshData = Util::createSphere(sphereRadius, sphereSegments);
	ew::Mesh sphereMesh(sphereMeshData);
	ew::Transform sphereTransform;
	sphereTransform.position = ew::Vec3(3.f, 0.f, 0.f);

	//Torus
	float torusInnerRadius = 0.3f;
	float torusOuterRadius = 2.f;
	int torusInnerSegments = 15;
	int torusOuterSegments = 15;
	ew::MeshData torusMeshData = Util::createTorus(torusInnerRadius, torusOuterRadius, torusInnerSegments, torusOuterSegments);
	ew::Mesh torusMesh(torusMeshData);
	ew::Transform torusTransform;
	torusTransform.position = ew::Vec3(8.f, 0.f, 0.f);

	//Create cube
	ew::MeshData cubeMeshData = ew::createCube(0.5f);
	ew::Mesh cubeMesh(cubeMeshData);

	//Initialize transforms
	ew::Transform cubeTransform;

	resetCamera(camera,cameraController);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		cameraController.Move(window, &camera, deltaTime);

		//Render
		glClearColor(appSettings.bgColor.x, appSettings.bgColor.y, appSettings.bgColor.z,1.0f);

		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setInt("_Mode", appSettings.shadingModeIndex);
		shader.setVec3("_Color", appSettings.shapeColor);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		//Euler angels to forward vector
		ew::Vec3 lightRot = appSettings.lightRotation * ew::DEG2RAD;
		ew::Vec3 lightF = ew::Vec3(sinf(lightRot.y) * cosf(lightRot.x), sinf(lightRot.x), -cosf(lightRot.y) * cosf(lightRot.x));
		shader.setVec3("_LightDir", lightF);

		//Draw cube
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

		//Draw plane
		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

		//Draw cylidner
		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

		//Draw sphere
		shader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

		//Draw torus
		shader.setMat4("_Model", torusTransform.getModelMatrix());
		torusMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}

			ImGui::ColorEdit3("BG color", &appSettings.bgColor.x);
			ImGui::ColorEdit3("Shape color", &appSettings.shapeColor.x);
			ImGui::Combo("Shading mode", &appSettings.shadingModeIndex, appSettings.shadingModeNames, IM_ARRAYSIZE(appSettings.shadingModeNames));
			if (appSettings.shadingModeIndex > 3) {
				ImGui::DragFloat3("Light Rotation", &appSettings.lightRotation.x, 1.0f);
			}
			ImGui::Checkbox("Draw as points", &appSettings.drawAsPoints);
			if (ImGui::Checkbox("Wireframe", &appSettings.wireframe)) {
				glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_LINE : GL_FILL);
			}
			if (ImGui::Checkbox("Back-face culling", &appSettings.backFaceCulling)) {
				if (appSettings.backFaceCulling)
					glEnable(GL_CULL_FACE);
				else
					glDisable(GL_CULL_FACE);
			}
			if (ImGui::CollapsingHeader("Plane"))
			{
				ImGui::DragFloat("Plane width", &planeWidth, 0.05f, 0.05f, 10.f);
				ImGui::DragFloat("Plane height", &planeHeight, 0.05f, 0.05f, 10.f);
				ImGui::SliderInt("Plane subdivisions", &planeSubdivisions, 1, 20);
				
				planeMeshData = Util::createPlane(planeWidth, planeHeight, planeSubdivisions);
				planeMesh = ew::Mesh(planeMeshData);
			}
			if (ImGui::CollapsingHeader("Cylinder"))
			{
				ImGui::DragFloat("Cylinder height", &cylinderHeight, 0.05f, 0.2f, 10.f);
				ImGui::DragFloat("Cylinder radius", &cylinderRadius, 0.05f, 0.1f, 5.f);
				ImGui::SliderInt("Cuylinder segments", &cylinderSegments, 1, 20);

				cylinderMeshData = Util::createCylidner(cylinderHeight, cylinderRadius, cylinderSegments);
				cylinderMesh = ew::Mesh(cylinderMeshData);
			}
			if (ImGui::CollapsingHeader("Sphere"))
			{
				ImGui::DragFloat("Sphere radius", &sphereRadius, 0.05f, 0.05f, 10.f);
				ImGui::SliderInt("Sphere segments", &sphereSegments, 3, 64);

				sphereMeshData = Util::createSphere(sphereRadius, sphereSegments);
				sphereMesh = ew::Mesh(sphereMeshData);
			}
			if (ImGui::CollapsingHeader("Torus"))
			{
				ImGui::DragFloat("Torus inner radius", &torusInnerRadius, 0.05f, 0.05f, 5.f);
				ImGui::DragFloat("Torus outer radius", &torusOuterRadius, 0.05f, 0.05f, 10.f);
				ImGui::SliderInt("Torus inner segments", &torusInnerSegments, 3, 64);
				ImGui::SliderInt("Torus outer segments", &torusOuterSegments, 3, 64);

				torusMeshData = Util::createTorus(torusInnerRadius, torusOuterRadius, torusInnerSegments, torusOuterSegments);
				torusMesh = ew::Mesh(torusMeshData);
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
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 3);
	camera.target = ew::Vec3(0);
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}


