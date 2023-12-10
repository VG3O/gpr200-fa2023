#include <stdio.h>
#include <math.h>
#include <string>
#include <random>

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
#include <vg3o/cutscene.h>
#include <vg3o/framebuffer.h>
#include <vg3o/procGen.h>

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

	glfwWindowHint(GLFW_SAMPLES, 4);

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
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
	ew::Shader unlitShader("assets/unlit.vert", "assets/unlit.frag");
	ew::Shader skyboxShader("assets/skybox.vert", "assets/skybox.frag");
	ew::Shader screenShader("assets/screen.vert", "assets/screen.frag");
	ew::Shader blurShader("assets/blur.vert", "assets/blur.frag");

	resetCamera(camera,cameraController);

	vg3o::Light pointLights[MAX_LIGHTS];
	int pointLightTemperature[MAX_LIGHTS];

	std::random_device rd;
	std::mt19937 e(rd());
	std::uniform_int_distribution<int> dist(2500, 13500);

	for (int i = 0; i < MAX_LIGHTS; i++) {
		pointLightTemperature[i] = dist(e);
	}

	vg3o::SpotLight spotLights[MAX_LIGHTS];
	vg3o::DirectionalLight sun;

	const float lightPosition = 3.0f;

	int sunColor = 5500;
	sun.ambient = vg3o::GetTemperatureColor(sunColor);
	sun.diffuse = vg3o::GetTemperatureColor(sunColor);
	sun.specular = vg3o::GetTemperatureColor(sunColor);
	sun.direction = ew::Vec3(-1.0f, -1.0f, 0.0f);

	// car lights
	// muddies code but i didnt want to do this iteravely
	vg3o::SpotLight headlights[10];
	
	headlights[0].position = ew::Vec3(-0.63f, -0.36f, -2.49f);
	headlights[1].position = ew::Vec3(-0.78f, -0.33f, -2.63f);
	headlights[2].position = ew::Vec3(-0.77f, -0.12f, -2.46f);
	headlights[3].position = ew::Vec3(-0.69f, -0.34f, -2.41f);
	headlights[4].position = ew::Vec3(-0.76f, -0.31f, -2.57f);

	headlights[5].position = ew::Vec3(0.63f, -0.36f, -2.49f);
	headlights[6].position = ew::Vec3(0.78f, -0.33f, -2.63f);
	headlights[7].position = ew::Vec3(0.77f, -0.12f, -2.46f);
	headlights[8].position = ew::Vec3(0.69f, -0.34f, -2.41f);
	headlights[9].position = ew::Vec3(0.76f, -0.31f, -2.57f);

	headlights[0].rotation = ew::Vec3(-6.0f, -256.0f, 0.0f);
	headlights[1].rotation = ew::Vec3(-4.0f, -271.0f, 0.0f);
	headlights[2].rotation = ew::Vec3(-29.0f, 91.0f, 0.0f);
	headlights[3].rotation = ew::Vec3(-10.0f, 97.0f, 0.0f);
	headlights[4].rotation = ew::Vec3(-13.0f, -280.0f, 0.0f);
	
	headlights[5].rotation = ew::Vec3(-6.0f, -288.0f, 0.0f);
	headlights[6].rotation = ew::Vec3(-4.0f, -270.0f, 0.0f);
	headlights[7].rotation = ew::Vec3(-29.0f, 90.0f, 0.0f);
	headlights[8].rotation = ew::Vec3(-10.0f, 84.0f, 0.0f);
	headlights[9].rotation = ew::Vec3(-13.0f, -259.0f, 0.0f);

	int headlightColor = 3750;

	for (int i = 0; i < 10; i++) {
		headlights[i].color = vg3o::GetTemperatureColor(headlightColor);
		headlights[i].cutoff = 1.0f;
		headlights[i].range = 1.0f;
	}

	headlights[0].strength = 116.0f;
	headlights[1].strength = 181.0f;
	headlights[2].strength = 130.0f;
	headlights[3].strength = 164.0f;
	headlights[4].strength = 64.0f;

	headlights[5].strength = 116.0f;
	headlights[6].strength = 181.0f;
	headlights[7].strength = 130.0f;
	headlights[8].strength = 164.0f;
	headlights[9].strength = 64.0f;
	
	headlights[0].outerCutoff = 10.0f;
	headlights[1].outerCutoff = 9.0f;
	headlights[2].outerCutoff = 11.5f;
	headlights[3].outerCutoff = 16.0f;
	headlights[4].outerCutoff = 9.0f;
	
	headlights[5].outerCutoff = 10.0f;
	headlights[6].outerCutoff = 9.0f;
	headlights[7].outerCutoff = 11.5f;
	headlights[8].outerCutoff = 16.0f;
	headlights[9].outerCutoff = 9.0f;

	int activePointLights = 6;

	vg3o::Model newModel("assets/cars/gtr/gtr-exported.obj");

	ew::Transform modelTransform;
	modelTransform.scale = ew::Vec3(0.3f, 0.3f, 0.3f);

	// light source meshes
	ew::Mesh lightSources[MAX_LIGHTS];

	ew::Transform lightSourceTransforms[MAX_LIGHTS];

	for (int i = 0; i < MAX_LIGHTS; i++) {
		lightSources[i] = ew::Mesh(ew::createSphere(0.1f, 32));
		pointLights[i].color = ew::Vec3(1, 1, 1);
		pointLights[i].strength = 4.0f;
		pointLights[i].radius = 7.5f;
	}

	//------------------------
	// skybox setup
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// skybox texture paths
	std::vector<std::string> faces
	{
		"assets/skybox/right.png",
		"assets/skybox/left.png",
		"assets/skybox/top.png",
		"assets/skybox/bottom.png",
		"assets/skybox/front.png",
		"assets/skybox/back.png"
	};

	unsigned int cubemapTexture = vg3o::loadCubemap(faces);
	//------------------------------------------
	
	//------------------------------------------
	// set up the ground texture
	// this plane uses normal mapping
	// configured procGen.h to generate tangents for 
	ew::MeshData groundData = vg3o::createPlane(150.0f, 150.0f, 1);
	for (int i = 0; i < groundData.vertices.size(); i++) { groundData.vertices[i].uv *= 60.0f; }

	ew::Mesh ground = ew::Mesh(groundData); ew::Transform groundTransform;
	groundTransform.position = ew::Vec3(0.0f, -0.887f, 0.0f);

	ew::Texture groundDiffuse, groundNormal;
	groundDiffuse.id = ew::loadTexture("assets/rocks/rocks_diffuse.jpg", GL_REPEAT, GL_LINEAR);
	groundDiffuse.path = "assets/rocks/rocks_diffuse.jpg";
	groundDiffuse.type = "texture_diffuse";
	groundNormal.id = ew::loadTexture("assets/rocks/rocks_normal.jpg", GL_REPEAT, GL_LINEAR);
	groundNormal.path = "assets/rocks/rocks_normal.jpg";
	groundNormal.type = "texture_normal";
	ground.addTexture(groundDiffuse);
	ground.addTexture(groundNormal);
	
	vg3o::Material groundMaterial;
	groundMaterial.ambient = ew::Vec3(0.5f, 0.5f, 0.5f);
	groundMaterial.diffuse = ew::Vec3(0.5f, 0.5f, 0.5f);
	groundMaterial.specular = ew::Vec3(0.5f, 0.5f, 0.5f);
	groundMaterial.shininess = 20.f;
	groundMaterial.opacity = 1.f;
	groundMaterial.hasBump = true;
	groundMaterial.hasDiffuse = true;

	ground.setMaterial(groundMaterial);

	//-----------------------------------------------
	// blur framebufffers
	// this is used for bloom but until we take GPR-300 it is a very very very basic implementation
	// it doesn't look the best it could but i'm happy I got it to work
	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	//-----------------------------------------------
	// cutscene setup (stretch goal but not finishing it for this)

	float tweenTime = 0;
	bool reverse = true;
	bool animating = true;

	vg3o::Cutscene cutscene = vg3o::Cutscene(10.0f);

	float valueToRead = 0.0f;
	vg3o::CutsceneEvent<float> newEvent;
	newEvent.valueReference = &valueToRead;
	newEvent.startValue = 0.0f;
	newEvent.targetValue = 5.0f;
	newEvent.duration = 5.0f;
	newEvent.startTime = 10.0f;
	newEvent.style = vg3o::LINEAR;

	cutscene.AddEvent(newEvent);

	//-----------------------------------------------
	// configure shaders
	shader.use();
	shader.setInt("_Skybox", 10);

	skyboxShader.use();
	skyboxShader.setInt("_Skybox", 10);

	blurShader.use();
	blurShader.setInt("_Screen", 0);

	screenShader.use();
	screenShader.setInt("_HDRTexture", 0);
	screenShader.setInt("_Bloom", 1);

	//---------------------------------
	// set up screen buffers

	vg3o::ScreenBuffer framebuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	// active booleans
	bool headlightsActive = false;
	bool bloom = false;
	int bufferMode = 0;

	// change default camera position
	camera.position = ew::Vec3(-8.113f, 3.866f, -11.319f);
	camera.target = ew::Vec3(-7.499f, 3.584f, -10.582f);
	camera.fov = 17.812f;

	cameraController.pitch = -16.3f;
	cameraController.yaw = 140.1f;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		framebuffer.useBuffer();
		glEnable(GL_DEPTH_TEST);
		//RENDER
		glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		shader.setVec3("_CameraPosition", camera.position);

		//Draw models

		shader.setMat4("_Model", modelTransform.getModelMatrix());
		newModel.draw(shader);

		shader.setMat4("_Model", groundTransform.getModelMatrix());
		ground.drawLoadTex(shader);

		tweenTime += deltaTime;
		if (tweenTime >= 1.25f) { tweenTime = 0.0f; reverse = !reverse; }

		// light setup

		// start with the sun (directional light)
		sun.ambient = vg3o::GetTemperatureColor(sunColor);
		sun.diffuse = vg3o::GetTemperatureColor(sunColor);
		sun.specular = vg3o::GetTemperatureColor(sunColor);

		shader.setVec3("_Sun.direction", sun.direction);
		shader.setFloat("_Sun.strength", sun.strength);
		shader.setVec3("_Sun.ambient", sun.ambient);
		shader.setVec3("_Sun.diffuse", sun.diffuse);
		shader.setVec3("_Sun.specular", sun.specular);

		shader.setInt("_HeadlightsActive", headlightsActive);

		for (int i = 0; i < 10; i++) {
			headlights[i].color = vg3o::GetTemperatureColor(headlightColor);
		}

		for (int i = 0; i < 10; i++) {
			std::string headlightStr = "_Headlights[" + std::to_string(i) + "]";
			shader.setVec3(headlightStr + ".position", headlights[i].position);
			shader.setVec3(headlightStr + ".direction", vg3o::getForwardVector(headlights[i].rotation));
			shader.setVec3(headlightStr + ".color", headlights[i].color);
			shader.setFloat(headlightStr + ".strength", headlights[i].strength);
			shader.setFloat(headlightStr + ".cutoff", cos(headlights[i].cutoff * ew::DEG2RAD));
			shader.setFloat(headlightStr + ".outerCutoff", cos(headlights[i].outerCutoff * ew::DEG2RAD));
			shader.setFloat(headlightStr + ".range", headlights[i].range);
		}

		shader.setInt("_PointLightAmount", activePointLights);

		for (int i = 0; i < activePointLights; i++) {
			pointLights[i].position = ew::Vec3(
				lightPosition * cos(time + i), 
				1.0f, 
				lightPosition * sin(time + i)
			);
			pointLights[i].color = vg3o::GetTemperatureColor(pointLightTemperature[i]);

			shader.setVec3("_PointLights[" + std::to_string(i) + "].position", pointLights[i].position);
			shader.setVec3("_PointLights[" + std::to_string(i) + "].color", pointLights[i].color);
			shader.setFloat("_PointLights[" + std::to_string(i) + "].strength", pointLights[i].strength);

			shader.setVec3("_PointLights[" + std::to_string(i) + "].diffuse", pointLights[i].diffuse);
			shader.setVec3("_PointLights[" + std::to_string(i) + "].specular", pointLights[i].specular);

			shader.setFloat("_PointLights[" + std::to_string(i) + "].radius", pointLights[i].radius);
		}

		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		for (int i = 0; i < activePointLights; i++) {
			lightSourceTransforms[i].position = pointLights[i].position;
			unlitShader.setMat4("_Model", lightSourceTransforms[i].getModelMatrix());
			unlitShader.setVec3("_Color", pointLights[i].color);
			lightSources[i].draw();
		}
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use(); // remove translation from the view matrix so that the skybox looks infinite
		skyboxShader.setMat4("_View", camera.ViewMatrix());
		skyboxShader.setMat4("_Projection", camera.ProjectionMatrix());
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default


		framebuffer.useDefaultBuffer();

		// do the blurring stuff
		bool horizontal = true, first_iteration = true;
		std::vector<unsigned int> colorBuffers = framebuffer.getColorBuffers();
		// how many times to render the blur
		unsigned int amount = 6;
		blurShader.use();
		glActiveTexture(GL_TEXTURE0);
		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			blurShader.setInt("_Horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
			glDisable(GL_DEPTH_TEST);
			framebuffer.draw();
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;

		}
		// swap back to the default frame buffer to render onto the quad
		framebuffer.useDefaultBuffer();

		
		screenShader.use();
		screenShader.setInt("_BloomActive", bloom);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[bufferMode]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
		framebuffer.draw();

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
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Text("Lighting Settings");;
			ImGui::Checkbox("Bloom", &bloom);
			ImGui::SliderInt("Color Buffer", &bufferMode, 0, 1);
			ImGui::SliderInt("Light Amount", &activePointLights, 0, MAX_LIGHTS);
			if (ImGui::CollapsingHeader("Sun Settings")) {
				ImGui::DragFloat3("Direction", &sun.direction.x, 0.01f, -1.0f, 1.0f);
				ImGui::DragFloat("Strength", &sun.strength, 0.01f, 0.0f, 1.0f);
				ImGui::SliderInt("Temperature", &sunColor, 2500, 13500);
			}
			if (ImGui::CollapsingHeader("Moving Light Settings")) {
				ImGui::Indent();
				for (int i = 0; i < activePointLights; i++) {
					std::string str = "Light " + std::to_string(i + 1);
					ImGui::PushID(i);
					if (ImGui::CollapsingHeader(str.c_str())) {
						ImGui::SliderInt("Temperature", &pointLightTemperature[i], 2500, 13500);
						ImGui::SliderFloat("Strength", &pointLights[i].strength, 1.0f, 20.0f);
						ImGui::SliderFloat("Radius", &pointLights[i].radius, 1.0f, 30.0f);
					}
					ImGui::Spacing();
					ImGui::PopID();
				}
				ImGui::Unindent();
			}
			if (ImGui::CollapsingHeader("Car Settings")) {
				ImGui::Checkbox("Headlights", &headlightsActive);
				if (headlightsActive) {
					ImGui::SliderInt("Headlight Temperature", &headlightColor, 2500, 13500);
				}
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