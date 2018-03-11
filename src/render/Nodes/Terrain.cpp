#include "render/Nodes/Terrain.h"

namespace render {


	//=================================================================================
	C_Terrain::C_Terrain()
	{

	}

	//=================================================================================
	C_Terrain::~C_Terrain()
	{

	}

	//=================================================================================
	void C_Terrain::Render(const S_RenderParams& params)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	//=================================================================================
	void C_Terrain::Clean()
	{
		throw std::logic_error("The method or operation is not implemented.");
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
		throw std::logic_error("The method or operation is not implemented.");
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

}