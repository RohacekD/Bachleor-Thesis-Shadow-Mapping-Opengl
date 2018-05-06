/** ==============================================
 * @file 		FrameStatistics.h
 * @date 		2018/05/06 15:50
 * @project 	Opengl Shadow Rendering
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
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
	void BeginFrame(double started);
	double GetStartTimeOfFrame() const { return m_StartedProgress; }
	void Stamp();
	void EndFrame();
	inline void AddDrawCall() { ++m_DrawedMeshes; }
	inline unsigned int GetNumDrawCalls() const { return m_DrawedMeshes; }
	virtual std::string Print() const = 0;
	// defines separator for columns in statistics
	virtual const std::string& GetSeparator() const = 0;
protected:
	std::vector<float>	m_Times;
private:
	int					m_CurrentStamp;
	GLuint				m_BeginQuery;
	std::vector<GLuint>	m_Queries;
	double				m_StartedProgress;
	unsigned int		m_DrawedMeshes;
};

class C_CSVFrameStatistics : public C_FrameStatistics {
public:
	virtual std::string Print() const override;
	virtual const std::string& GetSeparator() const override;
private:
	const static std::string s_separator;
};