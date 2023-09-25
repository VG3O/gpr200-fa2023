#include <stdio.h>
#include <math.h>

#include <vg3o/shader.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

unsigned int createShader(GLenum shaderType, const char* sourceCode);
unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
unsigned int createVAO(float* vertexData, int numVertices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

vg3o::Vertex vertices[4] = {
	//x   //y  //z	//u  //v
	{ 1.0, 1.0, 0.0, 1.0, 1.0 }, // top right
	{ -1.0, 1.0, 0.0, 0.0, 1.0 }, // top left
	{ -1.0, -1.0, 0.0, 0.0, 0.0 }, // bottom left
	{ 1.0, -1.0, 0.0, 1.0, 0.0 } // bottom right
};

unsigned int indices[6] = {
	0, 1, 2, // triangle 1
	0, 2, 3 // triangle 2
};

bool showImGUIDemoWindow = false;

float SkyColor[4][3] = {
	// day time colors
	// bottom		// top
	{1.0,0.17,0.0}, {0.0, 0.2, 0.3}, // each of these equates to the vec3() constructor
	// night time colors
	// top			// bottom
	{0.85,0.0,0.1}, {0.05,0.05,0.05}
};

float SunColor[2][3] = {
	// base		   // peak
	{0.8,0.0,0.0}, {1,0.5,0.0}
};

float SunRadius = 0.3;
float SunSpeed = 0.7;

float HillColor[2][3] = {
	// bottom      // top
	{0.0,0.0,0.0}, { 0.2,0.05,0.0 }
};

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Coldest Place in the Universe May Actually Be a Sussy Baka ?", NULL, NULL);
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

    unsigned int vao = vg3o::createVAO(vertices, 4, indices, 6);

	vg3o::Shader shaderProgram("assets/vertexShader.vert", "assets/fragmentShader.frag");
	shaderProgram.use();

	glBindVertexArray(vao);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		shaderProgram.setVec2("_Resolution", (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
		shaderProgram.setFloat("_Time", (float)glfwGetTime());
		shaderProgram.setFloat("_SunSpeed", SunSpeed);
		shaderProgram.setFloat("_SunRadius", SunRadius);
		shaderProgram.setVec3Array("_SkyColor", 4, SkyColor);
		shaderProgram.setVec3Array("_SunColor", 2, SunColor);
		shaderProgram.setVec3Array("_HillColor", 2, HillColor);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);

			ImGui::Separator();
			ImGui::Text("Daytime Colors");
			ImGui::ColorEdit3("Top", SkyColor[1]);
			ImGui::ColorEdit3("Bottom", SkyColor[0]);
			
			ImGui::Separator();
			ImGui::Text("Nighttime Colors");
			ImGui::ColorEdit3("Top", SkyColor[3]);
			ImGui::ColorEdit3("Bottom", SkyColor[2]);

			ImGui::Separator();
			ImGui::Text("Sun Settings");
			ImGui::ColorEdit3("Peak Color", SunColor[1]);
			ImGui::ColorEdit3("Base Color", SunColor[0]);
			ImGui::SliderFloat("Radius", &SunRadius, 0.1, 1.0);
			ImGui::SliderFloat("Speed", &SunSpeed, 0.1, 10.0);

			ImGui::Separator();
			ImGui::Text("Hill Colors");
			ImGui::ColorEdit3("Day Color", HillColor[1]);
			ImGui::ColorEdit3("Night Color", HillColor[0]);
			ImGui::Separator();

			ImGui::End();
			if (showImGUIDemoWindow) {
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}

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

