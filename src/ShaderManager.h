#pragma once

#include "ShaderCompiler.hpp"

#include <pugixml/pugixml.hpp>

#include <gl/glew.h>

#include <map>
#include <string>
#include <memory>

namespace GLW {
	class C_ShaderProgram;
}


// I just wanna have some statistics and also save some loading time and memory
class C_ShaderManager {
	using T_ShaderPtr = std::shared_ptr<GLW::C_ShaderProgram>;
public:
	//Singleton stuff
	C_ShaderManager(C_ShaderManager const&) = delete;
	void operator=(C_ShaderManager const&) = delete;
	static C_ShaderManager& Instance();


	T_ShaderPtr GetProgram(const std::string& name);
	bool		ShaderLoaded(const std::string& name);
	std::string ShadersStatistics() const;
private:
	C_ShaderManager() {}

	bool LoadDoc(pugi::xml_document& document, const std::string& filename) const;

	GLuint LoadShader(const pugi::xml_node& node) const;

	using T_ProgramMap = std::map<std::string, std::shared_ptr<GLW::C_ShaderProgram>>;

	T_ProgramMap m_Programs;
	const static std::string s_ShadersFolder;
	static ShaderCompiler s_Compiler;
};