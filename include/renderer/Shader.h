#ifndef _SHADER__H_
#define _SHADER__H_

// STL libraries
#include <string>
#include <unordered_map>

// OpenGL libraries
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// OpenGL math
#include <glm/gtc/matrix_transform.hpp>

enum class ShaderType { none = -1, vertex = 0, fragment = 1 };

struct ShaderData
{
	std::string _vertexData;
	std::string _fragmentData;
};

class Shader
{
private:
	std::string _filepath;
	uint32_t _rendererID;
	mutable std::unordered_map<std::string, int> _uniformLocationCache;

public:
	Shader(const std::string& filepath);
	~Shader(void);

	void bind(void) const;
	void unbind(void) const;

	// Set uniforms
	void setUniform1i(const std::string& name, int v);
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

private:
	ShaderData parseShader(const std::string& filepath);
	uint16_t compileShader(uint16_t type, const std::string& source);
	uint16_t createShader(const std::string& vertexShader, const std::string& fragmentShader);

	int getUniformLocation(const std::string& name) const;
};

#endif