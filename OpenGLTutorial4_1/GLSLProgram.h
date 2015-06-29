#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H

#include "GLIncludes.h"
#include <string>
#include <map>

using std::string;
using std::map;


class GLSLProgram
{
public:
	GLSLProgram();
	~GLSLProgram();

	bool LoadShader(const string& Filename, GLenum ShaderType);

	bool Link();
	void Use();
	void UnUse();

	GLuint GetHandle();
	bool IsLinked();

	void SetUniform(const string& uniformName, const mat4& m);
	void SetUniform(const string& uniformName, const mat3& m);
	void SetUniform(const string& uniformName, const vec4& m);
	void SetUniform(const string& uniformName, const vec3& v);
	void SetUniform(const string& uniformName, const vec2& v);
	void SetUniform(const string& uniformName, GLint val);
	void SetUniform(const string& uniformName, GLuint val);
	void SetUniform(const string& uniformName, GLfloat val);
	void SetUniform(const string& uniformName, bool val);

	GLint GetAttributeLocation(const string& attributeName);
	void BindAttributeLocation(GLuint location, const string& attributeName);
	void BindFragLocation(GLuint location, const string& fragName);


private:
	GLuint ProgramID;
	bool bIsLinked;

	map<string, GLint> AttributeLocation;
	map<string, GLint> UniformLocation;

	string LoadSourceFromFile(const string& Filename);
	void PrintShaderLog(GLuint shader);
	void PrintProgramLog(GLuint program);

	GLint GetUniformLocation(const string& uniformName);
};

#endif