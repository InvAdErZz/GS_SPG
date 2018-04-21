// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <cstdlib>
#include <memory>
#include <limits>
#include "SDL.h"
#undef main

#define GLEW_STATIC
#include "glew.h"


#define  GLM_ENABLE_EXPERIMENTAL
#pragma warning(push) 
#pragma warning( disable : 4201) //nonstandard extension used: nameless struct/union
#include "glm.hpp"
#pragma warning(pop)

#pragma warning(push) 
#pragma warning( disable : 4201) //nonstandard extension used: nameless struct/union
#pragma warning( disable : 4309) // truncation of constant value
#include "ext.hpp"
#pragma warning(pop)

#include "tiny_obj_loader.h"

#include "stb_image.h"

#include <array>
#include <vector>


// TODO: reference additional headers your program requires here
