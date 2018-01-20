#pragma once

#include <glm/glm.hpp>


namespace Animation {
	enum E_Affected {
		E_A_Translation,
		E_A_Rotation,
		E_A_Scale,
		E_A_OneAxisTRS,
	};

	class I_AnimationComponent {
	public:
		I_AnimationComponent() {}
		virtual ~I_AnimationComponent() {};

		virtual E_Affected	AffectedParameter() = 0;
		virtual glm::vec3	update3v(const double percentage)	{ return glm::vec3(0.0f); };
		virtual double		updatef(const double percentage)	{ return 0.0f; };
		virtual int			updatei(const double percentage)	{ return 0; };
		virtual glm::mat4	update4mat(const double percentage) { return glm::mat4(); }

		virtual void		Start() { }
	private:
	};
}