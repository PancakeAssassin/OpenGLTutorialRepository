#include "GLIncludes.h"

#include <math.h>

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <assert.h>
#include <iostream>

#include "HeightField.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform2.hpp>
using glm::mat4;

float xpos= 850, ypos= 350, zpos= 250, xrot= 0, yrot= 0, angle= 0.0;
float lastx, lasty;

vec3 position=vec3(0.f, 0.f, 0.f);
float horizontalAngle = 3.14f;
float verticalAngle = 0.f;

float bounce;
float cScale= 1.0;
float deltaTime = 1.0f;
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

HeightField hField;

mat4 model;
mat4 view;
mat4 projection;

void setcamera()
{
	
	view = glm::rotate(view, xrot, vec3(0.0f, 1.0f, 0.0f));
	view = glm::rotate(view, yrot, vec3(0.0f, 0.0f, 1.0f));
	view= glm::translate(view, vec3(-xpos, -ypos, -zpos));	
}

void setMatrices()
{
	mat4 mv= view * model;
	hField.prog.setUniform("ModelViewMatrix", mv);
	hField.prog.setUniform("NormalMatrix", 
							mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	hField.prog.setUniform("MVP", projection *mv);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//set identity matrix
	model= mat4(1.0);
	setcamera();
	setMatrices();
	hField.Render();
}

void Init(void)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	projection= glm::perspective(60.f, (float)SCREEN_WIDTH/SCREEN_HEIGHT, 1.0f, 1000.f);

	hField.Create("heightField.raw", 1024, 1024);
	std::cout<<"Height Map initialized"<<std::endl;
}

/*void mouseMove_callback(GLFWwindow* window, double x, double y)
{
	xrot += 10.f * deltaTime * float(lastx - x);
	yrot += 10.f * deltaTime * float(lasty - y);

	lastx = x;
	lasty = y;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if((key == GLFW_KEY_W  || key ==  GLFW_KEY_UP) && (action == GLFW_PRESS || action == GLFW_REPEAT) )
	{
		float xrotrad, yrotrad;
		yrotrad= (yrot / 180 * 3.141592654f);
		xrotrad= (xrot / 180 * 3.141592654f);
		xpos-= float(sin(yrotrad)) *cScale;
		zpos+= float(cos(yrotrad)) *cScale;
		ypos+= float(sin(xrotrad));
		bounce += 0.04f;
	}
	if((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		float xrotrad, yrotrad;
		yrotrad= (yrot / 180 * 3.141592654f);
		xrotrad= (xrot / 180 * 3.141592654f);
		xpos+= float(sin(yrotrad)) * cScale;
		zpos-= float(cos(yrotrad)) * cScale;
		ypos-= float(sin(xrotrad));
		bounce+= 0.04f;
	}
	if((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		float yrotrad;
		yrotrad= (yrot / 180 * 3.141592654f);
		xpos-= float(cos(yrotrad)) * cScale;
		zpos-= float(sin(yrotrad)) * cScale;
	}
	if((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		float yrotrad;
		yrotrad= (yrot / 180 * 3.141592654f);
		xpos+= float(cos(yrotrad)) *cScale;
		zpos+= float(sin(yrotrad)) *cScale;
	}
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}*/

void HandleInput(GLFWwindow* window)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	horizontalAngle += .005f * float(SCREEN_WIDTH / 2 - x);
	verticalAngle += .005f * float(SCREEN_HEIGHT / 2 - y);

	vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);

	vec3 right = vec3(
		sin(horizontalAngle - 3.14f / 2.f),
		0.f,
		cos(horizontalAngle - 3.14f / 2.f));

	vec3 up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		position += direction * deltaTime;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		position -= direction * deltaTime;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		position += right * deltaTime;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		position -= right * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	
	view = glm::lookAt(position,
		position + direction,
		up);
}

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}


void resize(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	projection= glm::perspective(60.f, (float)w/h, 1.0f, 1000.f);
}

int main(void)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if(!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window= glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Terrain Generation", NULL, NULL);
	glfwSetCursorPos(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

	if(!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	//initialize GLEW
	glewExperimental= GL_TRUE;
	GLenum glewStatus= glewInit();
	if(glewStatus != GLEW_OK)
	{
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(glewStatus));
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//glfwSetKeyCallback(window, key_callback);
	//glfwSetCursorPosCallback(window, mouseMove_callback);
	glfwSetWindowSizeCallback(window, resize);
	
	Init();
	int currentTime = 0.0f;
	int previousTime= 0.0f;
	while(!glfwWindowShouldClose(window))
	{
		currentTime = glfwGetTime();
		deltaTime= currentTime-previousTime;
		previousTime = currentTime;
		HandleInput(window);
		display();
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}