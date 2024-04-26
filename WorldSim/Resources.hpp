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

#define Vec2i glm::ivec2

using namespace std;

inline auto RandomInt(int Max) { return std::rand() % Max; }

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