#pragma once

#include "Resources.hpp"
#include "World.hpp"
#include "Person.hpp"

class City
{
public:
	int Index = 0;
	shared_ptr<World> WorldPtr;
	vector<shared_ptr<Person>> ResidentList = {};
	Vec2i Position = { 0,0 };
	array<float, 10> Qualities;
	float Age = 0;
	int Population = 0;
	int DeltaPopulation = 0;
	int Capacity = 1000000;

	City(int _Index, shared_ptr<World> _WorldPtr, Vec2i _Position);

	void UpdateResidentList();

	void UpdateQualities();

	void UpdateAge();

	vector<vector<shared_ptr<Person>>> FindPotentialPartners();
};