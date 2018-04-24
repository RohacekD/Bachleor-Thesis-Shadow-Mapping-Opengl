#include "CameraKeysLogger.h"

#include <iostream>
#include <thread>

//=================================================================================
C_CameraKeysLogger::C_CameraKeysLogger(float lenght)
{
	m_camPath.append_child("path").append_attribute("time").set_value(lenght);
}

//=================================================================================
void C_CameraKeysLogger::LogKeyFrame(const glm::vec3& position, const glm::vec3& viewVec, const glm::vec3& upVec /*= glm::vec3(0.0, 1.0f, 0.0)*/)
{
	std::lock_guard<std::mutex> lk(m_DocumentMutex);
	pugi::xml_node keyElement = m_camPath.child("path").append_child("key");
	CreateNode(keyElement.append_child(), position, "position");
	CreateNode(keyElement.append_child(), viewVec, "viewVec"); 
	CreateNode(keyElement.append_child(), upVec, "upVec");
	std::cout << "Added key frame #"<< std::distance(m_camPath.child("path").children("key").begin(), m_camPath.child("path").children("key").end())
		<<" to the camera path" << std::endl;
}

//=================================================================================
void C_CameraKeysLogger::DumpKeyFrames() const
{
	m_camPath.print(std::cout);
	std::thread save([this] { SaveXML(); });
	save.detach();
}

//=================================================================================
void C_CameraKeysLogger::SaveXML() const
{
	std::lock_guard<std::mutex> lk(m_DocumentMutex);
	std::cout << "Please enter name for xml file: " << std::endl;
	std::string filename;
	std::cin >> filename;
	m_camPath.save_file(filename.c_str());
}
