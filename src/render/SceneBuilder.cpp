#include "render/SceneBuilder.h"

#include "render/Nodes/MeshNode.h"
#include "render/Nodes/RenderNode.h"
#include "render/Nodes/Scene.h"
#include "GLW/Texture.h"

#include <GL/glew.h>

#include "Scene.hpp"
#include "Debug.h"

#include <iostream>

namespace render {

	//=================================================================================
	C_SceneBuilder::~C_SceneBuilder()
	{
		DestructorFullCheck();
	}

	//=================================================================================
	std::shared_ptr<C_Scene> C_SceneBuilder::LoadScene(std::shared_ptr<Scene> scene)
	{
		m_scene = scene;
		m_nullTexture = std::make_shared<GLW::C_Texture>();
		m_nullTexture->StartGroupOp();
		m_nullTexture->setFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		m_nullTexture->setWrap(GL_REPEAT, GL_REPEAT);
		GLubyte data[] = { 0, 0, 0, 255 };
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		m_nullTexture->EndGroupOp();


		std::shared_ptr<C_Scene> ret = std::make_shared<C_Scene>();
		for (const auto&texture : scene->textures) {
			m_textures.push_back(LoadTexture(texture));
		}
		for (const auto &mesh : scene->meshes) {
			const auto& node = LoadMesh(mesh);
			//static_cast<render::C_MeshNode*>(node.get())->SetTexture(m_earthTexture);
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
};