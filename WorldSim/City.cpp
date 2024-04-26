#include "City.hpp"

City::City(int _Index, shared_ptr<World> _WorldPtr, Vec2i _Position)
{
	Index = _Index;
	WorldPtr = _WorldPtr;
	Position = _Position;
	/*ResidentList = GetResidentList();*/
}

void City::UpdateResidentList()
{
	int OldPopulation = Population;
	Population = 0;
	vector<shared_ptr<Person>> Residents = {};
	for (int i = 0; i < WorldPtr->Persons.size(); i++)
	{
		if (WorldPtr->Persons[i]->CityPtr->Index == Index)
		{
			Residents.push_back(WorldPtr->Persons[i]);
			if (!WorldPtr->Persons[i]->Dead) Population++;
		}
	}
	ResidentList = Residents;
	DeltaPopulation = Population - OldPopulation;
}

void City::UpdateQualities()
{
	array<float, 10> AverageQualities = {};
	for (int i = 0; i < ResidentList.size(); i++)
	{
		if (ResidentList[i]->Dead) continue;
		for (int q = 0; q < AverageQualities.size(); q++)
		{
			AverageQualities[q] += ResidentList[i]->Qualities[q];
		}
	}
	for (int q = 0; q < AverageQualities.size(); q++)
	{
		AverageQualities[q] /= Population;
	}
	Qualities = AverageQualities;
}

void City::UpdateAge()
{
	float AverageAge = 0;
	for (int i = 0; i < ResidentList.size(); i++)
	{
		if(!ResidentList[i]->Dead) AverageAge += ResidentList[i]->Age;
	}
	Age = AverageAge / Population;
}

vector<vector<shared_ptr<Person>>> City::FindPotentialPartners()
{
	vector<vector<shared_ptr<Person>>> Partners = {};
	if (ResidentList.size() == 0) return Partners;
	for (int i = 0; i < ResidentList.size() - 1; i++)
	{
		if (ResidentList[i]->Dead) continue;
		if (ResidentList[i]->Age < 20) continue;
		if (ResidentList[i]->Partners.size() > 0) continue;
		int j = RandomInt(ResidentList.size() - (i + 1)) + i + 1;
		if (ResidentList[j]->Dead) continue;
		if (ResidentList[j]->Age < 20) continue;
		if (ResidentList[j]->Partners.size() > 0) continue;
		bool Potential = 1;
		for (int p = 0; p < ResidentList[i]->Parents.size(); p++)
		{
			if (ResidentList[j] == ResidentList[i]->Parents[p]) Potential = 0;
		}
		for (int p = 0; p < ResidentList[i]->Partners.size(); p++)
		{
			if (ResidentList[j] == ResidentList[i]->Partners[p]) Potential = 0;
		}
		for (int c = 0; c < ResidentList[i]->Children.size(); c++)
		{
			if (ResidentList[j] == ResidentList[i]->Children[c]) Potential = 0;
		}
		if (Potential == 0) continue;

		Partners.push_back({ ResidentList[i], ResidentList[j] });
	}
	return Partners;
}
