#include "pstpch.h"

#include "OpenGLShader.h"

#include <fstream>
#include <filesystem>
#include <chrono>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "Pistachio/Core/Core.h"


namespace Pistachio {

	namespace Utils {

		static GLenum GLShaderTypeFromString(const std::string& shaderTypeName) {
			if (shaderTypeName == "vertex") {
				return GL_VERTEX_SHADER;
			} else if (shaderTypeName == "fragment" || shaderTypeName == "pixel") {
				return GL_FRAGMENT_SHADER;
			}

			PST_CORE_ASSERT(false, "Unrecognised shader type");
			return 0;
		}

		static std::string GLShaderTypeToString(GLenum shaderType) {
			switch (shaderType) {
				case GL_VERTEX_SHADER:
					return "Vertex";
				case GL_FRAGMENT_SHADER:
					return "Fragment";
				default:
					break;
			}

			PST_CORE_ASSERT(false, "Unrecognised shader type");
			return "";
		}

		static shaderc_shader_kind GLShaderTypeToShaderC(GLenum type) {
			switch (type) {
				case GL_VERTEX_SHADER:
					return shaderc_glsl_vertex_shader;
				case GL_FRAGMENT_SHADER:
					return shaderc_glsl_fragment_shader;
				default:
					break;
			}

			PST_CORE_ASSERT(false, "Unrecognised shader type");
			return (shaderc_shader_kind)0;
		}

		static const char* CacheDirectory() {
			return "assets/cache/shader/opengl";
		}

		static void CreateCacheDirectory() {
			std::string cacheDirectory = CacheDirectory();
			if (!std::filesystem::exists(cacheDirectory)) {
				std::filesystem::create_directories(cacheDirectory);
			}
		}

		static const char* GLShaderTypeCachedOpenGLFileExtension(GLenum type) {
			switch (type) {
				case GL_VERTEX_SHADER:
					return ".cached_opengl.vert";
				case GL_FRAGMENT_SHADER:
					return ".cached_opengl.frag";
				default:
					break;
			}

			PST_CORE_ASSERT(false, "Unrecognised shader type");
			return "";
		}

		static const char* GLShaderTypeCachedVulkanFileExtension(GLenum type) {
			switch (type) {
				case GL_VERTEX_SHADER:
					return ".cached_vulkan.vert";
				case GL_FRAGMENT_SHADER:
					return ".cached_vulkan.frag";
				default:
					break;
			}

			PST_CORE_ASSERT(false, "Unrecognised shader type");
			return "";
		}

	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: m_Filepath(filepath) {
		PST_PROFILE_FUNCTION();

		Utils::CreateCacheDirectory();

		std::string shaderSource = ReadFile(filepath);
		auto shaderSources = Preprocess(shaderSource);

		{
			auto start = std::chrono::high_resolution_clock::now();
			CompileVulkanBinaries(shaderSources);
			CompileOpenGLBinariesFromVulkanSPIRVs(m_VulkanSPIRVs);
			CreateProgram();
			auto stop = std::chrono::high_resolution_clock::now();
			PST_CORE_WARN("Shader compilation took {:.2f} ms",
				(float)std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 1000.0f);
		}

		// Reflect info about shaders (optional)
		//for (auto&& [type, shaderData] : m_OpenGLSPIRVs) {
		//	Reflect(type, shaderData);
		//}

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

		auto lastPeriod = filepath.rfind(".");
		auto count = (lastPeriod == std::string::npos ? filepath.size() : lastPeriod) - lastSlash;

		m_Name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& filepath)
		: m_Name(name), m_Filepath(filepath) {
		PST_PROFILE_FUNCTION();

		std::string shaderSource = ReadFile(filepath);
		auto shaderSources = Preprocess(shaderSource);

		{
			CompileVulkanBinaries(shaderSources);
			CompileOpenGLBinariesFromVulkanSPIRVs(m_VulkanSPIRVs);
			CreateProgram();
		}
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
		: m_RendererID(0), m_Name(name) {
		PST_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSource;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSource;
		
		{
			CompileVulkanBinaries(shaderSources);
			CompileOpenGLBinariesFromVulkanSPIRVs(m_VulkanSPIRVs);
			CreateProgram();
		}
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

	void OpenGLShader::SetIntArray(const std::string& name, int* values, unsigned int count) {
		PST_PROFILE_FUNCTION();

		UploadUniformIntArray(name, values, count);
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

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, unsigned int count) const {
		glUniform1iv(UniformLocation(name), count, values);
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

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value) const {
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
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
			
			GLenum shaderType = Utils::GLShaderTypeFromString(shaderTypeName);

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);

			shaderSources[shaderType] = source.substr(
				nextLinePos, 
				pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos)
			);
		}

		return shaderSources;
	}

	void OpenGLShader::CompileVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources, bool force /*= false*/) {
		PST_PROFILE_FUNCTION();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

		constexpr bool optimise = true;
		if (optimise) {
			options.SetOptimizationLevel(shaderc_optimization_level_performance);
		}


		auto& shaderData = m_VulkanSPIRVs;
		shaderData.clear();

		for (auto&& [type, source] : shaderSources) {
			std::filesystem::path cacheDirectory = Utils::CacheDirectory();
			std::filesystem::path shaderFilepath = m_Filepath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilepath.filename().string() + Utils::GLShaderTypeCachedVulkanFileExtension(type));

			// Load in cached file if we have it
			std::ifstream inFile(cachedPath, std::ios::in | std::ios::binary);
			if (!force && inFile.is_open()) {
				inFile.seekg(0, std::ios::end);
				auto size = inFile.tellg();
				inFile.seekg(0, std::ios::beg);

				// Read binary datainto 
				auto& data = shaderData[type];
				data.resize(size / sizeof(uint32_t));
				inFile.read((char*)data.data(), size);

			} else {
				PST_CORE_TRACE("Compiling Vulkan {} Shader SPIR-V binaries ... ", Utils::GLShaderTypeToString(type));

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
					source,
					Utils::GLShaderTypeToShaderC(type),
					m_Filepath.c_str(), options
				);

				if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
					PST_CORE_CRITICAL("{} Shader compilation failed", Utils::GLShaderTypeToString(type));
					PST_CORE_ASSERT(false, module.GetErrorMessage());
				}

				shaderData[type] = std::vector<uint32_t>(module.cbegin(), module.cend());

				// Cache binary
				std::ofstream outFile(cachedPath, std::ios::out | std::ios::binary);
				if (outFile.is_open()) {
					auto& data = shaderData[type];
					outFile.write((char*)data.data(), data.size() * sizeof(uint32_t));
					outFile.flush();
					outFile.close();
				}
			}
		}
	}

	void OpenGLShader::CompileOpenGLBinariesFromVulkanSPIRVs(std::unordered_map<GLenum, std::vector<uint32_t>>& vulkanSPIRVs, bool force /*= false*/) {
		PST_PROFILE_FUNCTION();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);

		constexpr bool optimise = true;
		if (optimise) {
			options.SetOptimizationLevel(shaderc_optimization_level_performance);
		}


		auto& shaderData = m_OpenGLSPIRVs;
		shaderData.clear();

		for (auto&& [type, spirv] : vulkanSPIRVs) {
			std::filesystem::path cacheDirectory = Utils::CacheDirectory();
			std::filesystem::path shaderFilepath = m_Filepath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilepath.filename().string() + Utils::GLShaderTypeCachedOpenGLFileExtension(type));

			// Load in cached file if we have it
			std::ifstream inFile(cachedPath, std::ios::in | std::ios::binary);
			if (!force && inFile.is_open()) {
				inFile.seekg(0, std::ios::end);
				auto size = inFile.tellg();
				inFile.seekg(0, std::ios::beg);

				// Read binary datainto 
				auto& data = shaderData[type];
				data.resize(size / sizeof(uint32_t));
				inFile.read((char*)data.data(), size);

			} else {
				PST_CORE_TRACE("Cross compiling OpenGL {} Shader SPIR-V binaries ... ", Utils::GLShaderTypeToString(type));

				spirv_cross::CompilerGLSL glslCompiler(spirv);
				auto& source = m_OpenGLSourceCodes[type] = glslCompiler.compile();

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
					source,
					Utils::GLShaderTypeToShaderC(type),
					m_Filepath.c_str(), options
				);

				if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
					PST_CORE_CRITICAL("{} Shader compilation failed", Utils::GLShaderTypeToString(type));
					PST_CORE_ASSERT(false, module.GetErrorMessage());
				}

				shaderData[type] = std::vector<uint32_t>(module.cbegin(), module.cend());

				// Cache binary
				std::ofstream outFile(cachedPath, std::ios::out | std::ios::binary);
				if (outFile.is_open()) {
					auto& data = shaderData[type];
					outFile.write((char*)data.data(), data.size() * sizeof(uint32_t));
					outFile.flush();
					outFile.close();
				}
			}
		}
	}

	void OpenGLShader::CreateProgram() {
		PST_PROFILE_FUNCTION();

		GLuint programID = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [type, spirv] : m_OpenGLSPIRVs) {

			GLuint shaderID = glCreateShader(type);
			shaderIDs.emplace_back(shaderID);
			
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), (GLsizei)(spirv.size() * sizeof(uint32_t)));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			
			glAttachShader(programID, shaderID);
		}

		// Link our program
		glLinkProgram(programID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE) {
			GLint maxLength;
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

		// Always detach shaders after a successful link.
		for (GLuint ID : shaderIDs) {
			glDetachShader(programID, ID);
			glDeleteShader(ID);
		}

		m_RendererID = programID;
	}

	void OpenGLShader::Reflect(GLenum type, const std::vector<unsigned int>& shaderData) {
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		PST_CORE_INFO("OpenGLShader::Reflect - {}: \"{}\"", Utils::GLShaderTypeToString(type), m_Filepath);
		PST_CORE_INFO("    {} uniform buffers", resources.uniform_buffers.size());
		PST_CORE_INFO("    {} resources", resources.sampled_images.size());

		PST_CORE_INFO("Unform buffers:");
		for (const auto& resource : resources.uniform_buffers) {
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			size_t bufferSize = compiler.get_declared_struct_size(bufferType);
			unsigned int binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			size_t memberCount = bufferType.member_types.size();

			PST_CORE_INFO("  {}", resource.name);
			PST_CORE_INFO("    Size = {}", bufferSize);
			PST_CORE_INFO("    Binding = {}", binding);
			PST_CORE_INFO("    Members = {}", memberCount);
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
			PST_CORE_WARN("Warning: uniform '{}' doesn't exist!", name);
		}

		m_UniformLocationCache[name] = location;
		return location;
	}

}
