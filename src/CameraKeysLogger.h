#pragma once

#include <pugixml/pugixml.hpp>

#include <glm/glm.hpp>

#include <string>

class C_CameraKeysLogger {
public:
	C_CameraKeysLogger(float lenght);
	void LogKeyFrame(const glm::vec3& position, const glm::vec3& viewVec, const glm::vec3& upVec = glm::vec3(0.0, 1.0f, 0.0));
	void DumpKeyFrames() const;

protected:
	template<class T>
	void CreateNode(pugi::xml_node& node, const glm::vec3& vector, T&& name) const;
	pugi::xml_document m_camPath;
};

//=================================================================================
template<class T>
void C_CameraKeysLogger::CreateNode(pugi::xml_node& node, const glm::vec3& vector, T&& name) const
{
	node.set_name(name);
	node.append_attribute("x").set_value(vector.x);
	node.append_attribute("y").set_value(vector.y);
	node.append_attribute("z").set_value(vector.z);
}
