#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

float vertices[21] = {
	//x   //y  //z   //r  //g  //b  //a
	-0.5, -0.5, 0.0, 1.0, 0.55, 0.15, 1.0, //Bottom left
	 0.5, -0.5, 0.0, 0.3, 0.85, 1, 1.0, //Bottom right
	 0.0,  0.5, 0.0, 0.7, 0.125, 0.6, 1.0  //Top center
};


const char* vertexShaderSRC = R"(
	#version 450
	layout(location = 0) in vec3 vPos;
	layout(location = 1) in vec4 vColor;
	out vec4 Color;
	uniform float _Time;
	void main() {
		Color = vColor;
		vec3 offset = vec3(0,sin(vPos.x + (_Time),0)*0.5;
		gl_Position = vec4(vPos + offset,1.0);
	}
)";
const char* fragmentShaderSRC = R"(
	#version 450
	out vec4 FragColor;
	in vec4 Color;
	uniform float _Time;
	void main(){
		FragColor = Color * abs(sin(_Time)*1.5);
	}
)";


unsigned int createVAO(float* vertexData, int numVertices)
{
	int stride = 7; // how many attributes per vertex
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * stride * numVertices, vertexData, GL_STATIC_DRAW);

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//Tell vao to pull vertex data from vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//Define position attribute (3 floats)
	// first paramter is the location int, second one is the amount of data being sent through, third one is the size allocation
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (const void*)0);
	glEnableVertexAttribArray(0);

	//Define color attribute (4 floats)
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (const void*)(sizeof(float)*3));
	glEnableVertexAttribArray(1);
	return vao;
};

unsigned int createShader(GLenum shaderType, const char* source)
{
	unsigned int shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) 
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("Failed to compile shader: %s", infoLog);
	}
	return shader;
};

unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	unsigned int vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Failed to link shader program: %s", infoLog);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

int main() {

	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}
	// Create shader program and vertex buffer array
	unsigned int shader = createShaderProgram(vertexShaderSRC, fragmentShaderSRC);
	unsigned int vao = createVAO(vertices, 3);

	// Render Loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader);
		glBindVertexArray(vao);

		// Time value update
		float time = (float)glfwGetTime();

		// get uniform location inside the shader program
		// as long as it references the correct name, i.e "_Time" in this case
		// the vertex and fragment shaders will see this value
		// and will be able to both read from it, as long as it is
		// denoted using "uniform typename _UniformName"

		int timeLocation = glGetUniformLocation(shader, "_Time");
		
		// set uniform value at the uniform location
		glUniform1f(timeLocation, time);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
	}

	printf("Shutting down...");
}
