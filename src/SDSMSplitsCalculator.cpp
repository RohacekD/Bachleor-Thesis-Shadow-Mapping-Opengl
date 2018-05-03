#include "SDSMSplitsCalculator.h"

#include "Debug.h"
#include "GLW/Texture.h"
#include "GLW/ShaderProgram.h"
#include "GLW/Buffers/ShaderStorage.h"
#include "ShaderManager.h"
#include "Camera/ICamera.h"

#include <algorithm>

//=================================================================================
C_SDSMSplitsCalculator::C_SDSMSplitsCalculator(unsigned int frusrums, std::shared_ptr<I_Camera> camera)
	: m_frustums(frusrums)
	, I_SplitPlanesCalculator(camera)
{
	m_histogram = std::make_shared<C_Histogram>(256, 3);


#ifdef _DEBUG
	m_HistogramTexture = std::make_shared<GLW::C_Texture>("m_HistogramTexture");

	m_HistogramTexture->StartGroupOp();
	glTexImage2D(m_HistogramTexture->GetTarget(), 0, GL_RGBA32F, 256, 150, 0, GL_RGBA, GL_FLOAT, 0);
	m_HistogramTexture->SetDimensions({ 256, 150 });

	ErrorCheck();
	m_HistogramTexture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	m_HistogramTexture->SetFilter(GL_NEAREST, GL_NEAREST);
	ErrorCheck();

	m_HistogramTexture->EndGroupOp();
#endif // _DEBUG

}

//=================================================================================
C_SDSMSplitsCalculator::~C_SDSMSplitsCalculator()
{
#ifdef _DEBUG
	m_HistogramTexture = nullptr;
#endif
	m_histogram = nullptr;
}

//=================================================================================
void C_SDSMSplitsCalculator::RecalcSplits(std::shared_ptr<GLW::C_Texture> depthMap)
{
	auto& shdManager = C_ShaderManager::Instance();
	{
		RenderDoc::C_DebugScope scope("Compute shader");
		auto program = shdManager.GetProgram("compute-splits");
		shdManager.ActivateShader(program);
		program->SetUniform("nearFar", glm::vec2(m_camera->GetNear(), m_camera->GetFar()));
		program->SetUniform("globalSize", glm::ivec2(512, 512));
		m_histogram->bind();
		m_histogram->ClearBuffer();

		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		glActiveTexture(GL_TEXTURE0);
		depthMap->bind();
		glDispatchCompute(512 / 16, 512 / 16, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		depthMap->unbind();
	}
	{
		RenderDoc::C_DebugScope scope("Compute shader splits");
		auto program = shdManager.GetProgram("calcSplits");
		shdManager.ActivateShader(program);
		//todo
		auto planes = m_ratios->CalcRations(m_frustums);

		const auto func = [](float value, float from1, float to1, float from2, float to2) {
			return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
		};

		std::for_each(planes.begin(), planes.end(), [func, this](float& val) {
			val = func(val, m_camera->GetNear(), m_camera->GetFar(), 0.0f, 1.0f);
		});

		program->SetUniform("splitRatios", planes);
		program->SetUniform("nearFar", glm::vec2(m_camera->GetNear(), m_camera->GetFar()));
		m_histogram->bind();

		m_SplitFrust->bind();
		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		m_SplitFrust->unbind();
	}
#ifdef _DEBUG
	{
		RenderDoc::C_DebugScope scope("Compute shader draw");
		auto program = C_ShaderManager::Instance().GetProgram("histagraDrawColor");
		shdManager.ActivateShader(program);

		m_SplitFrust->bind();
		program->SetUniform("nearFar", glm::vec2(m_camera->GetNear(), m_camera->GetFar()));
		glActiveTexture(GL_TEXTURE0);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glBindImageTexture(0, m_HistogramTexture->GetTexture(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glDispatchCompute(256, 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		m_histogram->ClearBuffer();
		m_histogram->unbind();
		m_SplitFrust->unbind();
		m_HistogramTexture->unbind();
	}
#endif
}

//=================================================================================
#ifdef _DEBUG
std::shared_ptr<GLW::C_Texture> C_SDSMSplitsCalculator::GetHistogramTexture() const
{
	return m_HistogramTexture;
}
#endif // _DEBUG


//=================================================================================
const I_SplitPlanesCalculator::T_FrustVector& C_SDSMSplitsCalculator::GetSplitFrusts()
{
	m_SplitFrust->DownloadData();
	m_frusts = m_SplitFrust->m_Frustums;
	return m_frusts;
}

//=================================================================================
I_SplitPlanesCalculator::E_MethodType C_SDSMSplitsCalculator::MethodType() const
{
	return I_SplitPlanesCalculator::E_MethodType::SDSM;
}
