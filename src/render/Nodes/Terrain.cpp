#include "render/Nodes/Terrain.h"

#include "ShaderManager.h"
#include "GLW/ShaderProgram.h"

#include "Debug.h"

#include "Scene.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace render {


	//=================================================================================
	C_Terrain::C_Terrain(float tileSize, const Texture& texture)
		: m_x(texture.width)
		, m_z(texture.width)
		, m_tileSize(tileSize)
	{
		std::vector<glm::vec4> terrainMatrix;
		terrainMatrix.reserve(m_x*m_z);
		for (unsigned int i = 0; i < m_x; ++i) {
			for (unsigned int j = 0; j < m_z; ++j) {
				auto height = static_cast<float>((texture.data.get())[(m_x * i + j) * 4]) / std::numeric_limits<unsigned char>::max();
				terrainMatrix.emplace_back(glm::vec4( i*tileSize, height, j*tileSize, 1.0f ));
			}
		}

		std::vector<glm::vec3> normals;
		normals.reserve(m_x*m_z);
		for (unsigned int i = 0; i < m_x; ++i) {
			for (unsigned int j = 0; j < m_z; ++j) {
				auto hL = (i < 0) ? terrainMatrix[m_x * (i - 1) + j].z : 0.0f;
				auto hR = (i>m_x-1) ? terrainMatrix[m_x * (i+1) + j].z : 0.0f;
				auto hU = (j < 0) ? terrainMatrix[m_x * i + j + 1].z : 0.0f;
				auto hD = (j > m_z - 1) ? terrainMatrix[m_x * i + j - 1].z : 0.0f;

				normals.emplace_back(glm::normalize(glm::vec3(hL - hR, hD - hU, 2.0f)));
			}
		}

		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < m_x ; i++) {
			for (unsigned int j = 0; j < m_z; j++) {
				indices.push_back(m_x*i + j);
				indices.push_back(m_x*(i + 1) + j);
			}
			indices.push_back(0xffffffff);
		}

		m_indicesNum = indices.size();

		ErrorCheck();

		glGenVertexArrays(1, &m_VAO);

		glGenBuffers(1, &m_VBOs[0]);
		glGenBuffers(1, &m_VBOs[1]);
		glGenBuffers(1, &m_VBOs[2]);
		glGenBuffers(1, &m_elementbuffer);

		glBindVertexArray(m_VAO);

		// bind VBO in order to use
		glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[0]);

		static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3, "Platform doesn't support this directly.");

		// upload data to VBO
		glBufferData(GL_ARRAY_BUFFER, terrainMatrix.size() * sizeof(glm::vec4), terrainMatrix.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		ErrorCheck();


		glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[1]);
		// upload data to VBO
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[2]);
		//// upload data to VBO
		//glBufferData(GL_ARRAY_BUFFER, mesh.texcoords.size() * sizeof(glm::vec2), mesh.texcoords.data(), GL_STATIC_DRAW);
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		//glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		auto& shdManager = C_ShaderManager::Instance();
		m_program = shdManager.GetProgram("basic-planes");
	}

	//=================================================================================
	C_Terrain::~C_Terrain()
	{

	}

	//=================================================================================
	/**
	 * @method:    Render
	 * @fullName:  render::C_Terrain::Render
	 * @access:    virtual public 
	 * @return:    void
	 * @qualifier:
	 * @param: 	   const S_RenderParams & params
	 * @param: 	   glm::mat4 & parentModelMatrix
	 * @brief	   Terrain should be leaf node, so it have no need to call render on its child.
	 * @todo:		Leaf node structure 
	 */
	//=================================================================================
	void C_Terrain::RenderChilds(const S_RenderParams& params, const glm::mat4& modelMatrix)
	{
		
	}

	//=================================================================================
	void C_Terrain::Clean()
	{
		T_Base::Clean();
		glDeleteBuffers(1, &m_VBOs[0]);
		glDeleteBuffers(1, &m_VBOs[1]);
		glDeleteBuffers(1, &m_VBOs[2]);
		glDeleteBuffers(1, &m_elementbuffer);
		glDeleteVertexArrays(1, &m_VAO);
		DestructorFullCheck();
	}

	//=================================================================================
	/**
	 * @method:    Update
	 * @fullName:  render::C_Terrain::Update
	 * @access:    virtual public 
	 * @return:    void
	 * @param: 	   const double timeSinceLastUpdateMs
	 * @brief	   Terrain doesn't have any animation and should not have any child
	 */
	//=================================================================================
	void C_Terrain::Update(const double timeSinceLastUpdateMs)
	{
	}

	//=================================================================================
	/**
	 * @method:    PostUpdate
	 * @fullName:  render::C_Terrain::PostUpdate
	 * @access:    virtual public 
	 * @return:    void
	 * @brief	   Terrain doesn't have any animation and should not have any child
	 */
	//=================================================================================
	void C_Terrain::PostUpdate()
	{
	}

	//=================================================================================
	/**
	 * @method:    IsAABBChanged
	 * @fullName:  render::C_Terrain::IsAABBChanged
	 * @access:    virtual public 
	 * @return:    bool
	 * @qualifier: const
	 * @brief	   AABB of terrain did not change so I always return false
	 */
	//=================================================================================
	bool C_Terrain::IsAABBChanged() const
	{
		return false;
	}

	//=================================================================================
	/**
	 * @method:    GetActualModelMatrix
	 * @fullName:  render::C_Terrain::GetActualModelMatrix
	 * @access:    virtual public 
	 * @return:    const glm::mat4
	 * @qualifier: const
	 * @brief	   I doesn't change model matrix of terrain
	 */
	//=================================================================================
	const glm::mat4 C_Terrain::GetActualModelMatrix() const
	{
		throw glm::mat4(1.0f);
	}

	//=================================================================================
	void C_Terrain::RenderMyself(const S_RenderParams& params, const glm::mat4& modelMatrix)
	{
		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(0xffffffff);
		ErrorCheck();
		m_program->useProgram();

		glBindVertexArray(m_VAO);

		glDrawElements(
			GL_TRIANGLE_STRIP,      // mode
			static_cast<GLsizei>(m_indicesNum),    // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);

		glBindVertexArray(0);
		m_program->disableProgram();
		glDisable(GL_PRIMITIVE_RESTART);
	}

}