#pragma once

#include <string>

#include <pugixml/pugixml.hpp>

#include <glm/glm.hpp>

class CameraPath;

class C_CameraPathLoader {
public:
	CameraPath LoadPath(const std::string& file);
private:
	glm::vec3 ReadVec3Node(const pugi::xml_node& node) const noexcept;
};