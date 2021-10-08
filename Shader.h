#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader {
private:
	const unsigned int ID;
public:
	Shader(const char* vertexPath, const char* fragmentPath);

	void use();

	void setBool(const std::string& name, bool value);
	void setFloat(const std::string& name, float value);
	void setInt(const std::string& name, int value);

	void set3fv(const std::string& name, int count, glm::vec3 value);
	void set4fv(const std::string& name, int count, glm::vec4 value);
	void set4f(const std::string& name, float f1, float f2, float f3, float f4);
	void setMat4fv(const std::string& name, int count, bool transpose, glm::mat4 value);
};