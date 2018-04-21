#pragma once
#include "glew.h"
#include "CheckGl.h"
#include <limits>

template<class DerivedClass>
class Resource {
public:
	static constexpr GLuint INVALID_HANDLE = std::numeric_limits<GLuint>::max();
	Resource() noexcept = default;
	~Resource();

	Resource(Resource&& other) noexcept;
	Resource& operator=(Resource&& other) noexcept;

	bool IsValid() const;
	const GLuint GetHandle() const { return m_handle; }

protected:
	GLuint m_handle = INVALID_HANDLE;
private:

	Resource(const Resource&) = delete;
	Resource& operator=(const Resource&) = delete;
	
};

template<class DerivedClass>
Resource<DerivedClass>::~Resource()
{
	DerivedClass* derivedThis = static_cast<DerivedClass*>(this);

	if (IsValid())
	{
		derivedThis->FreeResource();
		ASSERT_GL_ERROR_MACRO();
	}
}

template<class DerivedClass>
inline Resource<DerivedClass>::Resource(Resource<DerivedClass>&& other) noexcept
	: m_handle(other.m_handle)
{
	other.m_handle = INVALID_HANDLE;
}

template<class DerivedClass>
inline Resource<DerivedClass>& Resource<DerivedClass>::operator=(Resource<DerivedClass>&& other) noexcept
{
	std::swap(m_handle, other.m_handle);
	return *this;
}

template<class DerivedClass>
inline bool Resource<DerivedClass>::IsValid() const
{
	return m_handle != INVALID_HANDLE;
}