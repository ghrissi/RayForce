#pragma once

#include <raylib.h>
#include <raymath.h>

#include <PxPhysicsAPI.h>

#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <array>

#if defined(_WIN32) || defined(_WIN64)
	#define GLFW_EXPOSE_NATIVE_WIN32
#endif

using namespace physx;

#define RESOUCES_PATH "resources/"
#define MODELS_PATH RESOUCES_PATH "models/"
#define SAHDERS_PATH RESOUCES_PATH "shaders/"

