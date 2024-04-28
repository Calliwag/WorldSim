#include "World.hpp"

World::World(Vec2i _ScreenSize)
{
    ScreenSize = _ScreenSize;
}

void World::SetupWorld()
{
    WorldCamera.offset = { 0,0 };
    WorldCamera.rotation = 0;
    WorldCamera.target = { 0,0 };
    WorldCamera.zoom = 1;
    UpdateCities();
}

void World::AddCity(shared_ptr<City> NewCity)
{
    Cities.push_back(NewCity);
    CityPopulations.push_back({});
    DeltaCityPopulations.push_back({});
}

void World::AddPerson(shared_ptr<Person> NewPerson)
{
    Persons.push_back(NewPerson);
}

void World::Thread()
{
    while (true)
    {
        if (IsUpdating)
        {
            Update();
            IsUpdating = false;
        }
    }
}

void World::Update()
{
    Frames++;
    RecordCityPopulations();
    UpdateCities();
    MakePartners();
    int MovedPersons = MovePersons();
    int MadeChildren = MakeChildren();
    int NewDead = AgePersons();
    stringstream Log;
    Log << "---Begin Log---\n";
    Log << MovedPersons << " Persons were moved\n";
    Log << MadeChildren << " Persons were made\n";
    Log << NewDead << " Persons have died\n";
    Log << "There are " << DeadPersons.size() << " dead Persons, and " << Persons.size() << " living Persons\n";
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

void World::RecordCityPopulations()
{
    for (int i = 0; i < Cities.size(); i++)
    {
        CityPopulations[i].push_back(Cities[i]->Population);
        
        if (CityPopulations[i].size() >= 2)
        {
            DeltaCityPopulations[i].push_back(CityPopulations[i][CityPopulations[i].size() - 1] - CityPopulations[i][CityPopulations[i].size() - 2]);
        }
        else
        {
            DeltaCityPopulations[i].push_back(0);
        }
    }
    Populations.push_back(Persons.size());
}

int World::MovePersons()
{
    int MovedPersons = 0;
    for (int i = 0; i < Persons.size(); i++)
    {
        if (Persons[i]->Age < 20) continue;
        if (RandomInt(int(1 / MoveChance)) == 0)
        {
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
            if (MoveOptions.size() != 0)
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
        if (RandomInt(1 / ReproduceChance) == 0)
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
            if (Partners[p][0] == PotentialPartners[i][0] || 
                Partners[p][0] == PotentialPartners[i][1] ||
                Partners[p][1] == PotentialPartners[i][0] ||
                Partners[p][1] == PotentialPartners[i][1])
            {
                Eligible = 0;
                break;
            }
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

int World::AgePersons()
{
    int NewDead = 0;
    for (int i = 0; i < Persons.size(); i++)
    {
        bool AlreadyDead = 0;
        Persons[i]->Age += 1;
        if (Persons[i]->Dead) AlreadyDead = 1;
        //cout << RandomInt((int)pow(80.0f / Persons[i]->Age, 10)) << endl;
        if (RandomInt(1 / exp(20 * (Persons[i]->Age / 80 - 1))) == 0)
        {
            Persons[i]->Die();
            DeadPersons.push_back(Persons[i]);
            VectorRemove(Persons, i);
            if (!AlreadyDead) NewDead++;
        }
    }
    CheckPartnerDeaths();
    return NewDead;
}

void World::CheckPartnerDeaths()
{
    for (int i = 0; i < Partners.size(); i++)
    {
        if (Partners[i][0]->Dead || Partners[i][1]->Dead)
        {
            VectorRemove(Partners, i);
        }
    }
}

void World::PlotInformation()
{
    vector<int> Time = {};
    for (int i = 0; i < Frames; i++)
    {
        Time.push_back(i);
    }
    ImGui::Begin("My Window");
    if (ImPlot::BeginPlot("Population Graph", ImVec2(-1,0), ImPlotFlags_NoLegend))
    {
        ImPlot::SetupAxes("Iterations", "Population", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        for (int i = 0; i < CityPopulations.size(); i++)
        {
            vector<int>& Population = CityPopulations[i];
            string LineName = "City " + to_string(i);
            ImPlot::PlotLine(LineName.c_str(), Time.data(), Population.data(), Frames);
        }
        ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("Average Population Graph", ImVec2(-1, 0), ImPlotFlags_NoLegend))
    {
        ImPlot::SetupAxes("Iterations", "Average Population", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        string LineName = "Average City Population";
        ImPlot::PlotLine(LineName.c_str(), Time.data(), Populations.data(),Frames);
        ImPlot::EndPlot();
    }
    //if (ImPlot::BeginPlot("Delta Population Graph", ImVec2(-1, 0), ImPlotFlags_NoLegend))
    //{
    //    ImPlot::SetupAxes("Iterations", "Delta Population", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
    //    for (int i = 0; i < DeltaCityPopulations.size(); i++)
    //    {
    //        vector<int>& DeltaPopulation = DeltaCityPopulations[i];
    //        string LineName = "City " + to_string(i);
    //        ImPlot::PlotLine(LineName.c_str(), Time.data(), DeltaPopulation.data(), Frames);
    //    }
    //    ImPlot::EndPlot();
    //}
    vector<int> CityIndices = {};
    for (int i = 0; i < Cities.size(); i++)
    {
        CityIndices.push_back(i);
    }
    vector<float> CityAges = {};
    for (int i = 0; i < Cities.size(); i++)
    {
        CityAges.push_back(Cities[i]->Age);
    }
    if (ImPlot::BeginPlot("City Average Ages", ImVec2(-1, 0), ImPlotFlags_NoLegend))
    {
        ImPlot::SetupAxes("City Index", "Average Age", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        //ImPlot::PlotBars("e", CityIndices.data(), CityAges.data());
        ImPlot::PlotBars("Vertical", CityAges.data(), CityAges.size());
        ImPlot::EndPlot();
    }

    ImGui::End();
}

void World::RaylibDraw()
{
    BeginMode2D(WorldCamera);
    for (int i = 0; i < Cities.size(); i++)
    {
        DrawCircle(Cities[i]->Position.x, Cities[i]->Position.y, Cities[i]->Population / 1000, WHITE);
    }
    EndMode2D();
}
