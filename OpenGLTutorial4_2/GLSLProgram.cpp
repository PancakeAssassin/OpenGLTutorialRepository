#include "GLSLProgram.h"

#include <iostream>

#include <fstream>
using std::ifstream;
using std::ios;

#include <sstream>
#include <sys/stat.h>

GLSLProgram::GLSLProgram() : ProgramID(0), bIsLinked(false)
{

}

GLSLProgram::~GLSLProgram()
{
	if (ProgramID == 0)
		return;

	//find the number of attached shaders
	GLint numShaders = 0;
	glGetProgramiv(ProgramID, GL_ATTACHED_SHADERS, &numShaders);

	//get the shader names
	GLuint* shaderHandles = new GLuint[numShaders];
	glGetAttachedShaders(ProgramID, numShaders, NULL, shaderHandles);

	//delete the shaders
	for (int i = 0; i < numShaders; i++)
	{
		glDeleteShader(shaderHandles[i]);
	}

	//delete the program
	glDeleteProgram(ProgramID);

	delete[] shaderHandles;
}


bool GLSLProgram::LoadShader(const string& Filename, GLenum ShaderType)
{
	if (ProgramID <= 0)
	{
		ProgramID = glCreateProgram();
		if (ProgramID == 0)
		{
			std::cout << "Unable to create shader program." << std::endl;
			return false;
		}
	}
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
		return false;
	}

	glAttachShader(ProgramID, shader);
	return true;
}

bool GLSLProgram::Link()
{
	if (bIsLinked)
		return true;
	
	if (ProgramID <= 0)
	{
		std::cout << "Program must be created to be able to link." << std::endl;
		return false;
	}
	glLinkProgram(ProgramID);

	GLint programLinked = GL_TRUE;
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &programLinked);
	if (programLinked != GL_TRUE)
	{
		std::cout << "Unable to link program." << std::endl;
		return false;
	}
	UniformLocation.clear();
	AttributeLocation.clear();
	bIsLinked = true;
	return true;
}


void GLSLProgram::Use()
{
	if (ProgramID <= 0 || (!bIsLinked))
	{
		std::cout << "Shader is not linked" << std::endl;
		return;
	}
	glUseProgram(ProgramID);
}


void GLSLProgram::UnUse()
{
	glUseProgram(0);

}

GLuint GLSLProgram::GetHandle()
{
	return ProgramID;
}

bool GLSLProgram::IsLinked()
{
	return bIsLinked;
}

void GLSLProgram::SetUniform(const string& uniformName, const mat4& m)
{
	GLint location = GetUniformLocation(uniformName);
	glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]);
}

void GLSLProgram::SetUniform(const string& uniformName, const mat3& m)
{
	GLint location = GetUniformLocation(uniformName);
	glUniformMatrix3fv(location, 1, GL_FALSE, &m[0][0]);
}

void GLSLProgram::SetUniform(const string& uniformName, const vec4& v)
{
	GLint location = GetUniformLocation(uniformName);
	glUniform4f(location, v.x, v.y, v.z, v.w);
}

void GLSLProgram::SetUniform(const string& uniformName, const vec3& v)
{
	GLint location = GetUniformLocation(uniformName);
	glUniform3f(location, v.x, v.y, v.z);
}
void GLSLProgram::SetUniform(const string& uniformName, const vec2& v)
{
	GLint location = GetUniformLocation(uniformName);
	glUniform2f(location, v.x, v.y);
}
void GLSLProgram::SetUniform(const string& uniformName, GLint val)
{
	GLint location = GetUniformLocation(uniformName);
	glUniform1i(location, val);
}

void GLSLProgram::SetUniform(const string& uniformName, GLuint val)
{
	GLint location = GetUniformLocation(uniformName);
	glUniform1ui(location, val);
}

void GLSLProgram::SetUniform(const string& uniformName, GLfloat val)
{
	GLint location = GetUniformLocation(uniformName);
	glUniform1f(location, val);
}

void GLSLProgram::SetUniform(const string& uniformName, bool val)
{
	GLint location = GetUniformLocation(uniformName);
	glUniform1i(location, val);
}

void GLSLProgram::BindAttributeLocation(GLuint location, const string& attributeName)
{
	glBindAttribLocation(ProgramID, location, attributeName.c_str());
}

void GLSLProgram::BindFragLocation(GLuint location, const string& fragName)
{
	glBindFragDataLocation(ProgramID, location, fragName.c_str());
}

string GLSLProgram::LoadSourceFromFile(const string& Filename)
{
	std::ifstream in(Filename, std::ios::in | std::ios::binary);
	if (in)
	{
		string source;
		in.seekg(0, std::ios::end);
		source.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&source[0], source.size());
		in.close();
		return source;
	}
	throw(errno);
}

void GLSLProgram::PrintShaderLog(GLuint shader)
{
	// only shaders should be passed to this function
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

void GLSLProgram::PrintProgramLog(GLuint program)
{
	//only programs should be passed to this function
	if (glIsProgram(program))
	{
		std::cout << "Cannot print program log for a non  program object." << std::endl;
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
		std::cout << infoLog;
	}

	delete[] infoLog;
}

GLint GLSLProgram::GetUniformLocation(const string& uniformName)
{
	auto pos = UniformLocation.find(uniformName);
	
	if (pos == UniformLocation.end())
	{
		UniformLocation[uniformName] = glGetUniformLocation(ProgramID, uniformName.c_str());
	}
	return UniformLocation[uniformName];
}

GLint GLSLProgram::GetAttributeLocation(const string& attributeName)
{
	auto pos = AttributeLocation.find(attributeName);

	if (pos == AttributeLocation.end())
	{
		AttributeLocation[attributeName] = glGetAttribLocation(ProgramID, attributeName.c_str());
	}
	return AttributeLocation[attributeName];
}
