#include "StudentRenderer.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "animation/Animation.h"
#include "animation/TranslationAnimations.h"
#include "HighResolutionTimer.hpp"
#include "render/Nodes/Scene.h"
#include "render/Nodes/MeshNode.h"
#include "render/SceneBuilder.h"
#include "render/RenderParams.h"

#include "GLW/Texture.h"
#include "GLW/Framebuffer.h"

#include "GLW/Buffers/ShaderStorage.h"
#include "GLW/ShaderProgram.h"
#include "ShaderManager.h"
#include "Camera/ICamera.h"
#include "CameraManager.h"
#include "Application.hpp"
#include "UniformBuffersManager.h"

#include "imgui/imgui.h"

#include <glm/gtx/string_cast.hpp>

#include "Debug.h"

#include <iostream>
#include <iterator>

//if I want to do this properly I need also make changes to the shaders
//#define FBO_COLOR

const int gs_shadowMapsize = 2048;
const int StudentRenderer::gs_splits = 4;

//=================================================================================
// StudentRenderer
//=================================================================================
StudentRenderer::StudentRenderer()
	:m_bWireframe(false)
{

}

//=================================================================================
StudentRenderer::~StudentRenderer() {
	printFrameStatistics();
}

//=================================================================================
bool StudentRenderer::init(std::shared_ptr<Scene> scene, unsigned int screenWidth, unsigned int screenHeight)
{
	HighResolutionTimer timer;
	timer.reset();
	m_scene = scene;
	m_screenWidht = screenWidth;
	m_screenHeight = screenHeight;
	render::C_SceneBuilder sceneBuilder;
	m_renderScene = sceneBuilder.LoadScene("models/scene.xml");
	m_renderScene->m_bbox = scene->bbox;
	m_avg = 0.0f;
	m_frameID = 0;

	// I should pass camera just to CSM
	auto camera = Application::Instance().GetCamManager()->GetMainCamera();
	auto lightInfo = std::make_shared<C_DirectionalLight>(camera, glm::vec3(0.0f, 3.0f, 0.0f) * 15.0f, glm::vec3(0.0, 0.0, 0.0), 1.0f);
	m_animation = std::make_shared<Animation::C_Animation>(5000.0f);
	m_animation->AddComponent(std::make_shared<Animation::C_ElipseTranslateAnim>(10.0f, 10.0f));
	lightInfo->SetDirectionAnimation(m_animation);

	m_CSM = std::make_shared<C_ShadowMapCascade>(lightInfo, camera, gs_shadowMapsize, gs_splits);

	if (!initFBO()) {
		return false;
	}
	glEnable(GL_CULL_FACE);
	glClearColor(static_cast<GLclampf>(.26), static_cast<GLclampf>(.26), static_cast<GLclampf>(.26), static_cast<GLclampf>(1.0));

	std::cout << C_ShaderManager::Instance().ShadersStatistics();

	std::cout << "Loading time: " << timer.getElapsedTimeFromLastQueryMilliseconds() << "ms" << std::endl;

	return true;
}

//=================================================================================
void StudentRenderer::onUpdate(float timeSinceLastUpdateMs)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	approxRollingAverage(timeSinceLastUpdateMs);
	m_renderScene->Update(timeSinceLastUpdateMs);
	++m_frameID;
	std::cout << timeSinceLastUpdateMs << std::endl;
	m_animation->Update(timeSinceLastUpdateMs);
	m_CSM->Update();
	C_ShaderManager::Instance().Update();
}

//=================================================================================
void StudentRenderer::onKeyPressed(SDL_Keycode code)
{
	switch (code) {
	case SDLK_w:
	{
		m_bWireframe = !m_bWireframe;
		break;
	}
	case SDLK_i:
	{
		printFrameStatistics();
		break;
	}
	case  SDLK_UP:
	{
		m_CSM->ChangeLambda(0.1f);
		std::cout << m_CSM->GetLambda() << std::endl;
	}
	break;
	case  SDLK_DOWN:
	{
		m_CSM->ChangeLambda(-0.1f);
		std::cout << m_CSM->GetLambda() << std::endl;
	}
	break;
	}
}

//=================================================================================
void StudentRenderer::onWindowRedraw(const I_Camera& camera, const  glm::vec3& cameraPosition)
{
	ShowGUI();
	renderDepthSamples();
	{
		RenderDoc::C_DebugScope scope("Compute shader");
		auto program = C_ShaderManager::Instance().GetProgram("compute-splits");
		auto texture = m_DepthSamplesframebuffer->GetAttachement(GL_DEPTH_ATTACHMENT);
		program->useProgram();
		program->SetUniform("globalSize", glm::ivec2(512, 512));
		m_histogram->bind();

		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		glActiveTexture(GL_TEXTURE0);
		texture->bind();
		glDispatchCompute(512 / 16, 512 / 16, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		texture->unbind();
		program->disableProgram();
	}
	{
		RenderDoc::C_DebugScope scope("Compute shader draw");
		auto program = C_ShaderManager::Instance().GetProgram("histagraDrawColor");
		program->useProgram();

		m_SplitFrust->bind();
		glActiveTexture(GL_TEXTURE0);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glBindImageTexture(0, m_HistogramTexture->GetTexture(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glDispatchCompute(256, 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		m_histogram->ClearBuffer();
		m_histogram->unbind();
		m_SplitFrust->unbind();
		m_HistogramTexture->unbind();
		program->disableProgram();
	}




	renderToFBO(camera.getViewProjectionMatrix());


	RenderDoc::C_DebugScope scope("Render pass");

	m_CSM->ActivateUBO();

	glViewport(0, 0, m_screenWidht, m_screenHeight);

	render::S_RenderParams params;
	params.m_cameraViewProjectionMatrix = camera.getViewProjectionMatrix();
	params.m_cameraPosition = m_CSM->GetBoundCamera()->getPosition();
	params.m_shadowMap = m_framebuffer->GetAttachement(GL_DEPTH_ATTACHMENT);
	params.m_pass = render::S_RenderParams::E_PassType::RenderPass;

	m_renderScene->Render(params, glm::mat4(1.0f));
	ErrorCheck();
	m_CSM->ActivateUBO(false);

	{
		RenderDoc::C_DebugScope scope("DebugDraw");
		m_CSM->DebugDrawAABBs(camera.getViewProjectionMatrix());
		Application::Instance().GetCamManager()->DebugDraw();
	}
}

//=================================================================================
void StudentRenderer::clearStudentData()
{
	m_framebuffer.reset();
	m_DepthSamplesframebuffer.reset();
	m_scene.reset();
	m_CSM.reset();
	m_renderScene.reset();
	m_HistogramTexture.reset();
	m_histogram.reset();
	m_SplitFrust.reset();
	C_ShaderManager::Instance().Clear();
	C_DebugDraw::Instance().Clear();
	C_UniformBuffersManager::Instance().Clear();
	DestructorFullCheck(); // I have to call it here because destructor is called after OpenGL context is destroyed

	std::cout << "Clear ended after this NO OpenGL should be call" << std::endl;
}

//=================================================================================
void StudentRenderer::renderToFBO(const glm::mat4& cameraViewProjectionMatrix) const
{
	RenderDoc::C_DebugScope scope("Shadow pass");

	m_framebuffer->Bind();
	m_CSM->ActivateUBO();
#ifdef FBO_COLOR
	glDrawBuffer(GL_NONE);
#endif


	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, gs_shadowMapsize, gs_shadowMapsize);

	render::S_RenderParams params;
	params.m_pass = render::S_RenderParams::E_PassType::ShadowPass;

	glCullFace(GL_FRONT);
	m_renderScene->RenderChilds(params, glm::mat4(1.0f));
	glCullFace(GL_BACK);
	ErrorCheck();
	

	m_framebuffer->Unbind();
	m_CSM->ActivateUBO(false);
#ifdef FBO_COLOR
	glDrawBuffer(GL_BACK);
#endif
}

//=================================================================================
void StudentRenderer::renderDepthSamples() const
{
	RenderDoc::C_DebugScope scope("Z Pass");
	auto mainCam = Application::Instance().GetCamManager()->GetMainCamera();

	m_DepthSamplesframebuffer->Bind();


	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 512, 512);

	render::S_RenderParams params;
	params.m_pass = render::S_RenderParams::E_PassType::ZPass;
	params.m_cameraViewProjectionMatrix = mainCam->getViewProjectionMatrix();

	m_renderScene->RenderChilds(params, glm::mat4(1.0f));
	ErrorCheck();

	m_DepthSamplesframebuffer->Unbind();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

//=================================================================================
bool StudentRenderer::initFBO()
{
	m_framebuffer = std::make_shared<GLW::C_Framebuffer>();

	auto depthTexture = std::make_shared<GLW::C_Texture>("depthTexture", GL_TEXTURE_2D_ARRAY);
	depthTexture->StartGroupOp();
	glTexStorage3D(depthTexture->GetTarget(), 1, GL_DEPTH_COMPONENT32, gs_shadowMapsize, gs_shadowMapsize, m_CSM->GetNumLevels());

	ErrorCheck();
	depthTexture->setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	depthTexture->setFilter(GL_NEAREST, GL_NEAREST);
	glm::vec4 borderColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glTexParameterfv(depthTexture->GetTarget(), GL_TEXTURE_BORDER_COLOR, glm::value_ptr(borderColor));
	ErrorCheck();

	depthTexture->EndGroupOp();

	m_framebuffer->AttachTexture(GL_DEPTH_ATTACHMENT, depthTexture);

#ifdef FBO_COLOR
	auto colorTexture = std::make_shared<GLW::C_Texture>("colorTexture", GL_TEXTURE_2D_ARRAY);
	colorTexture->StartGroupOp();
	glTexStorage3D(colorTexture->GetTarget(), 1, GL_RGBA32F, gs_shadowMapsize, gs_shadowMapsize, m_CSM->GetNumLevels());

	ErrorCheck();
	colorTexture->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	colorTexture->setFilter(GL_NEAREST, GL_NEAREST);
	ErrorCheck();

	colorTexture->EndGroupOp();

	m_framebuffer->AttachTexture(GL_COLOR_ATTACHMENT0, colorTexture);
#endif

	m_DepthSamplesframebuffer = std::make_shared<GLW::C_Framebuffer>();

	auto depthSamplesTexture = std::make_shared<GLW::C_Texture>("depthSamplesTexture");
	depthSamplesTexture->StartGroupOp();
	glTexImage2D(depthSamplesTexture->GetTarget(),
		0,
		GL_DEPTH_COMPONENT32,
		512,
		512,
		0,
		GL_DEPTH_COMPONENT,
		GL_UNSIGNED_BYTE,
		0);

	ErrorCheck();
	depthSamplesTexture->setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	depthSamplesTexture->setFilter(GL_NEAREST, GL_NEAREST);
	ErrorCheck();

	depthSamplesTexture->EndGroupOp();

	m_DepthSamplesframebuffer->AttachTexture(GL_DEPTH_ATTACHMENT, depthSamplesTexture);

	m_histogram = std::make_shared<C_Histogram>(256, 3);



	m_HistogramTexture = std::make_shared<GLW::C_Texture>("m_HistogramTexture");
	m_HistogramTexture->StartGroupOp();
	glTexImage2D(m_HistogramTexture->GetTarget(), 0, GL_RGBA32F, 256, 150, 0, GL_RGBA, GL_FLOAT, 0);
	
	ErrorCheck();
	m_HistogramTexture->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	m_HistogramTexture->setFilter(GL_NEAREST, GL_NEAREST);
	ErrorCheck();
	
	m_HistogramTexture->EndGroupOp();

	m_SplitFrust = std::make_shared<C_SplitPlanesStorage>(gs_splits, 4);
	m_SplitFrust->m_Frustums[0] = { 0, 64 };
	m_SplitFrust->m_Frustums[1] = { 64, 128 };
	m_SplitFrust->m_Frustums[2] = { 128, 128+64 };
	m_SplitFrust->m_Frustums[3] = { 128+ 64, 256};
	m_SplitFrust->UploadData();
	m_SplitFrust->unbind();


	return true;
}

//=================================================================================
void StudentRenderer::ShowGUI()
{
	ImGui::Begin("Settings", &m_ControlPanel.m_active);
	ImGui::SliderFloat("Lambda", &m_ControlPanel.m_lambda, 0.0f, 1.0f);
	//ImGui::SliderInt("Splits", &m_ControlPanel.m_lambda, 1, 6);
	ImGui::End();
	bool active = true;
	ImGui::Begin("Depth histogram", &active);
	ImGui::Image((void*)m_HistogramTexture->GetTexture(), { 256, 150 }, { 0,1 }, {1,0});
	ImGui::End();

	m_CSM->SetLambda(m_ControlPanel.m_lambda);
}

//=================================================================================
glm::mat4 StudentRenderer::GetShadowViewMat() const
{
	float maxY = m_renderScene->m_bbox.maxPoint.y;
	return glm::lookAt(glm::vec3(0, maxY + 0.5f, 0.0f), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
}

//=================================================================================
glm::mat4 StudentRenderer::GetShadowProjectionMat() const
{
	AABB box = m_renderScene->m_bbox.getTransformedAABB(GetShadowViewMat());

	return glm::ortho(box.minPoint.x, box.maxPoint.x, box.minPoint.y, box.maxPoint.y, 0.5f, -box.minPoint.z);

}

//=================================================================================
glm::mat4 StudentRenderer::ScreenToTextureCoord() const
{
	glm::mat4 offset(1.0f);
	offset = glm::translate(offset, glm::vec3(0.5f, 0.5f, 0.5f));
	offset = glm::scale(offset, glm::vec3(0.5f, 0.5f, 0.5f));
	return offset;
}

//=================================================================================
inline double StudentRenderer::approxRollingAverage(double new_sample) {
	m_avg -= m_avg / s_avgFramesNum;
	m_avg += new_sample / s_avgFramesNum;

	return m_avg;
}

//=================================================================================
void StudentRenderer::printFrameStatistics() const
{
	using std::cout;
	using std::endl;
	cout << "Frame: " << m_frameID << endl;
	cout << "Approx avg frame time: " << m_avg << "ms" << endl;
	if (s_avgFramesNum > m_frameID) {
		cout << "Statistic irrelevant because too few frames passed" << endl;
	}
}
