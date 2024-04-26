#pragma once

#include "Resources.hpp"
#include "City.hpp"
#include "World.hpp"

class Person
{
public:
	int Index = -1;
	shared_ptr<World> WorldPtr;
	shared_ptr<City> CityPtr;
	float Age = 0;
	bool Dead = 0;
	vector<shared_ptr<Person>> Parents = {};
	vector<shared_ptr<Person>> Children = {};
	vector<shared_ptr<Person>> Partners = {};
	array<bool, 10> Qualities;

	Person(int _Index, shared_ptr<World> _WorldPtr, shared_ptr<City> _CityPtr, vector<shared_ptr<Person>> _Parents);

	int MoveToCity(shared_ptr<City> TargetCity);

	int MoveToCityLogged(shared_ptr<City> TargetCity);

	void MoveToCityShallow(shared_ptr<City> TargetCity);

	float GetCityMatch(shared_ptr<City> MatchCity);

	bool CheckDeath();

	void Die();
};