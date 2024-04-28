#include "Person.hpp"

Person::Person(int _Index, shared_ptr<World> _WorldPtr, shared_ptr<City> _CityPtr, vector<shared_ptr<Person>> _Parents)
{
	Index = _Index;
	WorldPtr = _WorldPtr;
	CityPtr = _CityPtr;
	Parents = _Parents;
	if (Parents.size() == 0)
	{
		int QSize = Qualities.size();
		for (int i = 0; i < QSize; i++)
		{
			Qualities[i] = RandomInt(2);
		}
		return;
	}
	vector<array<bool, 10>> ParentsQualities;
	for (int i = 0; i < Parents.size(); i++)
	{
		ParentsQualities.push_back(Parents[i]->Qualities);
	}
	int QSize = Qualities.size();
	for (int i = 0; i < QSize; i++)
	{
		Qualities[i] = ParentsQualities[RandomInt(Parents.size())][i];
	}
}

int Person::MoveToCity(shared_ptr<City> TargetCity)
{
	if (Dead) return 0;
	int MovedPersons = 1;
	CityPtr = TargetCity;
	for (int p = 0; p < Partners.size(); p++)
	{
		MovedPersons++;
		Partners[p]->MoveToCityShallow(TargetCity);
	}
	for (int c = 0; c < Children.size(); c++)
	{
		if (Children[c]->Age < 20)
		{
			MovedPersons++;
			Children[c]->MoveToCityShallow(TargetCity);
		}
	}
	return MovedPersons;
}

int Person::MoveToCityLogged(shared_ptr<City> TargetCity)
{
	if (Dead) return 0;
	int MovedPersons = 1;
	cout << "Moved Person " << Index << " (Age " << Age << ") from City " << CityPtr->Index << " to City " << TargetCity->Index << endl;
	CityPtr = TargetCity;
	for (int p = 0; p < Partners.size(); p++)
	{
		MovedPersons++;
		Partners[p]->MoveToCityShallow(TargetCity);
		cout << ">With Partner " << Partners[p]->Index << " (Age " << Partners[p]->Age << ")" << endl;
	}
	for (int c = 0; c < Children.size(); c++)
	{
		if (Children[c]->Age < 20)
		{
			MovedPersons++;
			Children[c]->MoveToCityShallow(TargetCity);
			cout << ">With Child " << Children[c]->Index << " (Age " << Children[c]->Age << ")" << endl;
		}
	}
	return MovedPersons;
}

void Person::MoveToCityShallow(shared_ptr<City> TargetCity)
{
	if (Dead) return;
	CityPtr = TargetCity;
}

float Person::GetCityMatch(shared_ptr<City> TargetCity)
{
	float CityMatch = 0;
	if (TargetCity->Population > 0)
	{
		for (int i = 0; i < Qualities.size(); i++)
		{
			CityMatch += 1 - abs(Qualities[i] - TargetCity->Qualities[i]);
		}
		CityMatch -= 5 * abs(TargetCity->Age - Age) / 80;
		CityMatch -= abs(1 - TargetCity->Population / CityPtr->Population);
		CityMatch += abs(TargetCity->DeltaPopulation / TargetCity->Population);
	}
	else
	{
		CityMatch += 1;
	}
	if (TargetCity != CityPtr)
	{
		CityMatch += 0.5;
	}
	return CityMatch;
}

void Person::Die()
{
	Dead = 1;
	for (int i = 0; i < Partners.size(); i++)
	{
		Partners[i]->Partners = {};
	}
	//for (int i = 0; i < WorldPtr->Partners.size(); i++)
	//{
	//	if (WorldPtr->Partners[i][0]->Index == Index || WorldPtr->Partners[i][1]->Index == Index)
	//	{
	//		WorldPtr->Partners.erase(WorldPtr->Partners.begin() + i);
	//	}
	//}
}
