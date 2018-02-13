#include "StudentRenderer.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "HighResolutionTimer.hpp"
#include "render/Nodes/Scene.h"
#include "render/Nodes/MeshNode.h"
#include "render/SceneBuilder.h"
#include "render/RenderParams.h"

#include "GLW/Texture.h"
#include "GLW/Framebuffer.h"
#include <iostream>
#include <iterator>

#include "ShaderManager.h"
#include "Camera/ICamera.h"
#include "CameraManager.h"
#include "Application.hpp"

#include <glm/gtx/string_cast.hpp>

#include "Debug.h"


const float gs_shadowMapsize = 2048*2.0f;

//=================================================================================
// StudentRenderer
//=================================================================================
StudentRenderer::StudentRenderer()
	:m_bWireframe(false)
{

}

//=================================================================================
StudentRenderer::~StudentRenderer() {
	ErrorCheck();
	printFrameStatistics();
	ErrorCheck();
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
	m_renderScene = sceneBuilder.LoadScene(scene);
	m_renderScene->m_bbox = scene->bbox;
	m_avg = 0.0f;
	m_frameID = 0;


	auto camera = Application::Instance().GetCamManager()->GetMainCamera();
	auto lightInfo = std::make_shared<C_DirectionalLight>(camera, glm::vec3(-1.0f, 3.0f, 2.0f) * 1000.0f, glm::vec3(0.0, 0.0, 0.0), 1.0f);

	m_CSM = std::make_shared<C_ShadowMapCascade>(lightInfo, gs_shadowMapsize, 4);

	if (!initFBO()) {
		return false;
	}

	std::cout << C_ShaderManager::Instance().ShadersStatistics();

	std::cout << "Loading time: " << timer.getElapsedTimeFromLastQueryMilliseconds() << "ms" << std::endl;

	return true;
}

//=================================================================================
void StudentRenderer::onUpdate(float timeSinceLastUpdateMs)
{
	approxRollingAverage(timeSinceLastUpdateMs);
	m_renderScene->Update(timeSinceLastUpdateMs);
	++m_frameID;
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
	//m_CSM->PrintSplittingDepths();
	renderToFBO(camera.getViewProjectionMatrix());
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 12, "Render pass");
	glClearColor(static_cast<GLclampf>(.26), static_cast<GLclampf>(.26), static_cast<GLclampf>(.26), static_cast<GLclampf>(1.0));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, m_screenWidht, m_screenHeight);

	render::S_RenderParams params;
	params.m_cameraViewProjectionMatrix = camera.getViewProjectionMatrix();
	params.m_cameraPosition = Application::Instance().GetCamManager()->GetMainCamera()->getPosition();
	params.m_shadowMap = m_framebuffer->GetAttachement(GL_DEPTH_ATTACHMENT);
	params.m_toShadowMapSpaceMatrix = ScreenToTextureCoord()*m_CSM->GetViewProjection();
	params.m_pass = render::S_RenderParams::E_PassType::E_P_RenderPass;
	m_CSM->RecalcAll();
	params.m_planes = m_CSM->GetPlanes();

	m_renderScene->Render(params);
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 10, "DebugDraw");
	m_CSM->DebugDrawAABBs(camera.getViewProjectionMatrix());
	Application::Instance().GetCamManager()->DebugDraw();
	glPopDebugGroup();
	glPopDebugGroup();
}

//=================================================================================
void StudentRenderer::clearStudentData()
{
	m_framebuffer.reset();
	m_scene.reset();
	m_CSM.reset();
	m_renderScene.reset();
	ErrorCheck();
}

//=================================================================================
void StudentRenderer::renderToFBO(const glm::mat4& cameraViewProjectionMatrix) const
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 12, "Shadow pass");
	m_framebuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, gs_shadowMapsize, gs_shadowMapsize);
	glDrawBuffer(GL_NONE);


	render::S_RenderParams params;

	params.m_cameraViewProjectionMatrix = m_CSM->GetViewProjection();
	params.m_pass = render::S_RenderParams::E_PassType::E_P_ShadowPass;

	m_renderScene->Render(params);

	m_framebuffer->Unbind();
	glDrawBuffer(GL_BACK);
	glPopDebugGroup();
}

//=================================================================================
bool StudentRenderer::initFBO()
{
	ErrorCheck();
	//static const int textureWidth = 1024;
	//static const int textureHeight = 1024;

	m_framebuffer = std::make_shared<GLW::C_Framebuffer>();

	auto depthTexture = std::make_shared<GLW::C_Texture>("depthTexture", GL_TEXTURE_2D);
	depthTexture->StartGroupOp();
	glTexImage2D(depthTexture->GetTarget(), 0, GL_DEPTH_COMPONENT16, gs_shadowMapsize, gs_shadowMapsize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	ErrorCheck();
	depthTexture->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	depthTexture->setFilter(GL_NEAREST, GL_NEAREST);
	ErrorCheck();

	depthTexture->EndGroupOp();

	m_framebuffer->AttachTexture(GL_DEPTH_ATTACHMENT, depthTexture);

	return true;
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
