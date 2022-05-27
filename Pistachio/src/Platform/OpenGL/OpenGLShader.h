#pragma once

#include <glm/glm.hpp>

#include "Pistachio/Renderer/Shader.h"


// TODO: Remove
typedef unsigned int GLenum;
typedef int GLint;


namespace Pistachio {

	class OpenGLShader : public Shader {
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);

		virtual ~OpenGLShader() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const std::string& Name() const override { return m_Name; };

		virtual void SetInt(const std::string& name, int value) override;

		virtual void SetFloat3(const std::string& name, const glm::vec3& vector) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& vector) override;

		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) override;

		void UploadUniformInt(const std::string& name, int value) const;

		void UploadUniformFloat1(const std::string& name, float value) const;
		void UploadUniformFloat2(const std::string& name, const glm::vec2& vector) const;
		void UploadUniformFloat3(const std::string& name, const glm::vec3& vector) const;
		void UploadUniformFloat4(const std::string& name, const glm::vec4& vector) const;

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> Preprocess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string> shaderSources);

		GLint UniformLocation(const std::string& name) const;

	private:
		unsigned int m_RendererID;
		std::string m_Name;

		std::unordered_map<std::string, GLint> mutable m_UniformLocationCache;
	};

}
