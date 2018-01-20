#pragma once


#include <memory>
#include <vector>

namespace GLW {
	class C_Framebuffer;
	class C_Texture;
}

class C_ShadowMapCascade {
public:
	// not sure about near and far...
	C_ShadowMapCascade(float resolution, int levels, float nearPlane, float farPlane, float m_lambda = 0.5f);

	unsigned int GetNumLevels() const { return m_levels; }
	void SetNumLevels(unsigned int val) { m_levels = val; }
	float GetLambda() const { return m_lambda; }
	void SetLambda(float val);
	void ChangeLambda(float diff);

	void PrintSplittingDepths() const;
	float GetFarPlane() const { return m_farPlane; }
	void SetFarPlane(float val);
	float GetNearPlane() const { return m_nearPlane; }
	void SetNearPlane(float val);
	//TODO set split resolution etc
private:
	void CalcSplitPlanes();

	unsigned int m_levels;
	float m_resolution;
	float m_nearPlane;
	float m_farPlane;
	float m_lambda;
	std::shared_ptr<GLW::C_Framebuffer> m_framebuffer;
	std::vector<std::shared_ptr<GLW::C_Texture>> m_depthMaps;
	std::vector<float> m_splitingPlanes;
};