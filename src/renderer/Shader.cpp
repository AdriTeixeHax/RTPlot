#include <iostream>
#include <fstream>
#include <sstream>

#include "Shader.h"
#include "Renderer.h"

Shader::Shader(const std::string& filepath) :
	_filepath(filepath), _rendererID(0)
{
	ShaderData source = parseShader(filepath);
	_rendererID = createShader(source._vertexData, source._fragmentData);
}

Shader::~Shader(void)
{
	glCall(glDeleteProgram(_rendererID));
}

void Shader::bind(void) const
{
	glCall(glUseProgram(_rendererID));
}

void Shader::unbind(void) const
{
	glCall(glUseProgram(0));
}

void Shader::setUniform1i(const std::string& name, int v)
{
	glCall(glUniform1i(getUniformLocation(name), v));
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glCall(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	glCall(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]))
}

int Shader::getUniformLocation(const std::string& name) const
{
	if (_uniformLocationCache.find(name) != _uniformLocationCache.end())
		return _uniformLocationCache[name];

	glCall(int location = glGetUniformLocation(_rendererID, name.c_str()));
	if (location == -1) std::cout << "Warning: '" << name << "' doesn't exist!" << std::endl;

	_uniformLocationCache[name] = location;
	return location;
}

ShaderData Shader::parseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::none;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::vertex;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::fragment;
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str() };
}

uint16_t Shader::compileShader(uint16_t type, const std::string& source)
{
	uint16_t id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader." << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

uint16_t Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	uint16_t program = glCreateProgram();
	uint16_t vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	uint16_t fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}
