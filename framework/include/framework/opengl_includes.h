#pragma once
#include "disable_all_warnings.h"
// Suppress warnings in third-party code.
DISABLE_WARNINGS_PUSH()
#include <glad/glad.h>

#ifdef WIN32
#define NOMINMAX
#include <windows.h> // Must include <windows.h> before <GL.glu.h>
#endif

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
DISABLE_WARNINGS_POP()