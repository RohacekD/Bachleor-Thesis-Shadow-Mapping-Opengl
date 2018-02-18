#pragma once

#include <gl/glew.h>

#include <string>

namespace RenderDoc {
	class C_DebugScope {
	public:
		C_DebugScope(const std::string& groupName);
		~C_DebugScope();
	};
}