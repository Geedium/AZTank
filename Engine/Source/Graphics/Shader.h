#pragma once
#include <glew.h>
#include <iostream>
#include <vector>

#include "../Maths/Matrix4.h"
#include "../Utilities/File.h"

namespace Augiwne { namespace Graphics {
	class Shader {
	public:
		GLuint m_ShaderID;
		const char* m_VertPath;
		const char* m_FragPath;
	public:
		Shader(const char* vertPath, const char* fragPath);
		~Shader();
	public:
		void Enable() const;
		void Disable() const;
		void SetUniform1i(const char* name, int value);
		void SetUniform1iv(const char* name, int* value, int count);
		void SetUniform1f(const char* name, float value);
		void SetUniform1fv(const char* name, float* value, int count);
		void SetUniform2f(const char* name, Maths::Vector2& vector);
		void SetUniform3f(const char* name, Maths::Vector3& vector);
		void SetUniform4f(const char* name, Maths::Vector4& vector);
		void SetUniformMatrix4(const char* name, const Maths::Matrix4& matrix);
	private:
		GLuint Load();
		GLint getLocation(const GLchar* name);
	};
}}