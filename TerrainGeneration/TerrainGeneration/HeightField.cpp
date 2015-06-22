#include "HeightField.h"

#include "tgaio.h"
#include <stdio.h>
#include <iostream>
using std::cerr;
using std::endl;

bool HeightField::Create(char *hFileName, int hWidth, int hHeight)
{
	hmWidth= hWidth;
	hmHeight= hHeight;
	unsigned char buffer;

	FILE *fp;

	fp= fopen(hFileName, "rb");
	
	//read in vertices from texture
	//can be optimized by just loading texture??
	//consider this first/simplest attempt with height maps
	std::vector<glm::vec3> Vertices;
	for(int hMapX= 0; hMapX < hWidth; ++hMapX)
	{
		for(int hMapZ= 0; hMapZ < hHeight; ++hMapZ)
		{
			fread(&buffer, 1, 1, fp);
			Vertices.push_back(vec3(hMapX, int(buffer), hMapZ));
		}
	}
	fclose(fp);

	//load number of vertices for glDrawArrays
	numOfVerts= Vertices.size();

	//load the vertices into the vbo
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(vec3), &Vertices[0], GL_STATIC_DRAW);

	generateElementArrayBuffer(Vertices);

	//load texture for terrain
	GLint w, h;
	glActiveTexture(GL_TEXTURE0);
	GLubyte* data= TGAIO::read("texture.tga", w, h);
	
	GLuint texID;
	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	delete[] data;

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	compileAndLinkShaders();
	return true;
}

void HeightField::Render(void)
{
	glBindVertexArray(vaoHandle);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	prog.use();
	glDrawElements(GL_TRIANGLE_STRIP, numOfElements, GL_UNSIGNED_INT, (void*) 0);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void HeightField::compileAndLinkShaders()
{
	try
	{
		prog.compileShader("shaders/simple.vert", GLSLShader::VERTEX);
		prog.compileShader("shaders/simple.frag", GLSLShader::FRAGMENT);
		prog.link();
		prog.use();
	}
	catch(GLSLProgramException &e)
	{
		cerr<<e.what()<<endl;
		exit(EXIT_FAILURE);
	}
}

void HeightField::generateElementArrayBuffer(const std::vector<vec3> Verts)
{

	std::vector<unsigned int> indices;
	for(int y= 0; y < hmHeight - 1;  ++y)
	{
		if( y > 0)
		{
			indices.push_back(y * hmWidth);
		}
		for(int x= 0; x < hmWidth; ++x)
		{
			indices.push_back(y * hmWidth + x);
			indices.push_back((y+1) * hmWidth + x);
		}
		if(y < hmHeight - 2)
		{
			indices.push_back(((y+1) * hmWidth) + (hmWidth-1));
		}
	}

	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	numOfElements= indices.size();
}