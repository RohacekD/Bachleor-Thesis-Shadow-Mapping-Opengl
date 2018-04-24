#include "CameraKeysLogger.h"

#include <iostream>

//=================================================================================
C_CameraKeysLogger::C_CameraKeysLogger(float lenght)
{
	m_camPath.append_child("path").append_attribute("time").set_value(lenght);
}

//=================================================================================
void C_CameraKeysLogger::LogKeyFrame(const glm::vec3& position, const glm::vec3& viewVec, const glm::vec3& upVec /*= glm::vec3(0.0, 1.0f, 0.0)*/)
{
	pugi::xml_node keyElement = m_camPath.child("path").append_child("key");
	CreateNode(keyElement.append_child(), position, "position");
	CreateNode(keyElement.append_child(), viewVec, "viewVec"); 
	CreateNode(keyElement.append_child(), upVec, "upVec");
}

//=================================================================================
void C_CameraKeysLogger::DumpKeyFrames() const
{
	m_camPath.print(std::cout);
}
