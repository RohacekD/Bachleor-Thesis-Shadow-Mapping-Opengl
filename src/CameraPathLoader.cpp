#include "CameraPathLoader.h"

#include "CameraPath.h"


//=================================================================================
CameraPath C_CameraPathLoader::LoadPath(const std::string& file)
{
	pugi::xml_parse_result result;
	pugi::xml_document document;

	result = document.load_file(file.c_str());
	if (!(result.status == pugi::status_ok)) {
		throw std::exception("Scene description file cannot be loaded!");
	}

	CameraPath ret(document.child("path").attribute("time").as_double());

	for (auto& child : document.child("path").children("key")) {
		ret.keys.emplace_back(ReadVec3Node(child.child("position")), ReadVec3Node(child.child("viewVec")), ReadVec3Node(child.child("upVec")));
	}
	return ret;
}

//=================================================================================
glm::vec3 C_CameraPathLoader::ReadVec3Node(const pugi::xml_node& node) const noexcept
{
	return glm::vec3(node.attribute("x").as_double(0.0f), node.attribute("y").as_double(0.0f), node.attribute("z").as_double(0.0f));
}
