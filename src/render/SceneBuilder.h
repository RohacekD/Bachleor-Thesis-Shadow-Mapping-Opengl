#pragma once

#include <pugixml/pugixml.hpp>

#include <memory>
#include <vector>
#include <string>

struct Scene;
struct Mesh;
struct Texture;

namespace GLW {
	class C_Texture;
}

namespace render {
	class C_MeshNode;
	class I_RenderNode;
	class C_Scene;
	class C_Terrain;

	class C_SceneBuilder {
	public:
		~C_SceneBuilder();
		std::shared_ptr<C_Scene> LoadScene(const std::string& sceneDefinitionFile);
		std::shared_ptr<C_Terrain> LoadTerrain(const pugi::xml_node& node);
		std::shared_ptr<C_Scene> LoadModel(const pugi::xml_node& node);
		std::shared_ptr<I_RenderNode> LoadMesh(const Mesh& mesh);
		std::shared_ptr<GLW::C_Texture> LoadTexture(const Texture& texture) const;
	private:
		std::string GetFolderpath(const std::string& filePath) const;

		std::vector<std::shared_ptr<GLW::C_Texture>> m_textures;
		std::shared_ptr<GLW::C_Texture> m_nullTexture;
		std::shared_ptr<Scene> m_scene;
		std::string m_sceneFolder;
	};
};