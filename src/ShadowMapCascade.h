#pragma once

#include <glm/glm.hpp>
#include "Scene.hpp"
#include "LightInfo.h"

#include <memory>
#include <vector>

namespace GLW {
	class C_Framebuffer;
	class C_Texture;
}

class C_LightInfo;

struct S_SplitInfo {
	double m_depth;
	glm::mat4 m_cropMat;
};

class C_ShadowMapCascade {
public:
	// not sure about near and far...
	C_ShadowMapCascade(std::shared_ptr<C_LightInfo> lightInfo, float resolution, int levels, float m_lambda = 0.5f);
	~C_ShadowMapCascade();

	unsigned int	GetNumLevels() const { return m_levels; }
	void			SetNumLevels(unsigned int val) { m_levels = val; }

	float			GetLambda() const { return m_lambda; }
	void			SetLambda(float val);
	void			ChangeLambda(float diff);

	//TODO set split resolution etc

	void RecalcAll();

	const std::vector<float> GetPlanes() const;

	void	PrintSplittingDepths() const;
	void	DebugDrawAABBs(const glm::mat4& projectionMatrix) const;

	glm::mat4 GetViewProjection() const { return m_lighInfo->GetViewProjectionMatrix(); }

	const S_SplitInfo& GetSplitInfo(int i) { return m_splitInfos[i]; }
private:
	void CalcSplitPlanes();
	void CalcCropMatrices();

	unsigned int									m_levels;
	float											m_resolution;
	float											m_lambda;
	std::shared_ptr<GLW::C_Framebuffer>				m_framebuffer;
	std::vector<std::shared_ptr<GLW::C_Texture>>	m_depthMaps;
	std::vector<float>								m_splitingPlanes;
	std::vector<S_SplitInfo>						m_splitInfos;
	std::vector<glm::mat4>							m_LightClippingMatrices;
	std::shared_ptr<C_LightInfo>					m_lighInfo;

	//for debug purposes
	std::vector<AABB>								m_bboxes;
};