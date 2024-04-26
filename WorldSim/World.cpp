#include "World.hpp"

World::World(Vec2i _ScreenSize)
{
    ScreenSize = _ScreenSize;
}

void World::SetupWorld()
{
    UpdateCities();
}

void World::AddCity(shared_ptr<City> NewCity)
{
    Cities.push_back(NewCity);
}

void World::AddPerson(shared_ptr<Person> NewPerson)
{
    Persons.push_back(NewPerson);
}

void World::Update()
{
    UpdateCities();
    MakePartners();
    int MovedPersons = MovePersons();
    int MadeChildren = MakeChildren();
    const auto [Dead, NewDead] = AgePersons();
    stringstream Log;
    Log << "---Begin Log---\n";
    Log << MovedPersons << " Persons were moved\n";
    Log << MadeChildren << " Persons were made\n";
    Log << NewDead << " Persons have died\n";
    Log << "There are " << Dead << " dead Persons, and " << Persons.size() - Dead << " living Persons\n";
    Log << "There are " << Partners.size() << " Partners\n";
    for (int i = 0; i < Cities.size(); i++)
    {
        Log << "Resident Count of City " << i << ":" << Cities[i]->Population << " ( " << Cities[i]->DeltaPopulation << " )\n";
    }
    for (int i = 0; i < Persons.size(); i++)
    {
        if (Persons[i]->Partners.size() > 1) cout << "ERROR: Person " << i << " has too many partners ( " << Persons[i]->Partners.size() << " )\n";
    }
    Log << "----End Log----\n\n";
    cout << Log.str();
}

int World::MovePersons()
{
    int MovedPersons = 0;
    for (int i = 0; i < Persons.size(); i++)
    {
        if (Persons[i]->Age < 20) continue;
        float HomeCityMatch = Persons[i]->GetCityMatch(Persons[i]->CityPtr);
        IntFloatPair HomeCityMatchPair = IntFloatPair(Persons[i]->CityPtr->Index, HomeCityMatch);
        //vector<IntFloatPair> MoveOptions = {IntFloatPair(Persons[i]->CityPtr->Index,HomeCityMatch)};
        vector<IntFloatPair> MoveOptions = {};
        for (int c = 0; c < Cities.size(); c++)
        {
            float CityMatch = Persons[i]->GetCityMatch(Cities[c]);
            if (CityMatch > HomeCityMatch)
            {
                IntFloatPair MoveOption(c, CityMatch);
                MoveOptions.push_back(MoveOption);
            }
        }
        if (RandomInt(int(1 / MoveChance)) == 0 && MoveOptions.size() != 0)
        {
            IntFloatPair MaxMatch = HomeCityMatchPair;
            for (int m = 0; m < MoveOptions.size(); m++)
            {
                if (MoveOptions[m].Float > MaxMatch.Float)
                {
                    MaxMatch = MoveOptions[m];
                }
            }
            if (Persons[i]->Partners.size() > 0)
            {
                for (int p = 0; p < Persons[i]->Partners.size(); p++)
                {
                    shared_ptr<Person> Partner = Persons[i]->Partners[p];
                    if (!(MaxMatch.Float >= Partner->GetCityMatch(Cities[MaxMatch.Int]))) continue;
                }
            }
            MovedPersons += Persons[i]->MoveToCity(Cities[MaxMatch.Int]);
        }
    }
    return MovedPersons;
}

void World::UpdateCities()
{
    for (int i = 0; i < Cities.size(); i++)
    {
        Cities[i]->UpdateResidentList();
        Cities[i]->UpdateQualities();
        Cities[i]->UpdateAge();
    }
}

int World::MakeChildren()
{
    int NewPersons = 0;
    for (int i = 0; i < Partners.size(); i++)
    {
        if (RandomInt(1 / (ReproduceChance/* * (1 - Partners[i][0]->CityPtr->Population / Partners[i][0]->CityPtr->Capacity)*/)) == 0)
        {
            shared_ptr<Person> NewPerson = make_shared<Person>(Persons.size(), Partners[i][0]->WorldPtr, Partners[i][0]->CityPtr, Partners[i]);
            Partners[i][0]->Children.push_back(NewPerson);
            Partners[i][1]->Children.push_back(NewPerson);
            AddPerson(NewPerson);
            NewPersons++;
        }
    }
    return NewPersons;
}

int World::MakePartners()
{
    int NewPartners = 0;
    vector<vector<shared_ptr<Person>>> PotentialPartners = {};
    for (int c = 0; c < Cities.size(); c++)
    {
        vector<vector<shared_ptr<Person>>> CityPartners = Cities[c]->FindPotentialPartners();
        PotentialPartners.insert(PotentialPartners.end(), CityPartners.begin(), CityPartners.end());
    }
    for (int i = 0; i < PotentialPartners.size(); i++)
    {
        bool Eligible = 1;
        for(int p = 0; p < Partners.size(); p++)
        {
            if (Partners[p][0] == PotentialPartners[i][0] || Partners[p][0] == PotentialPartners[i][1]) Eligible = 0;
            if (Partners[p][1] == PotentialPartners[i][0] || Partners[p][1] == PotentialPartners[i][1]) Eligible = 0;
        }
        if (Eligible == 0) continue;
        if (RandomInt(1 / PartnerChance) == 0)
        {
            Partners.push_back(PotentialPartners[i]);
            PotentialPartners[i][0]->Partners.push_back(PotentialPartners[i][1]);
            PotentialPartners[i][1]->Partners.push_back(PotentialPartners[i][0]);
            //cout << "Made Partner of Persons " << PotentialPartners[i][0]->Index << ", " << PotentialPartners[i][1]->Index << endl;
            NewPartners++;
        }
    }
    return NewPartners;
}

tuple<int, int> World::AgePersons()
{
    int Dead = 0;
    int NewDead = 0;
    for (int i = 0; i < Persons.size(); i++)
    {
        bool AlreadyDead = 0;
        Persons[i]->Age += 1;
        if (Persons[i]->Dead) AlreadyDead = 1;
        if (Persons[i]->CheckDeath())
        {
            if (!AlreadyDead) NewDead++;
            Dead++;
        }
    }
    return {Dead, NewDead};
}
