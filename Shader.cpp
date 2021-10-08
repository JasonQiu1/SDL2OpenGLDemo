#include "Shader.h"
#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath) 
	: ID(glCreateProgram())
{
	std::string vertexCode, fragmentCode;
	std::ifstream vShaderFile, fShaderFile;

	vShaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	fShaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure f) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		exit(-1);
	}

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	int success;
	char infoLog[512];

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED: " << infoLog;
		exit(-1);
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: " << infoLog;
		exit(-1);
	}

	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::LINK_FAILED: " << infoLog;
		exit(-1);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
}

void Shader::setFloat(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set3fv(const std::string& name, int count, glm::vec3 value) {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), count, glm::value_ptr(value));
}

void Shader::set4fv(const std::string& name, int count, glm::vec4 value) {
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), count, glm::value_ptr(value));
}
void Shader::set4f(const std::string& name, float f1, float f2, float f3, float f4) {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), f1, f2, f3, f4);
}

void Shader::setMat4fv(const std::string& name, int count, bool transpose, glm::mat4 value) {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), count, transpose, glm::value_ptr(value));
}