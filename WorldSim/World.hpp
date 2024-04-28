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
	vector<shared_ptr<Person>> DeadPersons = {};
	vector<vector<shared_ptr<Person>>> Partners = {};

	vector<vector<int>> CityPopulations = {};
	vector<int> Populations = {};
	vector<vector<int>> DeltaCityPopulations = {};
	float Frames = 0;

	Vec2i ScreenSize = { 0,0 };
	float MoveChance = 0.05;
	float PartnerChance = 0.05;
	float ReproduceChance = .1;

	Camera2D WorldCamera;

	atomic_bool IsUpdating = false;

	World(Vec2i _ScreenSize);

	void SetupWorld();

	void AddCity(shared_ptr<City> NewCity);

	void AddPerson(shared_ptr<Person> NewPerson);

	void Thread();

	void Update();

	void RecordCityPopulations();

	int MovePersons();

	void UpdateCities();

	int MakeChildren();

	int MakePartners();

	int AgePersons();

	void CheckPartnerDeaths();

	void PlotInformation();

	void RaylibDraw();
};