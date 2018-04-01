#pragma once

#include <string>
#include <map>

class C_ShaderPreprocessor {
public:
	using T_Defines = std::map<std::string, std::string>;
	void Define(const std::string& symbol, const std::string& value);
	std::string PreprocessFile(const std::string& src, const std::string& filepath);
private:
	T_Defines m_defines;
	void IncludesFiles(std::string& content, const std::string& filepath);
	void ReplaceConstants(std::string& content);
	void GetDefines(std::string& content);

	bool _loadFile(const char* file, std::string& content);
};