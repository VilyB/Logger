#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

#include <SDL2/SDL.h>

int main(int, char **)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("ImGui + SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       800, 600, SDL_WINDOW_RESIZABLE);
    if (!win)
    {
        SDL_Log("CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren)
    {
        SDL_Log("CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(win, ren);
    ImGui_ImplSDLRenderer2_Init(ren);

    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                running = false;
            ImGui_ImplSDL2_ProcessEvent(&e);
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hello");
        ImGui::Text("Hello, SDL2 + ImGui!");
        ImGui::End();

        ImGui::Render();
        SDL_SetRenderDrawColor(ren, 30, 30, 30, 255);
        SDL_RenderClear(ren);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), ren);
        SDL_RenderPresent(ren);
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
