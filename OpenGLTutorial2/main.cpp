#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <cerrno>
using std::string;
using glm::mat4;
using glm::vec4;
using glm::vec3;
//---------------------------------------------------
//				FUNCTION PROTOTYPES
//---------------------------------------------------
bool Init();
bool InitGL();

//load shaders
//load from file
string LoadSourceFromFile(const string& Filename);
GLuint LoadShader(const string& Filename, GLenum ShaderType);

//log for errors
void PrintShaderLog(GLuint Shader);
void PrintProgramLog(GLuint Program);

//render the cube
void DrawScene();

//error callback for glfw
static void error_callback(int error, const char* description);

//close the program down
void Shutdown();

//------------------------------------------------------------
//			END FUNCTION PROTOTYPES
//------------------------------------------------------------

//--------------------------------------------------
//				GLOBALS
//--------------------------------------------------
const GLint SCREEN_WIDTH = 640;
const GLint SCREEN_HEIGHT = 480;

GLFWwindow* Window = NULL;


//GL program handles
GLuint ProgramID = 0;
GLuint VBO = 0;
GLuint IBO = 0;
GLint VertexPosition = -1;
GLint VertexColor = -1;

vec3 LightPosition = vec3(0.f, 10.f, 0.f);
vec3 LightIntensity = vec3(0.8f, 0.8f, 0.8f);

vec4 CubePosition = vec4(0.0f, -10.f, 0.0f, 1.0f);

mat4 Model;
mat4 View;
mat4 Projection;

string VertexShaderFile = "Shader.vs";
string FragmentShaderFile = "Shader.fs";

//---------------------------------------------------
//					END GLOBALS
//---------------------------------------------------

int main(int argc, char** argv)
{
	if (!Init())
	{
		std::cout << "Initialization Failure!" << std::endl;
		exit(EXIT_FAILURE);
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

bool Init()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		std::cout << "Unable to initialize GLFW" << std::endl;
		return false;
	}

	Window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Tutorial 2", NULL, NULL);
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

bool InitGL()
{
	ProgramID = glCreateProgram();
	GLuint vertexShader = LoadShader(VertexShaderFile, GL_VERTEX_SHADER);
	if (vertexShader == -1)
	{
		return false;
	}
	glAttachShader(ProgramID, vertexShader);

	GLuint fragmentShader = LoadShader(FragmentShaderFile, GL_FRAGMENT_SHADER);
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
		std::cout << "Unable to locate VertexPosition in GLSL program." << std::endl;
		return false;
	}

	VertexColor = glGetAttribLocation(ProgramID, "VertexColor");
	if (VertexColor == -1)
	{
		std::cout << "Unable to locate VertexColor in GLSL program." << std::endl;
		return false;
	}

	//initialize clear color to black
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	//cube vertices
	//position			color
	GLfloat cube_vertices[]=
	{
		1.f, 1.f, 1.f,			1.0f, 0.0f, 0.0f,	//0
		-1.f, 1.f, 1.f,			0.0f, 1.0f, 0.0f,	//1
		-1.f, 1.f, -1.f,		0.0f, 0.0f, 1.0f,	//2
		1.f, 1.f, -1.f,			1.0f, 1.0f, 1.0f,	//3
		1.f, -1.f, 1.f,			1.0f, 1.0f,	0.0f,	//4
		-1.f, -1.f, 1.f,		1.0f, 1.0f, 1.0f,	//5
		-1.f, -1.f, -1.f,		0.0f, 1.0f, 1.0f,	//6
		1.f, -1.f, -1.f,		1.0f, 0.0f, 1.0f	//7
	};


	//indices for the cube
	// 6 faces , 2 triangles per face, 12 triangles 
	GLuint indices[] = 
	{
		0, 1, 3, //top 1
		3, 1, 2, //top 2
		2, 6, 7, //front 1
		7, 3, 2, //front 2
		7, 6, 5, //bottom 1
		5, 4, 7, //bottom 2
		5, 1, 4, //back 1
		4, 1, 0, //back 2
		4, 3, 7, //right 1
		3, 4, 0, //right 2
		5, 6, 2, //left 1
		5, 1, 2  //left 2
	};

	//create VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	//create IBO
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//setup camera
	View = glm::lookAt(vec3(2.5f, 2.f, 3.f),
						vec3(0.0f, 0.0f, 0.f),
						vec3(0.0f, 1.0f, 0.0f));

	Projection = glm::perspective(45.f,
								(float)SCREEN_WIDTH / SCREEN_HEIGHT, 
								0.1f,
								1000.f);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void DrawScene()
{
	//clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ProgramID);

	//set the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(VertexPosition);
	glVertexAttribPointer(VertexPosition, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(VertexColor);
	glVertexAttribPointer(VertexColor, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	
	//model position
	Model = mat4(1.0f);
	glm::mat4  MVP = Projection * View * Model;

	GLuint uMVP = glGetUniformLocation(ProgramID, "MVP");
	glUniformMatrix4fv(uMVP, 1, GL_FALSE, &MVP[0][0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, 3*12, GL_UNSIGNED_INT, 0);

	//disable vertex position color
	glDisableVertexAttribArray(VertexPosition);
	glDisableVertexAttribArray(VertexColor);
	glUseProgram(NULL);
}

GLuint LoadShader(const string& Filename, GLenum ShaderType)
{
	GLuint shader = glCreateShader(ShaderType);
	string shaderSource = LoadSourceFromFile(Filename);
	const char* source = shaderSource.c_str();
	glShaderSource(shader, 1, &source, NULL);
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

string LoadSourceFromFile(const string& filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string source;
		in.seekg(0, std::ios::end);
		source.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&source[0], source.size());
		in.close();
		return source;
	}
	throw(errno);
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

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void Shutdown()
{
	glDeleteProgram(ProgramID);
	glfwDestroyWindow(Window);
	Window = NULL;
	glfwTerminate();
}