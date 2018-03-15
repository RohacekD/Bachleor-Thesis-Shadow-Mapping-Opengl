#include "render/SceneBuilder.h"

#include "render/Nodes/MeshNode.h"
#include "render/Nodes/RenderNode.h"
#include "render/Nodes/Scene.h"
#include "render/Nodes/Terrain.h"
#include "GLW/Texture.h"

#include "TextureLoader.hpp"
#include "SceneLoader.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Scene.hpp"
#include "Debug.h"

#include <iostream>

namespace render {

	//=================================================================================
	C_SceneBuilder::C_SceneBuilder()
	{
		m_nullTexture = std::make_shared<GLW::C_Texture>();
		m_nullTexture->StartGroupOp();
		m_nullTexture->setFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		m_nullTexture->setWrap(GL_REPEAT, GL_REPEAT);
		GLubyte data[] = { 0, 0, 0, 255 };
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		m_nullTexture->EndGroupOp();
	}

	//=================================================================================
	C_SceneBuilder::~C_SceneBuilder()
	{
		DestructorFullCheck();
	}

	//=================================================================================
	std::shared_ptr<render::C_Scene> C_SceneBuilder::LoadScene(const std::string& sceneDefinitionFile)
	{
		m_sceneFolder = GetFolderpath(sceneDefinitionFile);

		pugi::xml_parse_result result;
		pugi::xml_document document;

		result = document.load_file(sceneDefinitionFile.c_str());
		if (!(result.status == pugi::status_ok)) {
			throw std::exception("Scene description file cannot be loaded!");
		}
		auto scene = std::make_shared<render::C_Scene>();

		for (auto& child : document.child("scene").children()) {
			if (!strcmp(child.name(), "terrain")) {
				scene->emplace_back(LoadTerrain(child));
			}
			else if (!strcmp(child.name(), "model")) {
				scene->emplace_back(LoadModel(child));
			}
		}

		return scene;
	}

	//=================================================================================
	std::shared_ptr<render::C_Terrain> C_SceneBuilder::LoadTerrain(const pugi::xml_node& node)
	{
		assert(strcmp(node.name(), "terrain") == 0);
		auto fileAtt = node.attribute("file");

		TextureLoader tl;

		Texture t;
		bool retval = tl.loadTexture((m_sceneFolder + "/" + node.attribute("file").as_string()).c_str(), t);


		if (!retval)
		{
			throw std::exception("terrain height map does not exists");
		}

		return std::make_shared<render::C_Terrain>(node.attribute("tile-size").as_float(1.0f), t);
	}

	//=================================================================================
	std::shared_ptr<C_Scene> C_SceneBuilder::LoadModel(const pugi::xml_node& node)
	{
		std::unique_ptr<SceneLoader> sl = std::unique_ptr<SceneLoader>(new SceneLoader);

		std::shared_ptr<Scene> scene = std::make_shared<Scene>();

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		auto positionNode = node.child("position");
		if (positionNode) {
			modelMatrix = glm::translate(modelMatrix, ReadPositionNode(positionNode));
		}

		if (!sl->addModelFromFileToScene(m_sceneFolder.c_str(), node.attribute("file").as_string(), scene, modelMatrix))
		{
			std::cerr << "Unable to load model " << std::endl; 
			return nullptr;
		}

		m_scene = scene;


		std::shared_ptr<C_Scene> ret = std::make_shared<C_Scene>();
		for (const auto&texture : scene->textures) {
			m_textures.push_back(LoadTexture(texture));
		}
		for (const auto &mesh : scene->meshes) {
			const auto& node = LoadMesh(mesh);
			try
			{
				ret->push_back(std::move(node));
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
		}

		return ret;
	}

	//=================================================================================
	std::shared_ptr<I_RenderNode> C_SceneBuilder::LoadMesh(const Mesh& mesh)
	{
		auto node = std::make_shared<C_MeshNode>();
		node->LoadMesh(mesh);
		auto material = m_scene->materials[mesh.materialIndex];
		if(material.textureIndex != -1)
		node->SetTexture(m_textures[material.textureIndex]);
		else
		{
			node->SetTexture(m_nullTexture);
		}
		return node;
	}

	//=================================================================================
	std::shared_ptr<GLW::C_Texture> C_SceneBuilder::LoadTexture(const Texture & texture) const
	{
		std::shared_ptr<GLW::C_Texture> tex = std::make_shared<GLW::C_Texture>(texture.m_name);
		tex->StartGroupOp();
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			(GLsizei)texture.width,
			(GLsizei)texture.height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			texture.data.get());
		ErrorCheck();
		tex->setWrap(GL_REPEAT, GL_REPEAT);
		tex->setFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		ErrorCheck();

		tex->EndGroupOp();

		return tex;
	}

	//=================================================================================
	glm::vec3 C_SceneBuilder::ReadPositionNode(const pugi::xml_node& node) const noexcept
	{
		assert(!strcmp(node.name(), "position"));
		return glm::vec3(node.attribute("x").as_double(0.0f), node.attribute("y").as_double(0.0f), node.attribute("z").as_double(0.0f));
	}

	//=================================================================================
	std::string C_SceneBuilder::GetFolderpath(const std::string& filePath) const
	{
		std::size_t found = filePath.find_last_of("/\\");

		return filePath.substr(0, found);
	}

};