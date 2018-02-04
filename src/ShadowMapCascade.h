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
	float m_depth;
};

class C_ShadowMapCascade {
public:
	// not sure about near and far...
	C_ShadowMapCascade(float resolution, int levels, float nearPlane, float farPlane, float fov, float m_lambda = 0.5f);

	unsigned int	GetNumLevels() const { return m_levels; }
	void			SetNumLevels(unsigned int val) { m_levels = val; }

	float			GetLambda() const { return m_lambda; }
	void			SetLambda(float val);
	void			ChangeLambda(float diff);

	float			GetFarPlane() const { return m_farPlane; }
	void			SetFarPlane(float val);
	float			GetNearPlane() const { return m_nearPlane; }
	void			SetNearPlane(float val);
	//TODO set split resolution etc

	glm::vec4 GetPlanes() const;

	void	PrintSplittingDepths() const;
	void	DebugDrawAABBs(const glm::mat4& projectionMatrix) const;

	glm::mat4 GetViewProjection() const { return m_lighInfo->GetViewProjectionMatrix(); }

	float	GetFov() const;
	void	SetFov(float val);
private:
	void CalcSplitPlanes();
	void CalcViewMatrices();

	unsigned int									m_levels;
	float											m_resolution;
	float											m_nearPlane;
	float											m_farPlane;
	float											m_fov;
	float											m_lambda;
	std::shared_ptr<GLW::C_Framebuffer>				m_framebuffer;
	std::vector<std::shared_ptr<GLW::C_Texture>>	m_depthMaps;
	std::vector<double>								m_splitingPlanes;
	std::vector<glm::mat4>							m_LightClippingMatrices;
	std::shared_ptr<C_LightInfo>					m_lighInfo;

	//for debug purposes
	std::vector<AABB>								m_bboxes;
};