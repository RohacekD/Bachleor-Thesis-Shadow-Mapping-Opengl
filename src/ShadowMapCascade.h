/** ==============================================
 * @file 		ShadowMapCascade.h
 * @date 		2018/03/17 20:02
 * @project 	Computer Graphics Project
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include <glm/glm.hpp>
#include "Scene.hpp"
#include "LightInfo.h"
#include "PSSMUBO.h"

#include "PSSMSplitsCalculator.h"

#include <memory>
#include <vector>

namespace GLW {
	class C_Framebuffer;
	class C_Texture;
}

class C_SplitPlanesStorage;
class C_LightInfo;

struct S_SplitInfo {
	double m_depth;
	glm::mat4 m_cropMat;
	glm::mat4 m_lightViewProjectionMatrix;
};

class C_ShadowMapCascade {
public:
	// not sure about near and far...
	C_ShadowMapCascade(std::shared_ptr<C_LightInfo> lightInfo, std::shared_ptr<I_Camera> camera, unsigned int defaultResolution, unsigned int levels, float m_lambda = 0.5f);
	~C_ShadowMapCascade();

	// not working just now
	unsigned int	GetNumLevels() const { return m_levels; }
	void			SetNumLevels(unsigned int val) { m_levels = val; }

	std::shared_ptr<I_Camera> GetBoundCamera() const { return m_boundCamera; }
	void SetBoundCamera(std::shared_ptr<I_Camera> val) { m_boundCamera = val; }

	void			ActivateUBO(bool activate = true);

	float			GetLambda() const { return m_lambda; }
	void			SetLambda(float val);
	void			ChangeLambda(float diff);

	void			Update();

	//@TODO set split resolution etc

	void RecalcAll();

	const std::vector<float> GetPlanes() const;

	void	PrintSplittingDepths() const;
	void	DebugDrawAABBs(const glm::mat4& projectionMatrix) const;

	glm::mat4 GetViewProjection() const { return m_lighInfo->GetViewProjectionMatrix(); }

	const S_SplitInfo& GetSplitInfo(int i) { return m_splitInfos[i]; }
private:
	void CalcCropMatrices();

	C_PSSMSplitsCalculator m_SplitCalculator;
	std::shared_ptr<C_SplitPlanesStorage> m_SplitFrust;

	unsigned int									m_levels;
	unsigned int									m_resolution;
	float											m_lambda;
	std::shared_ptr<GLW::C_Framebuffer>				m_framebuffer;
	std::vector<std::shared_ptr<GLW::C_Texture>>	m_depthMaps;
	std::vector<float>								m_splitingPlanes;
	std::vector<S_SplitInfo>						m_splitInfos;
	std::vector<glm::mat4>							m_LightClippingMatrices;
	std::shared_ptr<C_LightInfo>					m_lighInfo;
	std::shared_ptr<I_Camera>						m_boundCamera;
	std::shared_ptr<C_PSSMUBO<4>>					m_PSSSMUBO;

	//for debug purposes
	std::vector<AABB>								m_bboxes;
};