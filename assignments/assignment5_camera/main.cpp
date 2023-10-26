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

#include "vg3o/camera.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

//Projection will account for aspect ratio!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];
vg3o::Camera cameraObj;

void moveCamera(GLFWwindow* window, vg3o::Camera* camera, vg3o::CameraControls* camData, float dt)
{
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		camData->firstMouse = true;
		return;
	}
	// disable visual cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// mouse position getting
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (camData->firstMouse)
	{
		camData->firstMouse = false;
		camData->previousX = mouseX;
		camData->previousY = mouseY;
	}
	
	// mouse delta
	double deltaX = (mouseX - camData->previousX) * camData->sensitivity, deltaY = (mouseY - camData->previousY) * camData->sensitivity;

	camData->yaw += deltaX;
	camData->pitch += deltaY;

	camData->pitch = camData->pitch >= 89 || camData->pitch <= -89 ? 89 * vg3o::sign(camData->pitch) : camData->pitch;

	camData->previousX = mouseX;
	camData->previousY = mouseY;

	ew::Vec3 forward = ew::Vec3(
		cos(ew::Radians(camData->yaw)) * cos(ew::Radians(camData->pitch)),
		sin(ew::Radians(camData->pitch)),
		sin(ew::Radians(camData->yaw)) * cos(ew::Radians(camData->pitch))
	);
	ew::Vec3 up = ew::Vec3(0, 1, 0);
	ew::Vec3 right = ew::Normalize(ew::Cross(forward, up));
	up = ew::Normalize(ew::Cross(right, forward));

	if (glfwGetKey(window, GLFW_KEY_W)){
		camera->position += forward * camData->speed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		camera->position -= forward * camData->speed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		camera->position -= right * camData->speed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		camera->position += right * camData->speed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		camera->position -= up * camData->speed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		camera->position += up * camData->speed * dt;
	}

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

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

	// Camera Setup
	vg3o::CameraControls camData;

	cameraObj.position = ew::Vec3(0, 0, 5);
	cameraObj.target = ew::Vec3(0, 0, 0);
	cameraObj.fov = 60;
	cameraObj.orthoSize = 6;
	cameraObj.nearPlane = 0.1;
	cameraObj.farPlane = 100;
	cameraObj.aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

	float previousTime = (float)glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float dt = time - previousTime;
		previousTime = time;

		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// move the camera
		moveCamera(window, &cameraObj, &camData, dt);

		//Set uniforms
		shader.use();

		//TODO: Set model matrix uniform
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());
			cubeMesh.draw();
		}

		shader.setMat4("_ViewProjection", cameraObj.ProjectionMatrix() * cameraObj.ViewMatrix());

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
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::Indent();
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cameraObj.position.x, 0.05f);
					ImGui::DragFloat3("Target", &cameraObj.target.x, 0.05f);
				}
				ImGui::Unindent();
				ImGui::Checkbox("Orthographic", &cameraObj.orthographic);
				if (cameraObj.orthographic) {
					ImGui::SliderFloat("Ortho Size", &cameraObj.orthoSize, 0.01f, 10.0f);
				}
				else {
					ImGui::SliderFloat("FOV", &cameraObj.fov, 0.01f, 185.0f);
				}
				ImGui::SliderFloat("Near Plane", &cameraObj.nearPlane, 0.01f, 25.0f);
				ImGui::SliderFloat("Far Plane", &cameraObj.farPlane, 0.01f, 250.0f);
				
				ImGui::Text("Camera Data:");
				ImGui::Indent();
				ImGui::Text("Yaw: %f", camData.yaw);
				ImGui::Text("Pitch: %f", camData.pitch);
				ImGui::Unindent();

				ImGui::SliderFloat("Move Speed", &camData.speed, 0.01f, 20.0f);

				if (ImGui::Button("Reset")) {
					cameraObj.position = ew::Vec3(0, 0, 5);
					cameraObj.target = ew::Vec3(0, 0, 0);
					cameraObj.fov = 60;
					cameraObj.orthoSize = 6;
					cameraObj.nearPlane = 0.1;
					cameraObj.farPlane = 100;
					camData.yaw = -90;
					camData.pitch = 0;
					camData.previousX = NULL;
					camData.previousY = NULL;
					camData.speed = 5.0f;
					camData.firstMouse = true;
				}
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
	float newAspect = (float)width / (float)height;
	cameraObj.aspectRatio = newAspect;
}

