#pragma once
#include "Resource.h"
#include <limits>


class Query : public Resource<Query>
{
public:
	static constexpr GLenum InvalidQuery = std::numeric_limits<GLenum>::max();

	Query();
	~Query();
	void Begin(GLenum queryType);
	void End();
	GLuint GetResult();

	void Create();
	void FreeResource();

private:
	GLenum m_queryType;

	Query(Query&& other) = delete;
	Query& operator=(Query&& other) = delete;
};



inline Query::Query()
	:m_queryType(InvalidQuery)
{}

Query::~Query()
{
	assert(InvalidQuery == m_queryType);
}

inline void Query::Begin(GLenum queryType)
{
	assert(InvalidQuery == m_queryType);
	m_queryType = queryType;
	glBeginQuery(m_queryType, m_handle);
	ASSERT_GL_ERROR_MACRO();
}

void Query::End()
{
	assert(InvalidQuery != m_queryType);
	glEndQuery(m_queryType);
	m_queryType = InvalidQuery;
	ASSERT_GL_ERROR_MACRO();
}

inline GLuint Query::GetResult()
{
	GLuint result;
	glGetQueryObjectuiv(m_handle, GL_QUERY_RESULT, &result);
	ASSERT_GL_ERROR_MACRO();

	return result;
}

inline void Query::Create()
{
	assert(!IsValid());
	glGenQueries(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}


inline void Query::FreeResource()
{
	assert(IsValid());
	glDeleteQueries(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}