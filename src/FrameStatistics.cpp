#include "FrameStatistics.h"

#include <sstream>


//=================================================================================
C_FrameStatistics::C_FrameStatistics()
{
	glCreateQueries(GL_TIMESTAMP, 1, &m_BeginQuery);
}

//=================================================================================
C_FrameStatistics::~C_FrameStatistics()
{
	glDeleteQueries((GLsizei)m_Queries.size(), m_Queries.data());
	glDeleteQueries(1, &m_BeginQuery);
}

//=================================================================================
void C_FrameStatistics::BeginFrame()
{
	glQueryCounter(m_BeginQuery, GL_TIMESTAMP);
	m_CurrentStamp = 0;
}

//=================================================================================
void C_FrameStatistics::Stamp()
{
	GLuint q;
	glCreateQueries(GL_TIMESTAMP, 1, &q);
	m_Queries.push_back(q);
	glQueryCounter(m_Queries.at(m_CurrentStamp), GL_TIMESTAMP);
	++m_CurrentStamp;
}

//=================================================================================
void C_FrameStatistics::EndFrame()
{
	Stamp();
	GLuint64 beginTime = 0;
	glGetQueryObjectui64v(m_BeginQuery, GL_QUERY_RESULT, &beginTime);
	GLuint64 lastTime = 0;
	bool first = true;
	for (const GLuint &query : m_Queries)
	{
		GLuint64 time = 0;
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &time);
		if (first)
		{
			m_Times.push_back(float(time - beginTime)*1e-6f);
		}
		else {
			m_Times.push_back(float(time - lastTime)*1e-6f);
		}
		lastTime = time;
	}
}

//=================================================================================
std::string C_CSVFrameStatistics::Print() const
{
	std::stringstream ss;
	for (const auto& time : m_Times) {
		ss << time << ";";
	}
	std::string ret = ss.str();
	if (!ret.empty()) {
		//delete last comma
		ret = ret.substr(0, ret.size() - 1);
	}
	ret += '\n';
	return ret;
}