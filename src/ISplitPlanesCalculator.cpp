#include "ISplitPlanesCalculator.h"

#include "GLW/Buffers/ShaderStorage.h"


//=================================================================================
I_SplitPlanesCalculator::I_SplitPlanesCalculator(std::shared_ptr<I_Camera> camera)
	: m_ratios(std::make_shared<C_RatiosCalculator>(camera))
	, m_camera(camera)
{
	m_camera->Subscribe(m_ratios);
	//TODO
	m_SplitFrust = std::make_shared<C_SplitPlanesStorage>(4, 4);
}

//=================================================================================
I_SplitPlanesCalculator::~I_SplitPlanesCalculator()
{
	m_camera->Unsubscribe(m_ratios);
	m_SplitFrust.reset();
}

//=================================================================================
void I_SplitPlanesCalculator::BindBuffer(float activate /*= true*/)
{
	if (activate) {
		m_SplitFrust->bind();
	}
	else {
		m_SplitFrust->unbind();
	}
}

//=================================================================================
I_SplitPlanesCalculator::C_RatiosCalculator::C_RatiosCalculator(std::shared_ptr<I_Camera> camera, float lambda /*= 0.5f*/)
	: m_lambda(lambda)
	, m_changed(true)
	, m_camera(camera)
{
	
}

//=================================================================================
I_SplitPlanesCalculator::C_RatiosCalculator::~C_RatiosCalculator()
{
}

//=================================================================================
void I_SplitPlanesCalculator::C_RatiosCalculator::SetLambda(float val)
{
	if (m_lambda != val) {
		m_lambda = val; 
		m_changed = true;
	}
}

//=================================================================================
std::vector<float> I_SplitPlanesCalculator::C_RatiosCalculator::CalcRations(unsigned int splits)
{
	auto ratios = std::vector<float>(splits);
	float logArgument = (m_camera->GetFar() / m_camera->GetNear());
	float uniArgument = (m_camera->GetFar() - m_camera->GetNear());
	for (unsigned int i = 1; i <= splits; ++i) {
		float parameter = static_cast<float>(i) / static_cast<float>(splits);
		double result = (GetLambda() *(m_camera->GetNear() * std::pow(logArgument, parameter))) + ((1.0f - GetLambda())*(m_camera->GetNear() + uniArgument * parameter));
		ratios[i-1] = static_cast<float>(result);
	}
	return ratios;
}

//=================================================================================
void I_SplitPlanesCalculator::C_RatiosCalculator::Notify(E_CameraValues value)
{
	if (value == E_CameraValues::Far || value == E_CameraValues::Near) {
		m_changed = true;
	}
}
