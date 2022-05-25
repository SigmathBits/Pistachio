#pragma once

#include "Pistachio/Renderer/Shader.h"


namespace Pistachio {

	class OpenGLShader : public Shader {
	public:
		OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource);
		virtual ~OpenGLShader() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const override;

	private:
		unsigned int m_RendererID;
	};

}
