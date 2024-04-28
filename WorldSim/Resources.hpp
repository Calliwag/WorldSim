#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <thread>
#include <raylib.h>

#define Vec2i glm::ivec2

using namespace std;

inline auto RandomInt(int Max) { return std::rand() % max(1,Max); }

template<typename T>
inline void VectorRemove(vector<T>& vec, size_t index) {
	vec[index] = move(vec.back());
	vec.pop_back();
}

struct IntFloatPair
{
	int Int;
	float Float;
	IntFloatPair(int _Int, float _Float)
	{
		Int = _Int;
		Float = _Float;
	}
};

class Person;
class City;
class World;