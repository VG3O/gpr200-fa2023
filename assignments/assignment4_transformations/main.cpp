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
#include <ew/ewMath/vec3.h>
#include <ew/procGen.h>

#include <vg3o/transformations.h>
#include <vg3o/shader.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

//Square aspect ratio for now. We will account for this with projection later.
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;

const int CUBE_COUNT = 4;

using namespace std;

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


	vg3o::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	vg3o::Transform transformations[CUBE_COUNT];
	bool animScale[CUBE_COUNT];
	bool animPos[CUBE_COUNT];
	bool animRot[CUBE_COUNT];

	for (int i = 0; i < CUBE_COUNT; i++)
	{
		animScale[i] = true;
		animPos[i] = true;
		animRot[i] = true;
	}

	// Random setup
	std::random_device rd;
	std::mt19937 random(rd());
	std::uniform_int_distribution<int> direction(1, 2);
	std::uniform_real_distribution<float> distPos(0, 0.5);
	std::uniform_real_distribution<float> distRot(0, 360);
	std::uniform_real_distribution<float> distScale(0, 2);

	// Setup start values
	int DIRECTIONS[CUBE_COUNT][2];
	float START_POS[CUBE_COUNT][2];
	float START_ROT[CUBE_COUNT][3];
	float START_SCALE[CUBE_COUNT][3];

	for (int i = 0; i < CUBE_COUNT; i++)
	{
		DIRECTIONS[i][0] = direction(random);
		DIRECTIONS[i][1] = direction(random);

		START_POS[i][0] = distPos(random);
		START_POS[i][1] = distPos(random);

		START_ROT[i][0] = distRot(random);
		START_ROT[i][1] = distRot(random);
		START_ROT[i][2] = distRot(random);

		START_SCALE[i][0] = distScale(random);
		START_SCALE[i][1] = distScale(random);
		START_SCALE[i][2] = distScale(random);

		transformations[i].position = ew::Vec3(START_POS[i][0], START_POS[i][1], 0.0f);
		transformations[i].scale = ew::Vec3(START_SCALE[i][0], START_SCALE[i][1], START_SCALE[i][2]);
		transformations[i].rotation = ew::Vec3(START_ROT[i][0], START_ROT[i][1], START_ROT[i][2]);
	}

	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float Time = (float)glfwGetTime();

		//Set uniforms
		shader.use();

		for (int i = 0; i < CUBE_COUNT; i++)
		{
			shader.setMat4("_Model", transformations[i].getModelMatrix());
			cubeMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Transform");

			for (int i = 0; i < CUBE_COUNT; i++)
			{
				ImGui::PushID(i);
				std::string newstr = "Cube " + to_string(i+1);
				if (ImGui::CollapsingHeader(newstr.c_str()))
				{
					ImGui::Checkbox("Animate Position", &animPos[i]);
					if (!animPos[i]) { ImGui::DragFloat3("Position", &transformations[i].position.x, 0.05f); }
					ImGui::Checkbox("Animate Scale", &animScale[i]);
					if (!animScale[i]) { ImGui::DragFloat3("Scale", &transformations[i].scale.x, 0.05f); }
					ImGui::Checkbox("Animate Rotation", &animRot[i]);
					if (!animRot[i]) { ImGui::DragFloat3("Rotation", &transformations[i].rotation.x, 0.05f); }
				}
				ImGui::PopID();
				if (animPos[i]) 
				{
					switch (DIRECTIONS[i][0])
					{
					case 1:
						if (DIRECTIONS[i][1] == 1)
						{
							transformations[i].position.x = START_POS[i][0] + sin(Time);
							transformations[i].position.y = START_POS[i][1] + sin(Time);
						}
						else
						{
							transformations[i].position.x = START_POS[i][0] + sin(Time);
							transformations[i].position.y = START_POS[i][1] - sin(Time);
						}
						break;
					case 2:
						if (DIRECTIONS[i][1] == 1)
						{
							transformations[i].position.x = START_POS[i][0] - cos(Time);
							transformations[i].position.y = START_POS[i][1] + sin(Time);
						}
						else
						{
							transformations[i].position.x = START_POS[i][0] + cos(Time);
							transformations[i].position.y = START_POS[i][1] - sin(Time);
						}
						break;
					}
				}
				if (animScale[i])
				{
					switch (DIRECTIONS[i][0])
					{
					case 1:
						if (DIRECTIONS[i][1] == 1)
						{
							transformations[i].scale.x = START_SCALE[i][0] - sin(Time);
							transformations[i].scale.y = START_SCALE[i][1] + sin(Time);
							transformations[i].scale.z = START_SCALE[i][2] + cos(Time);
						}
						else
						{
							transformations[i].scale.x = START_SCALE[i][0] + cos(Time);
							transformations[i].scale.y = START_SCALE[i][1] - cos(Time);
							transformations[i].scale.z = START_SCALE[i][2] + sin(Time);
						}
						break;
					case 2:
						if (DIRECTIONS[i][1] == 1)
						{
							transformations[i].scale.x = START_SCALE[i][0] + cos(Time);
							transformations[i].scale.y = START_SCALE[i][1] + sin(Time);
							transformations[i].scale.z = START_SCALE[i][2] - cos(Time);
						}
						else
						{
							transformations[i].scale.x = START_SCALE[i][0] - cos(Time);
							transformations[i].scale.y = START_SCALE[i][1] + sin(Time);
							transformations[i].scale.z = START_SCALE[i][2] - sin(Time);
						}
						break;
					}
				}
				if (animRot[i])
				{
					switch (DIRECTIONS[i][0])
					{
					case 1:
						if (DIRECTIONS[i][1] == 1)
						{
							transformations[i].rotation.x += 0.01f;
							transformations[i].rotation.y += 0.01f;
							transformations[i].rotation.z -= 0.01f;
						}
						else
						{
							transformations[i].rotation.x += 0.01f;
							transformations[i].rotation.y -= 0.01f;
							transformations[i].rotation.z += 0.01f;
						}
						break;
					case 2:
						if (DIRECTIONS[i][1] == 1)
						{
							transformations[i].rotation.x -= 0.01f;
							transformations[i].rotation.y += 0.01f;
							transformations[i].rotation.z += 0.01f;
						}
						else
						{
							transformations[i].rotation.x -= 0.01f;
							transformations[i].rotation.y += 0.01f;
							transformations[i].rotation.z -= 0.01f;
						}
						break;
					}
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
}

