#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cerrno>

using std::vector;
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

//render the model
void DrawScene();

//error callback for glfw
static void error_callback(int error, const char* description);

//close the program down
void Shutdown();


//load object file
void load_obj(const char* filename, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<GLushort> &elements);

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
GLuint VAO = 0;
GLuint VBO_Vertices = 0;
GLuint VBO_Normals = 0;
GLuint IBO = 0;
GLint VertexPosition = -1;
GLint VertexNormal = -1;

vec4 LightPosition = vec4(0.f, 1.f, 1.5f, 1.f);
vec3 DiffuseLightIntensity = vec3(1.f, 1.f, 1.f);
vec3 AmbientLightIntensity = vec3(0.4f, 0.4f, 0.4f);
vec3 SpecularLightIntensity = vec3(1.f, 1.f, 1.f);

vec3 DiffuseReflectivity = vec3(.94f, .84f, .74f);
vec3 AmbientReflectivity = vec3(.94f, .84f, .74f);
vec3 SpecularReflectivity = vec3(.8f, .8f, .8f);
float Shininess = 100.f;

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

	Window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Tutorial 3", NULL, NULL);
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

	glBindAttribLocation(ProgramID, 0, "VertexPosition");
	glBindAttribLocation(ProgramID, 1, "VertexNormal");

	GLint programLinked = GL_TRUE;
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &programLinked);
	if (programLinked != GL_TRUE)
	{
		std::cout << "Unable to link program." << std::endl;
		PrintProgramLog(ProgramID);
		return false;
	}



	//initialize clear color to grey
	glClearColor(.5f, .5f, .5f, 1.0f);

	//load the model
	vector<vec4> monkey_vertices;
	vector<vec3> monkey_normals;
	vector<GLushort> monkey_elements;

	load_obj("torus.obj", monkey_vertices, monkey_normals, monkey_elements);


	//create VBO_Vertices
	glGenBuffers(1, &VBO_Vertices);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertices);
	glBufferData(GL_ARRAY_BUFFER, monkey_vertices.size() * sizeof(vec4), &monkey_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &VBO_Normals);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Normals);
	glBufferData(GL_ARRAY_BUFFER, monkey_normals.size() * sizeof(vec3), &monkey_normals[0], GL_STATIC_DRAW);

	//create IBO
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, monkey_elements.size() * sizeof(GLushort), &monkey_elements[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//setup camera
	View = glm::lookAt(vec3(0.f, 0.f, 2.5f),
		vec3(0.0f, 0.0f, -1.f),
		vec3(0.0f, 1.0f, 0.0f));

	Projection = glm::perspective(45.f,
		(float)SCREEN_WIDTH / SCREEN_HEIGHT,
		0.1f,
		100.f);
	glEnable(GL_DEPTH_TEST);

	LightPosition = View * LightPosition;



	return true;
}

void DrawScene()
{
	//clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ProgramID);

	//model position
	Model = mat4(1.0f);

	Model = glm::rotate(Model, 180.f, vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 MV = View*Model;
	glm::mat3 Normal = glm::mat3(vec3(MV[0]), vec3(MV[1]), vec3(MV[2]));
	glm::mat4  MVP = Projection * View * Model;

	GLuint uMVP = glGetUniformLocation(ProgramID, "MVP");
	glUniformMatrix4fv(uMVP, 1, GL_FALSE, &MVP[0][0]);

	GLuint uProjectionMatrix = glGetUniformLocation(ProgramID, "ProjectionMatrix");
	glUniformMatrix4fv(uProjectionMatrix, 1, GL_FALSE, &Projection[0][0]);

	GLuint uModelViewMatrix = glGetUniformLocation(ProgramID, "ModelViewMatrix");
	glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, &MV[0][0]);

	GLuint uNormalMatrix = glGetUniformLocation(ProgramID, "NormalMatrix");
	glUniformMatrix3fv(uNormalMatrix, 1, GL_FALSE, &Normal[0][0]);

	GLuint uLightPosition = glGetUniformLocation(ProgramID, "Light.Position");
	glUniform4f(uLightPosition, LightPosition.x, LightPosition.y, LightPosition.z, LightPosition.w);

	GLuint uALightIntensity = glGetUniformLocation(ProgramID, "Light.La");
	glUniform3f(uALightIntensity, AmbientLightIntensity.x, AmbientLightIntensity.y, AmbientLightIntensity.z);

	GLuint uAmbientReflectivity = glGetUniformLocation(ProgramID, "Material.Ka");
	glUniform3f(uAmbientReflectivity, AmbientReflectivity.r, AmbientReflectivity.g, AmbientReflectivity.b);

	GLuint uDLightIntensity = glGetUniformLocation(ProgramID, "Light.Ld");
	glUniform3f(uDLightIntensity, DiffuseLightIntensity.x, DiffuseLightIntensity.y, DiffuseLightIntensity.z);

	GLuint uDiffuse = glGetUniformLocation(ProgramID, "Material.Kd");
	glUniform3f(uDiffuse, DiffuseReflectivity.r, DiffuseReflectivity.g, DiffuseReflectivity.b);

	GLuint uSLightIntensity = glGetUniformLocation(ProgramID, "Light.Ls");
	glUniform3f(uSLightIntensity, SpecularLightIntensity.x, SpecularLightIntensity.y, SpecularLightIntensity.z);

	GLuint uSpecularReflectivity = glGetUniformLocation(ProgramID, "Material.Ks");
	glUniform3f(uSpecularReflectivity, SpecularReflectivity.r, SpecularReflectivity.g, SpecularReflectivity.b);

	GLuint uShininess = glGetUniformLocation(ProgramID, "Material.Shininess");
	glUniform1f(uShininess, Shininess);

	glBindVertexArray(VAO);
	//set the vertex data
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertices);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Normals);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//disable vertex position color
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
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

void load_obj(const char* filename, vector<vec4> &vertices, vector<vec3> &normals, vector<GLushort> &elements)
{
	std::ifstream in(filename, std::ios::in);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);
	}

	string line;
	while (getline(in, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			std::istringstream s(line.substr(2));
			vec4 v;
			s >> v.x;
			s >> v.y;
			s >> v.z;
			v.w = 1.0f;
			vertices.push_back(v);
		}
		else if (line.substr(0, 2) == "f ")
		{
			std::istringstream s(line.substr(2));
			GLushort a, b, c;
			s >> a;
			s >> b;
			s >> c;
			a--;
			b--;
			c--;
			elements.push_back(a);
			elements.push_back(b);
			elements.push_back(c);
		}
		else if (line[0] == '#')
		{
			/*Ignore, this is a comment*/
		}
		else
		{
			/*Ignore everything else*/
		}
	}
	normals.resize(vertices.size(), vec3(0.f, 0.f, 0.f));
	for (int i = 0; i < elements.size(); i += 3)
	{
		GLushort ia = elements[i];
		GLushort ib = elements[i + 1];
		GLushort ic = elements[i + 2];

		vec3 normal = glm::normalize(glm::cross(
			vec3(vertices[ib]) - vec3(vertices[ia]),
			vec3(vertices[ic]) - vec3(vertices[ia])));
		normals[ia] = normals[ib] = normals[ic] = normal;
		//	std::cout << normal.x << "," << normal.y<< "," << normal.z << std::endl;
	}
}