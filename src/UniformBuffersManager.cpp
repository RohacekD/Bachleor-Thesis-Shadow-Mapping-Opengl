#include "UniformBuffersManager.h"

#include "GLW/Buffers/UniformBuffer.h"
#include "GLW/ShaderProgram.h"
#include "Debug.h"


//=================================================================================
C_UniformBuffersManager::C_UniformBuffersManager()
{

}

//=================================================================================
C_UniformBuffersManager& C_UniformBuffersManager::Instance()
{
	static C_UniformBuffersManager    instance; // Guaranteed to be destroyed.
										// Instantiated on first use.
	return instance;
}

//=================================================================================
void C_UniformBuffersManager::PrintStatistics() const
{

}

//=================================================================================
void C_UniformBuffersManager::Clear()
{
	m_UBOs.clear();
	DestructorFullCheck();
}

//=================================================================================
void C_UniformBuffersManager::BindUBOs(const GLW::C_ShaderProgram* program) const
{
	assert(program);
	for (const auto& ubo : m_UBOs) {
		if (ubo->IsActive()) {
			program->BindUBO(ubo);
		}
	}
}

//=================================================================================
void C_UniformBuffersManager::ProcessUBOBindingPoints(std::shared_ptr<GLW::C_ShaderProgram> program) const
{
	int i = 0;
	for (const auto & ubo : m_UBOs)
	{
		auto uniformBlockIndex = program->FindUniformBlockLocation<const std::string&>(ubo->GetBlockName());
		if (uniformBlockIndex != GL_INVALID_INDEX) {
			glUniformBlockBinding(program->GetProgram(), uniformBlockIndex, i);
		}
		++i;
	}
}
