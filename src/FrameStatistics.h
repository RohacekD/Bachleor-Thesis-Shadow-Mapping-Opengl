#pragma once

//OpenGL stuff
#include <SDL/SDL.h>
#undef main
#include <GL/glew.h>

#include <vector>
#include <string>


class C_FrameStatistics {
public:
	C_FrameStatistics();
	virtual ~C_FrameStatistics();
	void BeginFrame();
	void Stamp();
	void EndFrame();
	virtual std::string Print() const = 0;
protected:
	std::vector<float>	m_Times;
private:
	int					m_CurrentStamp;
	GLuint				m_BeginQuery;
	std::vector<GLuint>	m_Queries;
};

class C_CSVFrameStatistics : public C_FrameStatistics {
public:
	virtual std::string Print() const override;

};