//-----------------------------------------------------------------------
//  OpenGL Tutorial 1 
//  Chris Minda
//  The tutorial associated with this source file can be found at
//	ChrisMinda.com
//-----------------------------------------------------------------------
#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

//------------------------------------------------
//Function Prototypes
//------------------------------------------------
bool Init();
bool InitGL();
void DrawScene();
GLuint LoadShader(const GLchar* source[], GLenum ShaderType);
void PrintShaderLog(GLuint shader);
void PrintProgramLog(GLuint program);
void Shutdown();
static void error_callback(int erro, const char* description);

//-----------------------------------------------
//GLOBALS
//-----------------------------------------------
//Shaders
//Vertex Shader
const GLchar* VertexShaderSource[] =
{
	"#version 430\n  \
	in vec2 VertexPosition;\
	void main() \
	{\
		gl_Position= vec4(VertexPosition, 0, 1); \
	}"
};

//Fragment Shader
const GLchar* FragmentShaderSource[] =
{
	"#version 430\n \
	out vec4 FragColor; \
	void main() \
	{ \
		FragColor = vec4(1.0, 1.0, 1.0, 1.0); \
	} " 
};

//window dimensions
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//pointer the window being rendered to
GLFWwindow* Window = NULL;


//GL program handles
GLuint ProgramID = 0;
GLuint VBO = 0;
GLuint IBO = 0;
GLint VertexPosition= -1;

//------------------------------------------------------
//GLOBALS END
//------------------------------------------------------



int main(int argc, char** argv)
{
	if (!Init())
	{
		std::cout << "Initialization Failed!" << std::endl;
		exit(EXIT_FAILURE);
		return -1;
	}


	while (!glfwWindowShouldClose(Window))
	{
		
		DrawScene();
		glfwSwapBuffers(Window);
		glfwPollEvents();
	}

	Shutdown();

	exit(EXIT_SUCCESS);
}

//Initialize all components
bool Init()
{
	glfwSetErrorCallback(error_callback);

	//initialize GLFW
	if (!glfwInit())
	{
		return false;
	}


	//create the window
	Window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Tutorial 1", NULL, NULL);
	if (!Window)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(Window);
	glfwSwapInterval(1);

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		std::cout << "Unable to initialize GLEW. Error: " << glewGetErrorString(glewError) << std::endl;
		return false;
	}

	if (!InitGL())
	{
		std::cout << "Unable to initialize OpenGL shaders and program." << std::endl;
		return false;
	}


	return true;
}

//Prepare OpenGL 
bool InitGL()
{
	ProgramID = glCreateProgram();

	GLuint vertexShader = LoadShader(VertexShaderSource, GL_VERTEX_SHADER);
	if (vertexShader == -1)
	{
		return false;
	}
	glAttachShader(ProgramID, vertexShader);

	GLuint fragmentShader = LoadShader(FragmentShaderSource, GL_FRAGMENT_SHADER);
	if (fragmentShader == -1)
	{
		return false;
	}
	glAttachShader(ProgramID, fragmentShader);


	glLinkProgram(ProgramID);

	GLint programLinked = GL_TRUE;
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &programLinked);
	if (programLinked != GL_TRUE)
	{
		std::cout << "Unable to link program." << std::endl;
		PrintProgramLog(ProgramID);
		return false;
	}

	VertexPosition = glGetAttribLocation(ProgramID, "VertexPosition");
	if (VertexPosition == -1)
	{
		std::cout << "Unable to locate VertexPosition in glsl program" << std::endl;
		return false;
	}

	//initialize clear color to blue
	glClearColor(0.f, 0.f, 1.f, 1.f);

	//VBO data
	GLfloat vertices[] =
	{
		0.f, .5f,
		-.5f, -.5f,
		.5f, -.5f
	};

	//IBO data
	GLuint indices[] = { 0, 1, 2 };

	//Create VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	//Create IBO
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(GLuint), indices, GL_STATIC_DRAW);

	return true;
}

void DrawScene()
{
	//clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(ProgramID);

	glEnableVertexAttribArray(VertexPosition);

	//set vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(VertexPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

	//disable vertex position
	glDisableVertexAttribArray(VertexPosition);
	glUseProgram(NULL);
}

//loads shader source and compiles the shader
GLuint LoadShader(const GLchar* source[], GLenum ShaderType)
{
	//create shader
	GLuint shader = glCreateShader(ShaderType);


	glShaderSource(shader, 1, source, NULL);

	glCompileShader(shader);

	GLint shaderCompiled = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
	if (shaderCompiled != GL_TRUE)
	{
		std::cout << "Unable to compile shader." << std::endl;
		PrintShaderLog(shader);
		return -1;
	}

	return shader;
}

//prints info about the state of the shader
void PrintShaderLog(GLuint shader)
{
	//only shaders should be passed to this function
	if (!glIsShader(shader))
	{
		std::cout << "Cannot print shader log for non shader object" << std::endl;
		return;
	}

	int infoLogLength = 0;
	int maxLength = 0;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	//allocate the string
	char* infoLog = new char[maxLength];

	//get the info log
	glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
	if (infoLogLength > 0)
	{
		//print the log
		std::cout << infoLog;
	}

	delete[] infoLog;
}

//prints info about the state of a GLSL program
void PrintProgramLog(GLuint program)
{
	//only programs should be passed to this functio
	if (glIsProgram(program))
	{
		std::cout << "Cannot print program log for a no program object" << std::endl;
		return;
	}
	int infoLogLength = 0;
	int maxLength = 0;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

	//allocate the string
	char* infoLog = new char[maxLength];

	//get the info log
	glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
	if (infoLogLength > 0)
	{
		//print the log
		std::cout << infoLog;
	}

	delete[] infoLog;

}

//closes program elegantly
void Shutdown()
{
	glDeleteProgram(ProgramID);

	glfwDestroyWindow(Window);
	Window = NULL;

	glfwTerminate();
}

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}