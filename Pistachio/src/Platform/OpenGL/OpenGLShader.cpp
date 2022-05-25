#include "pstpch.h"

#include "OpenGLShader.h"

#include <glad/glad.h>


namespace Pistachio {

	OpenGLShader::OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource)
		: m_RendererID(0) {
		/// Copied and modified from https://www.khronos.org/opengl/wiki/Shader_Compilation#Example
		// Create an empty vertex shader handle
		GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertexSource.c_str();
		glShaderSource(vertexShaderID, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShaderID);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShaderID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShaderID);

			PST_CORE_ERROR("Vertex Shader: {}", infoLog.data());
			PST_CORE_ASSERT(false, "Vertex Shader compilation failed");
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar*)fragmentSource.c_str();
		glShaderSource(fragmentShaderID, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShaderID);

		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShaderID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShaderID);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShaderID);

			PST_CORE_ERROR("Fragment Shader: {}", infoLog.data());
			PST_CORE_ASSERT(false, "Fragment Shader compilation failed");
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_RendererID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_RendererID, vertexShaderID);
		glAttachShader(m_RendererID, fragmentShaderID);

		// Link our program
		glLinkProgram(m_RendererID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_RendererID);
			// Don't leak shaders either.
			glDeleteShader(vertexShaderID);
			glDeleteShader(fragmentShaderID);

			PST_CORE_ERROR("Shader Linking: {}", infoLog.data());
			PST_CORE_ASSERT(false, "Shader linking failed");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_RendererID, vertexShaderID);
		glDetachShader(m_RendererID, fragmentShaderID);
	}

	OpenGLShader::~OpenGLShader() {
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const {
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const {
		glUseProgram(0);
	}

}
