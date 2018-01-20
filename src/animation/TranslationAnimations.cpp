#include "animation/TranslationAnimations.h"
#include "animation/OneAxisAnimations.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

namespace Animation {

	//=================================================================================
	glm::vec3 C_TranslationAnim::update3v(const double percentage)
	{
		return glm::vec3(m_XAnim->updatef(percentage), m_YAnim->updatef(percentage), m_ZAnim->updatef(percentage));
	}

	//=================================================================================
	glm::mat4 C_TranslationAnim::update4mat(const double percentage)
	{
		return glm::translate(glm::mat4(1.0f), update3v(percentage));
	}

	//=================================================================================
	void C_TranslationAnim::Start()
	{
		m_XAnim->Start();
		m_YAnim->Start();
		m_ZAnim->Start();
	}

}