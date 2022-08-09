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
		virtual void SetIntArray(const std::string& name, int* values, unsigned int count) override;

		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& vector) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& vector) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& vector) override;

		virtual void SetMat3(const std::string& name, const glm::mat3& matrix) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) override;

		void UploadUniformInt(const std::string& name, int value) const;
		void UploadUniformIntArray(const std::string& name, int* values, unsigned int count) const;

		void UploadUniformFloat(const std::string& name, float value) const;
		void UploadUniformFloat2(const std::string& name, const glm::vec2& vector) const;
		void UploadUniformFloat3(const std::string& name, const glm::vec3& vector) const;
		void UploadUniformFloat4(const std::string& name, const glm::vec4& vector) const;

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

	private:
		std::unordered_map<GLenum, std::string> Preprocess(const std::string& source);

		void CompileVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources, bool force = false);
		void CompileOpenGLBinariesFromVulkanSPIRVs(std::unordered_map<GLenum, std::vector<uint32_t>>& vulkanSPIRVs, bool force = false);
		void CreateProgram();

		void Reflect(GLenum type, const std::vector<uint32_t>& shaderData);

		GLint UniformLocation(const std::string& name) const;

	private:
		unsigned int m_RendererID;
		std::string m_Filepath;
		std::string m_Name;

		std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRVs;
		std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRVs;

		std::unordered_map<GLenum, std::string> m_OpenGLSourceCodes;

		std::unordered_map<std::string, GLint> mutable m_UniformLocationCache;
	};

}
