#pragma once

#include "GLSLProgram.h"
#include <vector>
#include <glm\glm.hpp>
using glm::vec3;

class HeightField
{
private:
	int hmHeight;
	int hmWidth;
	int numOfVerts;
	int numOfElements;

	GLuint vertexBuffer;
	GLuint vaoHandle;
	GLuint elementBuffer;
	


public:
	GLSLProgram prog;

	bool Create(char *hFileName, int hWidth, int hHeight);

	void Render(void);

	void compileAndLinkShaders();

	void generateElementArrayBuffer(std::vector<vec3> Verts);
};