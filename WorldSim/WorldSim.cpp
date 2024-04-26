// WorldSim.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Resources.hpp"
#include "World.hpp"
#include "City.hpp"
#include "Person.hpp"

int main()
{
    int CityCount = 20;
    int GodCount = 10;
    int PersonCount = 1000;

    srand(time(NULL));
    shared_ptr<World> HomeWorld = make_shared<World>(Vec2i{ 800,800 });
    for (int i = 0; i < CityCount; i++)
    {
        shared_ptr<City> NewCity = make_shared<City>(i, HomeWorld, Vec2i{ RandomInt(HomeWorld->ScreenSize.x), RandomInt(HomeWorld->ScreenSize.y) });
        HomeWorld->AddCity(NewCity);
    }
    vector<shared_ptr<Person>> Gods = {};
    for (int i = 0; i < GodCount; i++)
    {
        shared_ptr<Person> NewGod = make_shared<Person>(i, HomeWorld, HomeWorld->Cities[0], vector<shared_ptr<Person>>{});
        NewGod->Age = 20;
        HomeWorld->AddPerson(NewGod);
        Gods.push_back(NewGod);
    }
    for (int i = HomeWorld->Persons.size(); i < PersonCount; i++)
    {
        shared_ptr<Person> NewPerson = make_shared<Person>(i, HomeWorld, HomeWorld->Cities[RandomInt(CityCount)], vector<shared_ptr<Person>>{ Gods[RandomInt(GodCount)] });
        NewPerson->Age = RandomInt(20);
        HomeWorld->AddPerson(NewPerson);
        NewPerson->Parents[0]->Children.push_back(NewPerson);
    }

    HomeWorld->SetupWorld();
    int Frame = 0;
    while (true/* && Frame < 100*/)
    {
        HomeWorld->Update();
        Frame++;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
