#include "RenderDocTools.h"

namespace RenderDoc {

	//=================================================================================
	C_DebugScope::C_DebugScope(const std::string& groupName)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(groupName.length()), groupName.c_str());
	}

	//=================================================================================
	C_DebugScope::~C_DebugScope()
	{
		glPopDebugGroup();
	}

}