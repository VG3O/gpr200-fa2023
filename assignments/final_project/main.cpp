#include <stdio.h>
#include <math.h>
#include <string>

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

#include <vg3o/model.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

const int MAX_LIGHTS = 6;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

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

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
	ew::Shader unlitShader("assets/unlit.vert", "assets/unlit.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	//Create cube
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));

	resetCamera(camera,cameraController);

	vg3o::Light lights[MAX_LIGHTS]; 
	int activeLights = 1;

	vg3o::Model newModel("assets/cars/gtr/gtr-exported.obj");

	ew::Transform modelTransform;
	modelTransform.scale = ew::Vec3(0.3f, 0.3f, 0.3f);

	// light source meshes
	ew::Mesh lightSources[MAX_LIGHTS];
	ew::Transform lightSourceTransforms[MAX_LIGHTS];
	for (int i = 0; i < MAX_LIGHTS; i++) {
		lightSources[i] = ew::Mesh(ew::createSphere(0.1f, 32));
		lights[i].color = ew::Vec3(1, 1, 1);
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		//Draw models
		unsigned int diffuseNum = 1;
		unsigned int specularNum = 1;


		shader.setMat4("_Model", modelTransform.getModelMatrix());
		newModel.draw(shader);

		// light setup
		shader.setInt("_LightAmount", activeLights);
		for (int i = 0; i < activeLights; i++) {
			shader.setVec3("_Lights["+ std::to_string(i) +"].position", lights[i].position);
			shader.setVec3("_Lights[" + std::to_string(i) + "].color", lights[i].color);
			shader.setFloat("_Lights[" + std::to_string(i) + "].strength", lights[i].strength);

			shader.setVec3("_Lights[" + std::to_string(i) + "].diffuse", lights[i].diffuse);
			shader.setVec3("_Lights[" + std::to_string(i) + "].specular", lights[i].specular);

			shader.setFloat("_Lights[" + std::to_string(i) + "].constant", lights[i].constant);
			shader.setFloat("_Lights[" + std::to_string(i) + "].linear", lights[i].linear);
			shader.setFloat("_Lights[" + std::to_string(i) + "].quadratic", lights[i].quadratic);
		}
	
		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		for (int i = 0; i < activeLights; i++) {
			lightSourceTransforms[i].position = lights[i].position;
			unlitShader.setMat4("_Model", lightSourceTransforms[i].getModelMatrix());
			unlitShader.setVec3("_Color", lights[i].color);
			lightSources[i].draw();
		}

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
			ImGui::SliderInt("Light Amount", &activeLights, 0, MAX_LIGHTS);
			for (int i = 0; i < activeLights; i++) {
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Light")) {
					ImGui::DragFloat3("Position", &lights[i].position.x, 0.01f);
					ImGui::DragFloat3("Color", &lights[i].color.x, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Strength", &lights[i].strength, 0.01f, 0.0f, 10.0f);
					ImGui::DragFloat("Linear Coef", &lights[i].linear, 0.001f, 0.001f, 1.0f);
					ImGui::DragFloat("Quadratic Coef", &lights[i].quadratic, 0.001f, 0.001f, 2.0f);
				}
				ImGui::PopID();
			}

			ImGui::ColorEdit3("BG color", &bgColor.x);
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
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}


