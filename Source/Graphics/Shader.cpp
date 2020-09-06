#include "System.h"
#include "shader.h"

namespace Augiwne { namespace Graphics {
	Shader::Shader(const char* vertPath, const char* fragPath)
		: m_VertPath(vertPath), m_FragPath(fragPath)
	{
		m_ShaderID = Load();
	}
	Shader::~Shader()
	{
		glDeleteProgram(m_ShaderID);
	}

	GLuint Shader::Load()
	{
		GLuint program = glCreateProgram();
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

		string vSource = ReadFile(m_VertPath);
		string fSource = ReadFile(m_FragPath);

		const char* vs = vSource.c_str();
		const char* fs = fSource.c_str();

		glShaderSource(vertex, 1, &vs, NULL);
		glCompileShader(vertex);

		GLint result;
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			GLint length;
			glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &length);
			vector<char> error(length);
			glGetShaderInfoLog(vertex, length, &length, &error[0]);
			std::cout << &error[0] << std::endl;
			glDeleteShader(vertex);
			return 0;
		}

		glShaderSource(fragment, 1, &fs, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			GLint length;
			glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length);
			std::vector<char> error(length);
			glGetShaderInfoLog(fragment, length, &length, &error[0]);
			std::cout << "fragment error! shader" << &error[0] << std::endl;
			glDeleteShader(fragment);
			return 0;
		}
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);

		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		return program;
	}
	void Shader::Enable() const
	{
		glUseProgram(m_ShaderID);
	}
	void Shader::Disable() const
	{
		glUseProgram(0);
	}
	GLint Shader::getLocation(const GLchar* name)
	{
		return glGetUniformLocation(m_ShaderID, name);
	}
	void Shader::SetUniform1i(const char* name, int value)
	{
		glUniform1i(getLocation(name), value);
	}
	void Shader::SetUniform1iv(const char* name, int* value, int count)
	{
		glUniform1iv(getLocation(name), count, value);
	}
	void Shader::SetUniform1f(const char* name, float value)
	{ 
		glUniform1f(getLocation(name), value); 
	}
	void Shader::SetUniform1fv(const char* name, float* value, int count)
	{
		glUniform1fv(getLocation(name), count, value);
	}
	void Shader::SetUniform2f(const char* name, Maths::Vector2& vector)
	{
		glUniform2f(getLocation(name), vector.x, vector.y);
	}
	void Shader::SetUniform3f(const char* name, Maths::Vector3& vector)
	{
		glUniform3f(getLocation(name), vector.x, vector.y, vector.z);
	}
	void Shader::SetUniform4f(const char* name, Maths::Vector4& vector)
	{
		glUniform4f(getLocation(name), vector.x, vector.y, vector.z, vector.w);
	}
	void Shader::SetUniformMatrix4(const char* name, const Maths::Matrix4& matrix)
	{
		glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, matrix.elements);
	}
}}