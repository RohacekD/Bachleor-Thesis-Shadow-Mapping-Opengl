/** ==============================================
 * @file 		UniformBuffersManager.h
 * @date 		2018/03/17 20:04
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include <gl/glew.h>

#include <memory>
#include <string>
#include <vector>

namespace GLW {
	class C_UniformBuffer;
	class C_ShaderProgram;
}

/** ==============================================
 * @class C_UniformBuffersManager
 *
 * @brief	Manages UBO's over application.
 *
 * @note	This class is intended to store and bind UBO's to 
 *			shader programs. Also possibly prints some statistics
 *
 * @todo	It would be nice to extend statistics to memory usage details
 *
 * @author 	Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * @date 	2018/03/17
 ** ==============================================*/
class C_UniformBuffersManager {
public:
	//Singleton stuff
	C_UniformBuffersManager(C_UniformBuffersManager const&) = delete;
	void operator=(C_UniformBuffersManager const&) = delete;
	static C_UniformBuffersManager& Instance();

	void PrintStatistics() const;
	void Clear();

	void BindUBOs(const GLW::C_ShaderProgram* program) const;
	template<class T, typename ...Params>
	std::shared_ptr<T> CreateUniformBuffer(const std::string& name, Params&&... params);
private:
	C_UniformBuffersManager();
	std::vector<std::shared_ptr<GLW::C_UniformBuffer>> m_UBOs;
};

//=================================================================================
template<class T, typename ...Params>
std::shared_ptr<T> C_UniformBuffersManager::CreateUniformBuffer(const std::string& name, Params&&... params)
{
	auto ubo = std::make_shared<T>(name, static_cast<unsigned int>(m_UBOs.size()), std::forward<Params>(params)...);
	m_UBOs.push_back(ubo);

	return ubo;
}
