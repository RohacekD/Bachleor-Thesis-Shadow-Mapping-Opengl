#pragma once

#include <gl/glew.h>

#include <memory>
#include <string>
#include <vector>

namespace GLW {
	class C_UniformBuffer;
	class C_ShaderProgram;
}

class C_UniformBuffersManager {
public:
	//Singleton stuff
	C_UniformBuffersManager(C_UniformBuffersManager const&) = delete;
	void operator=(C_UniformBuffersManager const&) = delete;
	static C_UniformBuffersManager& Instance();

	void PrintStatistics() const;
	void Clear();

	void BindUBOs(const GLW::C_ShaderProgram* program) const;
	template<class T>
	std::shared_ptr<T> CreateUniformBuffer(const std::string& name);
private:
	C_UniformBuffersManager();
	std::vector<std::shared_ptr<GLW::C_UniformBuffer>> m_UBOs;
};

//=================================================================================
template<class T>
std::shared_ptr<T> C_UniformBuffersManager::CreateUniformBuffer(const std::string& name)
{
	auto ubo = std::make_shared<T>(name, m_UBOs.size());
	m_UBOs.push_back(ubo);

	return ubo;
}
