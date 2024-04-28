// WorldSim.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Resources.hpp"
#include "World.hpp"
#include "City.hpp"
#include "Person.hpp"

int main(int argc, char** argv)
{
    //SDL_SetMainReady();

    SDL_Window* window = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);


    window = SDL_CreateWindow("WorldSim Graphs", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1400, 800, window_flags);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init();

    ImPlot::CreateContext();
    

    int CityCount = 100;
    int GodCount = 10;
    int PersonCount = 10000;

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
        NewPerson->Age = RandomInt(80);
        HomeWorld->AddPerson(NewPerson);
        NewPerson->Parents[0]->Children.push_back(NewPerson);
    }

    HomeWorld->SetupWorld();
    bool quit = false;
    SDL_Event e;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    jthread UpdateThread{ [HomeWorld]() {HomeWorld->Thread(); } };

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        //HomeWorld->Update();
        if (!HomeWorld->IsUpdating/* && HomeWorld->Frames < 100*/)
        {
            //stuff

            HomeWorld->IsUpdating = true;
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        //Start ImPlot
        HomeWorld->PlotInformation();
        //Stop ImPlot
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
        HomeWorld->RaylibDraw();
    }
    
    ImPlot::DestroyContext();

    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
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
