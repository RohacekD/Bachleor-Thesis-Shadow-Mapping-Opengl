#include "ShaderManager.h"

#include "GLW/ShaderProgram.h"


#include <iostream>
#include <sstream>


const std::string C_ShaderManager::s_ShadersFolder = "shaders/";
ShaderCompiler C_ShaderManager::s_Compiler;

//=================================================================================
C_ShaderManager & C_ShaderManager::Instance()
{
	static C_ShaderManager    instance; // Guaranteed to be destroyed.
						  // Instantiated on first use.
	return instance;
}

//=================================================================================
void C_ShaderManager::Clear()
{
	m_Programs.clear();
}

//=================================================================================
C_ShaderManager::T_ShaderPtr C_ShaderManager::GetProgram(const std::string& name)
{
	if (ShaderLoaded(name)) {
		return m_Programs[name];
	}

	std::string filePath;

	pugi::xml_document doc;

	if (!LoadDoc(doc, name)) {
		std::cerr << "C_ShaderManager: Can't open config file for shader name: " << name << std::endl;
		return nullptr;
	}

	std::vector<GLuint> shaders;

	for (auto& shader : doc.child("pipeline").children("shader")) {
		GLuint shaderStage = LoadShader(shader);
		if (shaderStage == 0) {
			return nullptr;
		}
		shaders.push_back(shaderStage);
	}

	GLuint program;
	std::string str;
	if (!s_Compiler.linkProgram(program, str, shaders)) {
		std::cerr << str << std::endl;
		return false;
	}

	T_ShaderPtr shaderProgram = std::make_shared<GLW::C_ShaderProgram>(program);

	m_Programs.insert({ name, shaderProgram});

	return shaderProgram;
}

//=================================================================================
bool C_ShaderManager::ShaderLoaded(const std::string & name)
{
	return m_Programs.find(name) != m_Programs.end();
}

//=================================================================================
std::string C_ShaderManager::ShadersStatistics() const
{
	std::stringstream ss;
	ss << "Loaded shaders: " << m_Programs.size() << std::endl;
	for (auto& shader : m_Programs) {
		ss << shader.first << " used by " << shader.second.use_count() - 1 << std::endl;
		ss << "->\tHas stages:" << std::endl;
	}
	return ss.str();
}

//=================================================================================
bool C_ShaderManager::LoadDoc(pugi::xml_document & document, const std::string & filename) const
{
	pugi::xml_parse_result result;

	if (filename.find(".xml") != std::string::npos) {
		result = document.load_file(filename.c_str());
		if (!result.status == pugi::status_ok) {
			result = document.load_file((s_ShadersFolder + filename).c_str());
		}
		else {
			return true;
		}
	}
	else {
		return LoadDoc(document, filename + ".xml");
	}

	if (result.status == pugi::status_ok) return true;
	return false;
}

GLuint C_ShaderManager::LoadShader(const pugi::xml_node& node) const
{
	GLuint shader = 0;
	std::string str;

	std::string stageAttribute = node.attribute("stage").value();

	int stage = GL_VERTEX_SHADER;
	if (stageAttribute == "vertex") stage = GL_VERTEX_SHADER;
	else if (stageAttribute == "fragment") stage = GL_FRAGMENT_SHADER;
	else if (stageAttribute == "geometry") stage = GL_GEOMETRY_SHADER;
	else if (stageAttribute == "compute") stage = GL_COMPUTE_SHADER;

	if (!s_Compiler.compileShader(shader, std::string(s_ShadersFolder + std::string(node.first_child().value())).c_str(), stage, str)) {
		std::cerr << "--Compilation error" << std::endl;
		std::cerr << s_ShadersFolder + std::string(node.first_child().value()) << std::endl;
		std::cerr << str << std::endl;
		return 0;
	}
	return shader;
}
