#pragma once

#include <memory>
#include <vector>

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

	class C_SceneBuilder {
	public:
		std::shared_ptr<C_Scene> LoadScene(std::shared_ptr<Scene> scene);
		std::shared_ptr<I_RenderNode> LoadMesh(const Mesh& mesh);
		std::shared_ptr<GLW::C_Texture> LoadTexture(const Texture& texture) const;
	private:
		std::vector<std::shared_ptr<GLW::C_Texture>> m_textures;
	};
};