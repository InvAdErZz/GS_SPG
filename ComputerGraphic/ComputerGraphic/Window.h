#pragma once
#include <memory>
#include "SDL_video.h"

struct SDL_Window;

struct WindowDeleter
{
	void operator()(SDL_Window* Window) noexcept;
};

struct ContextDeleter
{
	void operator()(SDL_GLContext context) noexcept;
};

class Window
{
public:
	bool Init(const char *title, int xPositon, int yPosition, int width, int height, Uint32 flags);
	void CreateContext();
	bool IsValid() const;
	void SwapWindow();
private:
	std::unique_ptr<SDL_Window, WindowDeleter> m_sdlWindowPtr;
	std::unique_ptr<std::remove_pointer<SDL_GLContext>, ContextDeleter> m_sdlContextPtr;
};
