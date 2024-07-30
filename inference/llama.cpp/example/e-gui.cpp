// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>

#include <jalog/Instance.hpp>
#include <jalog/Log.hpp>
#include <jalog/sinks/ColorSink.hpp>

#include <iostream>

int sdlError(const char* msg) {
    std::cerr << msg << ": " << SDL_GetError() << "\n";
    return -1;
}

int main(int, char**) {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::ColorSink>();

    // setup sdl
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        sdlError("Error: SDL_Init");
    }

    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    SDL_Window* window = SDL_CreateWindow(
        "Alpaca Core llama.cpp example",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        return sdlError("Error: SDL_CreateWindow");
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        return sdlError("Error: SDL_CreateRenderer");
    }

    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);
    JALOG(Info, "SDL_Renderer: ", info.name);

    // setup imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // setup backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // app state

    // main loop
    bool done = false;
    while (!done)
    {
        // frame setup
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // app logic
        ImGui::ShowDemoWindow(nullptr);

        // frame render
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
