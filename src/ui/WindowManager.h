#include <SDL2/SDL.h>
#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

#include "LoggerUI.h"

class WindowManager
{
private:
    SDL_Window *m_win = nullptr;
    SDL_Renderer *m_ren = nullptr;
    bool m_running = false;
    bool m_initialized = false;
    bool m_imgui_active = true;

public:
    WindowManager() = default;
    ~WindowManager() { shutdown(); }

    WindowManager(const WindowManager &) = delete;
    WindowManager &operator=(const WindowManager &) = delete;
    WindowManager(WindowManager &&) = delete;
    WindowManager &&operator=(WindowManager &&) = delete;

    bool initialize(const char *title, int w, int h, bool vsync)
    {
        if (m_initialized)
            return true; // idempotent

        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            SDL_Log("SDL_Init failed: %s", SDL_GetError());
            return false;
        }

        m_win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 w, h, SDL_WINDOW_RESIZABLE);
        if (!m_win)
        {
            SDL_Log("CreateWindow failed: %s", SDL_GetError());
            SDL_Quit();
            return false;
        }

        m_ren = SDL_CreateRenderer(m_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!m_ren)
        {
            SDL_Log("CreateRenderer failed: %s", SDL_GetError());
            SDL_DestroyWindow(m_win);
            SDL_Quit();
            return false;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForSDLRenderer(m_win, m_ren);
        ImGui_ImplSDLRenderer2_Init(m_ren);

        m_initialized = true;
        m_running = true;
        return true;
    }

    void loop(LoggerUI &ui)
    {
        if (!m_initialized)
            return;
        while (m_running)
        {
            SDL_Event e;
            while (SDL_PollEvent(&e))
            {
                ImGui_ImplSDL2_ProcessEvent(&e);
                switch (e.type)
                {
                case SDL_QUIT:
                    m_running = false;
                    break;
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_CLOSE &&
                        e.window.windowID == SDL_GetWindowID(m_win))
                    {
                        m_running = false;
                    }
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE)
                    {
                        m_running = false;
                    }
                    break;
                default:
                    break;
                }

                if (!m_running)
                    break;
            }

            if (!m_running)
                break;

            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            // --- UI ---
            ui.draw();
            // --- /UI ---

            ImGui::Render();

            SDL_SetRenderDrawColor(m_ren, 30, 30, 30, 255);
            SDL_RenderClear(m_ren);
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_ren);
            SDL_RenderPresent(m_ren);
        }
    }

    void shutdown()
    {
        if (m_imgui_active)
        {
            ImGui_ImplSDLRenderer2_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();
            m_imgui_active = false;
        }

        if (m_ren)
            SDL_DestroyRenderer(m_ren);
        if (m_win)
            SDL_DestroyWindow(m_win);

        if (m_initialized)
        {
            SDL_Quit();
            m_initialized = false;
        }
    }
};
