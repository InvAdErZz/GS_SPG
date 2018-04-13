#include "stdafx.h"
#include "Window.h"
#include <type_traits>

void WindowDeleter::operator()(SDL_Window* window) noexcept
{
	if (window)
	{
		SDL_DestroyWindow(window);
	}
}

void ContextDeleter::operator()(SDL_GLContext context) noexcept
{
	if (context)
	{
		SDL_GL_DeleteContext(context);
	}
}

bool Window::Init(const char *title, int xPositon, int yPosition, int width, int height, Uint32 flags)
{
	assert(!IsValid());

	m_sdlWindowPtr.reset(SDL_CreateWindow(title, xPositon, yPosition, width, height, flags));
	if (!IsValid())
	{		
		std::printf("Could not create window: %s\n", SDL_GetError());
		return false;
	}
	return true;	
}

void Window::CreateContext()
{
	assert(m_sdlWindowPtr != nullptr);
	SDL_GL_CreateContext(m_sdlWindowPtr.get());
}

bool Window::IsValid() const
{
	return m_sdlWindowPtr != nullptr;
}

void Window::SwapWindow()
{
	SDL_GL_SwapWindow(m_sdlWindowPtr.get());
}

