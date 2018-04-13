// ComputerGraphic.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "Window.h"
#include "Scene.h"
#include <chrono>
#include "InputManager.h"
#include "CheckGl.h"
#include "MeshData.h"


Window window;
Scene scene;
InputManager inputManager;

void Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::printf("SDL Init Failed!");
		exit(1);
	}

	glm::ivec2 resultion = { 1200,900 };

	// Create an application window with the following settings:
	window.Init(
		"An SDL2 window",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		resultion.x,                               // width, in pixels
		resultion.y,                               // height, in pixels
		SDL_WINDOW_OPENGL                  // flags - see below
	);

	if (!window.IsValid()) {
		exit(1);
	}

	window.CreateContext();

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// This makes our buffer swap syncronized with the monitor's vertical refresh
	SDL_GL_SetSwapInterval(1);

	glewExperimental = GL_TRUE;
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEBUG_OUTPUT);

	glDebugMessageCallback(errorCallback, nullptr);

	// SUPER VERBOSE DEBUGGING!
	/*if (glDebugMessageControlARB != NULL) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
		glDebugMessageCallbackARB(ETB_GL_ERROR_CALLBACK, nullptr);
		
	}
*/
	scene.Init(resultion);
	glClearColor(0.5, 0.5, 0.5, 1.0);
}

void CleanUp()
{
	SDL_Quit();
}

void Render()
{
	scene.Render();
	window.SwapWindow();
}

void HandleEvent(const SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
		inputManager.UpdateKeyDown(event.key);
		break;
	case SDL_KEYUP:
		inputManager.UpdateKeyUp(event.key);
		break;
	case SDL_EventType::SDL_MOUSEMOTION:
		inputManager.UpdateMouse(event.motion);
		break;
	default:
		break;
	}
}

int main()
{
	using clock = std::chrono::high_resolution_clock;
	using Duration = std::chrono::duration<float>;
	constexpr Duration framTime(1.0f/60.f);
	Init();
	
	auto previousTime = clock::now();
	auto currentTime = clock::now();

	SDL_Event event;
	while (true)
	{
		inputManager.StartNewFrame();
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_EventType::SDL_QUIT)
			{
				goto breakOuterLoop;
			}
			else
			{
				HandleEvent(event);
			}
		}
		previousTime = currentTime;
		currentTime = clock::now();
		Duration deltaTime = (currentTime - previousTime);
		scene.Update(deltaTime.count(), inputManager);
		Render();
		Duration remainingFrameTime = framTime - deltaTime;
		if (remainingFrameTime.count() > 0)
		{
			SDL_Delay(static_cast<unsigned int>(remainingFrameTime.count() * 1000));
		}
		ASSERT_GL_ERROR_MACRO();		
	}
	breakOuterLoop:
	ASSERT_GL_ERROR_MACRO();
	//CleanUp(); // Temp fix this can only be called after all gl stuff is done, ie destructors of currently static objects have run -> not possible at the moment
	return 0;
}

