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
#include <vg3o/skybox.h>
#include <vg3o/tween.h>

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

	vg3o::Light pointLights[MAX_LIGHTS]; 
	vg3o::SpotLight spotLights[MAX_LIGHTS];
	spotLights[0].position = ew::Vec3(2.0f, 1.5f, 0.0f);
	spotLights[0].rotation = ew::Vec3(0.0f, 180.0f, 0.0f);
	int activePointLights = 1;
	int activeSpotLights = 1;

	vg3o::Model newModel("assets/cars/gtr/gtr-exported.obj");

	ew::Transform modelTransform;
	modelTransform.scale = ew::Vec3(0.3f, 0.3f, 0.3f);

	// light source meshes
	ew::Mesh lightSources[MAX_LIGHTS];
	ew::Mesh spotLightSources[MAX_LIGHTS];

	ew::Transform lightSourceTransforms[MAX_LIGHTS];
	ew::Transform spotLightSourceTransforms[MAX_LIGHTS];

	ew::Vec3 sourceOffset = ew::Vec3(-90.0f, -90.0f, 0.0f);
	for (int i = 0; i < MAX_LIGHTS; i++) {
		lightSources[i] = ew::Mesh(ew::createSphere(0.1f, 32));
		pointLights[i].color = ew::Vec3(1, 1, 1);
	}
	for (int i = 0; i < MAX_LIGHTS; i++) {
		spotLightSources[i] = ew::Mesh(ew::createCylinder(0.1f, 0.25f, 32));
		spotLights[i].color = ew::Vec3(1, 1, 1);
	}
	//Sky Box set up
	std::vector<std::string> faces
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};

	vg3o::loadCubemap(faces);
	
	float tweenTime = 0;
	bool reverse = true;
	bool animating = true;

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
		shader.setVec3("_CameraPosition", camera.position);
		
		//Draw models
		
		shader.setMat4("_Model", modelTransform.getModelMatrix());
		newModel.draw(shader);
		
		tweenTime += deltaTime;
		if (tweenTime >= 1.25f) { tweenTime = 0.0f; reverse = !reverse; }

		// light setup
		shader.setInt("_PointLightAmount", activePointLights);
		for (int i = 0; i < activePointLights; i++) {
			shader.setVec3("_PointLights["+ std::to_string(i) +"].position", pointLights[i].position);
			shader.setVec3("_PointLights[" + std::to_string(i) + "].color", pointLights[i].color);
			shader.setFloat("_PointLights[" + std::to_string(i) + "].strength", pointLights[i].strength);

			shader.setVec3("_PointLights[" + std::to_string(i) + "].diffuse", pointLights[i].diffuse);
			shader.setVec3("_PointLights[" + std::to_string(i) + "].specular", pointLights[i].specular);

			shader.setFloat("_PointLights[" + std::to_string(i) + "].constant", pointLights[i].constant);
			shader.setFloat("_PointLights[" + std::to_string(i) + "].linear", pointLights[i].linear);
			shader.setFloat("_PointLights[" + std::to_string(i) + "].quadratic", pointLights[i].quadratic);
		}
		shader.setInt("_SpotLightAmount", activeSpotLights);
		for (int i = 0; i < activeSpotLights; i++) {
			if (animating) { spotLights[i].strength = vg3o::GetTweenValue(0.0f, 10.0f, tweenTime / 0.32f, vg3o::QUINTIC); }

			shader.setVec3("_SpotLights[" + std::to_string(i) + "].position", spotLights[i].position);
			shader.setVec3("_SpotLights[" + std::to_string(i) + "].direction", vg3o::getForwardVector(spotLights[i].rotation));
			shader.setVec3("_SpotLights[" + std::to_string(i) + "].color", spotLights[i].color);
			shader.setFloat("_SpotLights[" + std::to_string(i) + "].strength", spotLights[i].strength);

			shader.setVec3("_SpotLights[" + std::to_string(i) + "].diffuse", spotLights[i].diffuse);
			shader.setVec3("_SpotLights[" + std::to_string(i) + "].specular", spotLights[i].specular);

			shader.setFloat("_SpotLights[" + std::to_string(i) + "].cutoff", spotLights[i].cutoff);
		}
	
		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		for (int i = 0; i < activePointLights; i++) {
			lightSourceTransforms[i].position = pointLights[i].position;
			unlitShader.setMat4("_Model", lightSourceTransforms[i].getModelMatrix());
			unlitShader.setVec3("_Color", pointLights[i].color);
			lightSources[i].draw();
		}
		for (int i = 0; i < activeSpotLights; i++) {
			spotLightSourceTransforms[i].position = spotLights[i].position;
			spotLightSourceTransforms[i].rotation = sourceOffset + spotLights[i].rotation;
			unlitShader.setMat4("_Model", spotLightSourceTransforms[i].getModelMatrix());
			unlitShader.setVec3("_Color", spotLights[i].color);
			spotLightSources[i].draw();
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
			ImGui::Checkbox("Animating", &animating);
			ImGui::SliderInt("Point Light Amount", &activePointLights, 0, MAX_LIGHTS);
			ImGui::SliderInt("Spot Light Amount", &activeSpotLights, 0, MAX_LIGHTS);
			for (int i = 0; i < activePointLights; i++) {
				std::string str = "Point Light " + std::to_string(i);
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader(str.c_str())) {
					ImGui::DragFloat3("Position", &pointLights[i].position.x, 0.01f);
					ImGui::DragFloat3("Color", &pointLights[i].color.x, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Strength", &pointLights[i].strength, 0.01f, 0.0f, 10.0f);
					ImGui::DragFloat("Linear Coef", &pointLights[i].linear, 0.001f, 0.001f, 1.0f);
					ImGui::DragFloat("Quadratic Coef", &pointLights[i].quadratic, 0.001f, 0.001f, 2.0f);
				}
				ImGui::PopID();
			}
			for (int i = 0; i < activeSpotLights; i++) {
				std::string str = "Spot Light " + std::to_string(i);
				ImGui::PushID(i + activePointLights);
				if (ImGui::CollapsingHeader(str.c_str())) {
					ImGui::DragFloat3("Position", &spotLights[i].position.x, 0.01f);
					ImGui::DragFloat3("Rotation", &spotLights[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Color", &spotLights[i].color.x, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Strength", &spotLights[i].strength, 0.01f, 0.0f, 10.0f);
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


