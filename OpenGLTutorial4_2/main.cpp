//OpenGL Tutorial 4: GLSLProgram Class
#include "GLIncludes.h"
#include "GLSLProgram.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cerrno>

using std::vector;

//-------------------------------------------------
//			GLOBALS
//-------------------------------------------------
const GLint SCREEN_WIDTH = 1024;
const GLint SCREEN_HEIGHT = 768;

GLFWwindow* Window = NULL;

GLSLProgram* Program;
GLuint VAO = 0;
GLuint VBO_Vertices = 0;
GLuint VBO_Normals = 0;
GLuint IBO = 0;
GLint VertexPosition = -1;
GLint VertexNormal = -1;

vec4 LightPosition = vec4(0.f, 1.f, 10.5f, -1.f);
vec3 LightIntensity = vec3(1.f, 1.f, 1.f);

vec3 DiffuseReflectivity = vec3(.0f, .8f, .8f);
vec3 AmbientReflectivity = vec3(.0f, .8f, .8f);

mat4 Model;
mat4 View;
mat4 Projection;

string VertexShaderFile = "Shader.vs";
string FragmentShaderFile = "Shader.fs";

//-------------------------------------------------
//			GLOBALS END
//-------------------------------------------------

//-------------------------------------------------
//			FUNCTION PROTOTYPES
//-------------------------------------------------
bool Init();
bool InitGL();
void Shutdown();

//render the model
void DrawScene();

//error callback for glfw
static void error_callback(int error, const char* description);

//load the object file
void load_obj(const char* filename, vector<vec4> &vertices, vector<vec3> &normals, vector<GLushort> &elements);

//---------------------------------------------------
//			FUNCTION PROTOTYPES END
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

void DrawScene()
{
	//clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use shader program
	Program->Use();

	//model position
	Model = mat4(1.0f);

	Model = glm::rotate(Model, -45.f, vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 MV = View*Model;
	glm::mat3 Normal = glm::mat3(vec3(MV[0]), vec3(MV[1]), vec3(MV[2]));
	glm::mat4  MVP = Projection * View * Model;

	Program->SetUniform("MVP", MVP);

	Program->SetUniform("ProjectionMatrix", Projection);
	
	Program->SetUniform("ModelViewMatrix", MV);

	Program->SetUniform("NormalMatrix", Normal);

	Program->SetUniform("Light.Position", LightPosition);
	Program->SetUniform("Light.Intensity", LightIntensity);
	Program->SetUniform("Material.Ka", AmbientReflectivity);
	Program->SetUniform("Material.Kd", DiffuseReflectivity);

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

	//end use of shader program
	Program->UnUse();
}

bool Init()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		std::cout << "Unable to initialize GLFW" << std::endl;
		return false;
	}

	Window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Tutorial 4", NULL, NULL);
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
		std::cout << "Unable to initialize OpenGL program." << std::endl;
		return false;
	}

	return true;
}

bool InitGL()
{
	Program = new GLSLProgram();
	if (!Program->LoadShader(VertexShaderFile, GL_VERTEX_SHADER))
	{
		return false;
	}

	if (!Program->LoadShader(FragmentShaderFile, GL_FRAGMENT_SHADER))
	{
		return false;
	}

	if (!Program->Link())
	{
		return false;
	}

	Program->BindAttributeLocation(0, "VertexPosition");
	Program->BindAttributeLocation(1, "VertexNormal");

	glClearColor(.5f, .5f, .5f, 1.f);

	//load the model
	vector<vec4> torus_vertices;
	vector<vec3> torus_normals;
	vector<GLushort> torus_elements;

	load_obj("torus.obj", torus_vertices, torus_normals, torus_elements);

	glGenBuffers(1, &VBO_Vertices);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertices);
	glBufferData(GL_ARRAY_BUFFER, torus_vertices.size() * sizeof(vec4), &torus_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &VBO_Normals);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Normals);
	glBufferData(GL_ARRAY_BUFFER, torus_normals.size() * sizeof(vec3), &torus_normals[0], GL_STATIC_DRAW);

	//create IBO
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, torus_elements.size() * sizeof(GLushort), &torus_elements[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	View = glm::lookAt(vec3(0.f, 0.f, -2.5f),
		vec3(0.f, 0.f, 1.f),
		vec3(0.f, 1.f, 0.f));

	Projection = glm::perspective(45.f,
							(float)SCREEN_WIDTH / SCREEN_HEIGHT,
							.1f,
							100.f);
	glEnable(GL_DEPTH_TEST);

	LightPosition = View * LightPosition;

	return true;
}

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void Shutdown()
{
	delete Program;
	Program = NULL;
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