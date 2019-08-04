#include <iostream>

#define GLEW_STATIC

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "Thirdparty/glm/glm/vec3.hpp"
#include "Thirdparty/glm/glm/vec4.hpp"

#include <vector>


#define REQUIRED(stm)  

#define GLSL(version, A) "#version " #version "\n" #A
const char* vertexShaderCode = GLSL(120,

	attribute vec4 position;
attribute vec4 color;

varying vec4 dstColor;
void main()
{
	dstColor = color;
	gl_Position = position;
}

);

const char* fragmentShaderCode = GLSL(120,
	varying vec4 dstColor;

void main()
{
	gl_FragColor = dstColor;
}

);



/*-----------------------------------------------------------------------------
	*  FUNCION TO CHECK FOR SHADER COMPILER ERRORS
	*-----------------------------------------------------------------------------*/
void compilerCheck(GLuint ID) {
	GLint comp;
	glGetShaderiv(ID, GL_COMPILE_STATUS, &comp);

	if (comp == GL_FALSE) {
		std::cout << "Shader Compilation FAILED" << std::endl;
		GLchar messages[256];
		glGetShaderInfoLog(ID, sizeof(messages), 0, &messages[0]);
		std::cout << messages;
	}
}

void linkCheck(GLuint ID) {
	GLint linkStatus, validateStatus;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_FALSE) {
		std::cout << "Shader Linking FAILED" << std::endl;
		GLchar messages[256];
		glGetProgramInfoLog(ID, sizeof(messages), 0, &messages[0]);
		std::cout << messages;
	}

	glValidateProgram(ID);
	glGetProgramiv(ID, GL_VALIDATE_STATUS, &validateStatus);

	std::cout << "Link: " << linkStatus << "  Validate: " << validateStatus << std::endl;
	if (linkStatus == GL_FALSE) {
		std::cout << "Shader Validation FAILED" << std::endl;
		GLchar messages[256];
		glGetProgramInfoLog(ID, sizeof(messages), 0, &messages[0]);
		std::cout << messages;
	}
}

/*------------------------------------------------------
*    Start Main Function
*------------------------------------------------------*/

struct  Vertex
{
	Vertex() = default;
	Vertex(float _x, float _y, float _z,
		float _r, float _g, float _b)
	{
		position.x = _x;
		position.y = _y;
		position.z = _z;

		color.r = _r;
		color.g = _g;
		color.b = _b;
		color.a = 1.0f;

		REQUIRED(color.a == 1.0f);
	}
	glm::vec3 position;
	glm::vec4 color;
};

std::vector<Vertex> model;

void initModel()
{
	model.emplace_back(Vertex(-1, -.5, 0,
		0.3f, 0.0f, 0.0f));
	model.emplace_back(Vertex(0, 1, 0,
		0.0f, 0.3f, 0.0f));
	model.emplace_back(Vertex(1, -.5, 0,
		0.0f, 0.0f, 0.3f));
}

GLuint sID;
GLuint vID;
GLuint fID;
GLuint positionID;
GLuint colorID;

void initShader()
{
	// open gl create a shader program
	sID = glCreateProgram();
	vID = glCreateShader(GL_VERTEX_SHADER);
	fID = glCreateShader(GL_FRAGMENT_SHADER);

	// Load shader source code
	glShaderSource(vID, 1, &vertexShaderCode, nullptr);
	glShaderSource(fID, 1, &fragmentShaderCode, nullptr);

	// compile
	glCompileShader(vID);
	glCompileShader(fID);

	compilerCheck(vID);
	compilerCheck(fID);

	glAttachShader(sID, vID);
	glAttachShader(sID, fID);

	glLinkProgram(sID);

	linkCheck(sID);

	glUseProgram(sID);

	positionID = glGetAttribLocation(sID, "position");
	colorID = glGetAttribLocation(sID, "color");

	glUseProgram(0);
}

GLuint arrayID;

void createBuffer()
{

	GLuint bufferID;

	glGenVertexArrays(1, &arrayID);
	glBindVertexArray(arrayID);

	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	// send the vertex data to GPU memory
	glBufferData(GL_ARRAY_BUFFER, model.size() * sizeof(Vertex), model.data(), GL_STATIC_DRAW);

	//tell a shader program to get access to the data.
	glEnableVertexAttribArray(positionID);
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(colorID);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3)); //start at 12(vec3size) offset

	// unbind vertex array
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawArray()
{
	glUseProgram(sID);
	glBindVertexArray(arrayID);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glUseProgram(0);

}




float x = 0.0f;
float y = 0.0f;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE) {
		std::cout << "Key ESC is pressed" << std::endl;
		exit(0);
	}
	
	
	 
}


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		std::cout << "GLEW is not working" << std::endl;
		return -1;
	}

	if (!GLEW_VERSION_2_1)
	{
		std::cout << "OPENGL2.1 does not supported." << std::endl;
		return -1;
	}

	const GLubyte* glVersion = glGetString(GL_VERSION);
	std::cout << "Graphics driver : " << glVersion << std::endl;

	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	std::cout << "GLSL version : " << glslVersion << std::endl;

	if (GLEW_ARB_vertex_array_object)
	{
		std::cout << "Vertex arrays is supported" << std::endl;
	}

	/* Set key callback*/
	glfwSetKeyCallback(window, key_callback);

	initModel();
	initShader();
	createBuffer();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glClearColor(0.5f, 0.8f, 0.8f, 0.5f);

		drawArray();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
