#include "pstpch.h"

#include "OpenGLShader.h"

#include <fstream>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "Pistachio/Core/Core.h"


namespace Pistachio {

	static GLenum ShaderTypeFromString(const std::string& shaderTypeName) {
		if (shaderTypeName == "vertex") {
			return GL_VERTEX_SHADER;
		} else if (shaderTypeName == "fragment" || shaderTypeName == "pixel") {
			return GL_FRAGMENT_SHADER;
		}

		PST_CORE_ASSERT(false, "Unrecognised shader type");
		return 0;
	}

	static std::string ShaderStringFromType(GLenum shaderType) {
		switch (shaderType) {
			case GL_VERTEX_SHADER:
				return "Vertex";
			case GL_FRAGMENT_SHADER:
				return "Fragment";
			default:
				PST_CORE_ASSERT(false, "Unrecognised shader type");
				return "";
		}
	}

	OpenGLShader::OpenGLShader(const std::string& filepath) {
		PST_PROFILE_FUNCTION();

		std::string shaderSource = ReadFile(filepath);
		auto shaderSources = Preprocess(shaderSource);
		Compile(shaderSources);

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

		auto lastPeriod = filepath.rfind(".");
		auto count = (lastPeriod == std::string::npos ? filepath.size() : lastPeriod) - lastSlash;

		m_Name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& filepath)
		: m_Name(name) {
		PST_PROFILE_FUNCTION();

		std::string shaderSource = ReadFile(filepath);
		auto shaderSources = Preprocess(shaderSource);
		Compile(shaderSources);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
		: m_RendererID(0), m_Name(name) {
		PST_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader() {
		PST_PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const {
		PST_PROFILE_FUNCTION();

		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const {
		PST_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string& name, int value) {
		PST_PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value) {
		PST_PROFILE_FUNCTION();

		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& vector) {
		PST_PROFILE_FUNCTION();

		UploadUniformFloat2(name, vector);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& vector) {
		PST_PROFILE_FUNCTION();

		UploadUniformFloat3(name, vector);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& vector) {
		PST_PROFILE_FUNCTION();

		UploadUniformFloat4(name, vector);
	}

	void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& matrix) {
		PST_PROFILE_FUNCTION();

		UploadUniformMat3(name, matrix);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& matrix) {
		PST_PROFILE_FUNCTION();

		UploadUniformMat4(name, matrix);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value) const {
		glUniform1i(UniformLocation(name), value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value) const {
		glUniform1f(UniformLocation(name), value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& vector) const {
		glUniform2f(UniformLocation(name), vector.x, vector.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& vector) const {
		glUniform3f(UniformLocation(name), vector.x, vector.y, vector.z);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const {
		glUniformMatrix3fv(UniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const {
		glUniformMatrix4fv(UniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath) {
		PST_PROFILE_FUNCTION();

		std::string result;
		std::ifstream inFile(filepath, std::ios::in | std::ios::binary);

		if (!inFile) {
			PST_CORE_ERROR("Could not open file \"{}\"", filepath);
			return "";
		}

		inFile.seekg(0, std::ios::end);
		result.resize(inFile.tellg());
		inFile.seekg(0, std::ios::beg);

		inFile.read(&result[0], result.size());

		inFile.close();

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::Preprocess(const std::string& source) {
		PST_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#shader";
		size_t typeTokenLength = strlen(typeToken);

		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos) {
			size_t eol = source.find_first_of("\r\n", pos);
			PST_CORE_ASSERT(eol != std::string::npos, "Syntax error: no shader source after type header");

			size_t begin = pos + typeTokenLength + 1;
			std::string shaderTypeName = source.substr(begin, eol - begin);
			
			GLenum shaderType = ShaderTypeFromString(shaderTypeName);

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);

			shaderSources[shaderType] = source.substr(
				nextLinePos, 
				pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos)
			);
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string> shaderSources) {
		PST_PROFILE_FUNCTION();

		/// Copied and modified from https://www.khronos.org/opengl/wiki/Shader_Compilation#Example
		// Get a program object.
		GLuint programID = glCreateProgram();

		PST_CORE_ASSERT(shaderSources.size() <= 2, "Only a maximum of two shaders in a single source is supported")
		std::array<GLuint, 2> shaderIDs;

		size_t i = 0;
		for (auto& pair : shaderSources) {
			GLenum shaderType = pair.first;
			const std::string& source = pair.second;
			
			GLuint shaderID = glCreateShader(shaderType);

			// Send the vertex shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shaderID, 1, &sourceCStr, 0);

			// Compile the vertex shader
			glCompileShader(shaderID);

			GLint isCompiled = 0;
			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE) {
				GLint maxLength = 0;
				glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shaderID);

				PST_CORE_ERROR("{} Shader: {}", ShaderStringFromType(shaderType), infoLog.data());
				PST_CORE_ASSERT(false, "Shader compilation failed");
				return;
			}

			// Attach our shaders to our program
			glAttachShader(programID, shaderID);

			shaderIDs[i++] = shaderID;
		}

		// Link our program
		glLinkProgram(programID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(programID);

			// Don't leak shaders either.
			for (GLuint ID : shaderIDs) {
				glDeleteShader(ID);
			}

			PST_CORE_ERROR("Shader Linking: {}", infoLog.data());
			PST_CORE_ASSERT(false, "Shader linking failed");
			return;
		}

		m_RendererID = programID;

		// Always detach shaders after a successful link.
		for (GLuint ID : shaderIDs) {
			glDetachShader(programID, ID);
		}
	}

	GLint OpenGLShader::UniformLocation(const std::string& name) const {
		PST_PROFILE_FUNCTION();

		auto locationSearch = m_UniformLocationCache.find(name);
		if (locationSearch != m_UniformLocationCache.end()) {
			return locationSearch->second;
		}

		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location == -1) {
			std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
		}

		m_UniformLocationCache[name] = location;
		return location;
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value) const {
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

}
