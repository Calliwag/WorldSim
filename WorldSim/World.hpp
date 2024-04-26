#pragma once
#include "Resources.hpp"
#include "City.hpp"
#include "Person.hpp"

using namespace std;

class World
{
public:
	vector<shared_ptr<City>> Cities = {};
	vector<shared_ptr<Person>> Persons = {};
	vector<vector<shared_ptr<Person>>> Partners = {};
	Vec2i ScreenSize = { 0,0 };
	float MoveChance = 0.01;
	float PartnerChance = 0.05;
	float ReproduceChance = 0.1;

	World(Vec2i _ScreenSize);

	void SetupWorld();

	void AddCity(shared_ptr<City> NewCity);

	void AddPerson(shared_ptr<Person> NewPerson);

	void Update();

	int MovePersons();

	void UpdateCities();

	int MakeChildren();

	int MakePartners();

	tuple<int,int> AgePersons();

	void Draw();
};