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

	//Create cube
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));

	resetCamera(camera,cameraController);

	vg3o::Light pointLights[MAX_LIGHTS]; 
	vg3o::SpotLight spotLights[MAX_LIGHTS];
	vg3o::DirectionalLight sun;
	sun.ambient = vg3o::GetTemperatureColor(5500);
	sun.diffuse = vg3o::GetTemperatureColor(5500);
	sun.specular = vg3o::GetTemperatureColor(5500);
	sun.direction = ew::Vec3(-1.0f, -1.0f, 0.0f);

	// car lights
	vg3o::SpotLight headlights[8];
	
	headlights[0].position = ew::Vec3(-0.63f, -0.36f, -2.49f);
	headlights[1].position = ew::Vec3(-0.78f, -0.33f, -2.63f);
	headlights[2].position = ew::Vec3(-0.77f, -0.12f, -2.46f);
	headlights[3].position = ew::Vec3(-0.69f, -0.34f, -2.41f);
	headlights[4].position = ew::Vec3(-0.76f, -0.31f, -2.57f);

	headlights[0].rotation = ew::Vec3(-6.0f, -256.0f, 0.0f);
	headlights[1].rotation = ew::Vec3(-4.0f, -271.0f, 0.0f);
	headlights[2].rotation = ew::Vec3(-29.0f, 91.0f, 0.0f);
	headlights[3].rotation = ew::Vec3(-10.0f, 97.0f, 0.0f);
	headlights[4].rotation = ew::Vec3(-13.0f, -280.0f, 0.0f);

	headlights[0].color = vg3o::GetTemperatureColor(3750);
	headlights[1].color = vg3o::GetTemperatureColor(3750);
	headlights[2].color = vg3o::GetTemperatureColor(3750);
	headlights[3].color = vg3o::GetTemperatureColor(3750);
	headlights[4].color = vg3o::GetTemperatureColor(3750);

	headlights[0].strength = 417.0f;
	headlights[1].strength = 304.0f;
	headlights[2].strength = 524.0f;
	headlights[3].strength = 565.0f;
	headlights[4].strength = 1000.0f;

	headlights[0].cutoff = 1.0f;
	headlights[1].cutoff = 1.0f;
	headlights[2].cutoff = 1.0f;
	headlights[3].cutoff = 1.0f;
	headlights[4].cutoff = 1.0f;
	
	headlights[0].outerCutoff = 10.0f;
	headlights[1].outerCutoff = 9.0f;
	headlights[2].outerCutoff = 11.5f;
	headlights[3].outerCutoff = 16.0f;
	headlights[4].outerCutoff = 9.0f;

	headlights[0].range = 1.0f;
	headlights[1].range = 1.0f;
	headlights[2].range = 1.0f;
	headlights[3].range = 1.0f;
	headlights[4].range = 1.0f;


	spotLights[0].position = ew::Vec3(2.0f, 1.5f, 0.0f);
	spotLights[0].rotation = ew::Vec3(0.0f, 0.0f, 0.0f);
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

	ew::Vec3 sourceOffset = ew::Vec3(90.0f, 0.0f, 90.0f);
	for (int i = 0; i < MAX_LIGHTS; i++) {
		lightSources[i] = ew::Mesh(ew::createSphere(0.1f, 32));
		pointLights[i].color = ew::Vec3(1, 1, 1);
	}
	for (int i = 0; i < MAX_LIGHTS; i++) {
		spotLightSources[i] = ew::Mesh(ew::createCylinder(0.1f, 0.25f, 32));
		spotLights[i].color = ew::Vec3(1, 1, 1);
	}
	//------------------------
	// skybox setup
	float exposure = 2.0f;
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
	// set up the ground texture (this is the only object in the scene that will use normal mapping)
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
	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_WIDTH, 0, GL_RGB, GL_FLOAT, NULL);
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
	// cutscene setup

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
		shader.setVec3("_Sun.direction", sun.direction);
		shader.setFloat("_Sun.strength", sun.strength);
		shader.setVec3("_Sun.ambient", sun.ambient);
		shader.setVec3("_Sun.diffuse", sun.diffuse);
		shader.setVec3("_Sun.specular", sun.specular);

		for (int i = 0; i < 5; i++) {
			std::string headlightStr = "_Headlights[" + std::to_string(i) + "]";
			shader.setVec3(headlightStr + ".position", headlights[i].position);
			shader.setVec3(headlightStr + ".direction", vg3o::getForwardVector(headlights[i].rotation));
			shader.setVec3(headlightStr + ".color", headlights[i].color);
			shader.setFloat(headlightStr + ".strength", headlights[i].strength);
			shader.setFloat(headlightStr + ".cutoff", cos(headlights[i].cutoff * ew::DEG2RAD));
			shader.setFloat(headlightStr + ".outerCutoff", cos(headlights[i].outerCutoff* ew::DEG2RAD));
			shader.setFloat(headlightStr + ".range", headlights[i].range);
		}

		shader.setInt("_PointLightAmount", activePointLights);
		for (int i = 0; i < activePointLights; i++) {
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
		for (int i = 0; i < activeSpotLights; i++) {
			spotLightSourceTransforms[i].position = spotLights[i].position;
			spotLightSourceTransforms[i].rotation = sourceOffset + -spotLights[i].rotation;
			unlitShader.setMat4("_Model", spotLightSourceTransforms[i].getModelMatrix());
			unlitShader.setVec3("_Color", spotLights[i].color);
			spotLightSources[i].draw();
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
		unsigned int amount = 10;
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
		framebuffer.useDefaultBuffer();

		
		screenShader.use();
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
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
			ImGui::DragFloat("Exposure", &exposure, 0.05f, 1.0f, 10.0f);
			ImGui::PushID(0);
			if (ImGui::CollapsingHeader("Sun Settings")) {
				ImGui::DragFloat3("Position", &sun.direction.x, 0.01f, -1.0f, 1.0f);
				ImGui::DragFloat("Strength", &sun.strength, 0.01f, 0.0f, 1.0f);
			}
			ImGui::PopID();

			ImGui::SliderInt("Point Light Amount", &activePointLights, 0, MAX_LIGHTS);
			ImGui::SliderInt("Spot Light Amount", &activeSpotLights, 0, MAX_LIGHTS);
			for (int i = 0; i < activePointLights; i++) {
				std::string str = "Point Light " + std::to_string(i);
				ImGui::PushID(i + 1);
				if (ImGui::CollapsingHeader(str.c_str())) {
					ImGui::DragFloat3("Position", &pointLights[i].position.x, 0.01f);
					ImGui::DragFloat3("Color", &pointLights[i].color.x, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Strength", &pointLights[i].strength, 0.01f, 0.0f, 1000.0f);
					ImGui::DragFloat("Radius", &pointLights[i].radius, 0.05f, 0.05f, 10.0f);
				}
				ImGui::PopID();
			}
			for (int i = 0; i < 5; i++) {
				std::string str = "Headlight " + std::to_string(i);
				ImGui::PushID(i + 1 + activePointLights);
				if (ImGui::CollapsingHeader(str.c_str())) {
					ImGui::DragFloat3("Position", &headlights[i].position.x, 0.01f);
					ImGui::DragFloat3("Rotation", &headlights[i].rotation.x, 1.0f);
					ImGui::DragFloat("Strength", &headlights[i].strength, 1.0f, 0.0f, 1000.0f);
					ImGui::DragFloat("Range", &headlights[i].range, 0.05f, 0.05f, 15.0f);
					ImGui::DragFloat("Inner Cutoff", &headlights[i].cutoff, 0.5f, 1.0f, 90.0f);
					ImGui::DragFloat("Outer Cutoff", &headlights[i].outerCutoff, 0.5f, 1.0f, 90.0f);

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