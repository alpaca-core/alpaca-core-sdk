// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// trivial example of using alpaca-core's whisper inference

// whisper
#include <ac/whisper/Init.hpp>
#include <ac/whisper/Model.hpp>
#include <ac/whisper/Instance.hpp>

// imgui & sdl
#include <imgui.h>
// #include <imgui_stdlib.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>

// audio decoding helper
#include <ac-audio.hpp>

// logging
#include <jalog/Instance.hpp>
#include <jalog/Log.hpp>
#include <jalog/sinks/ColorSink.hpp>

// model source directory
#include "ac-test-data-whisper-dir.h"

#include <iostream>
#include <string>
#include <vector>

struct WindowState {
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    ImGuiIO m_io;
};

int sdlError(const char* msg) {
    std::cerr << msg << ": " << SDL_GetError() << "\n";
    return -1;
}

int initSDL(WindowState& state) {
    auto sdlError = [](const char* msg){
        std::cerr << msg << ": " << SDL_GetError() << "\n";
        return -1;
    };

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        sdlError("Error: SDL_Init");
    }

    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    state.m_window = SDL_CreateWindow(
        "Alpaca Core whisper.cpp example",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!state.m_window) {
        return sdlError("Error: SDL_CreateWindow");
    }
    state.m_renderer = SDL_CreateRenderer(state.m_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (state.m_renderer == nullptr) {
        return sdlError("Error: SDL_CreateRenderer");
    }

    SDL_RendererInfo info;
    SDL_GetRendererInfo(state.m_renderer, &info);
    JALOG(Info, "SDL_Renderer: ", info.name);

    return 0;
}

void deinitSDL(WindowState& state) {
    SDL_DestroyRenderer(state.m_renderer);
    SDL_DestroyWindow(state.m_window);
    SDL_Quit();
}

void initImGui(WindowState& state) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    state.m_io = ImGui::GetIO();
    state.m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // setup backends
    ImGui_ImplSDL2_InitForSDLRenderer(state.m_window, state.m_renderer);
    ImGui_ImplSDLRenderer2_Init(state.m_renderer);
}

void deinitImGui() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

int main() try {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::ColorSink>();

    WindowState wState;
    int res = initSDL(wState);
    if (res != 0) {
        std::cerr << "Error during SDL initialization!\n";
        return res;
    }

    initImGui(wState);

    ac::whisper::initLibrary();

    auto models = std::to_array({
        AC_TEST_DATA_WHISPER_DIR "/whisper-base.en-f16.bin",
        AC_TEST_DATA_WHISPER_DIR "/whisper-tiny.en-f16.bin",
        AC_TEST_DATA_WHISPER_DIR "/whisper-base-q5_1.bin"
    });

        // main loop
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT) {
                done = true;
            }

            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(wState.m_window)) {
                done = true;
            }

            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            {
                // app logic
                auto* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->Pos);
                ImGui::SetNextWindowSize(viewport->Size);
                ImGui::Begin("#main", nullptr,
                    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize);

                ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
                ImGui::Separator();

                ImGui::BeginTable("##main", 2, ImGuiTableFlags_Resizable);
                ImGui::TableNextColumn();

                ImGui::Text("Models");
                ImGui::BeginListBox("##models", {-1, 0});
                for (auto& m : models) {
                    ImGui::PushID(&m);

                    std::string name = m.name();
                    if (m.state()) {
                        name += " (0 sessions)";
                    }
                    else {
                        name += " (unloaded)";
                    }

                    if (ImGui::Selectable(name.c_str(), selectedModel == &m)) {
                        selectedModel = &m;
                    }
                    ImGui::PopID();
                }
                ImGui::EndListBox();
                UModel::State* modelState = nullptr;
            }

            // render frame
            ImGui::Render();
            SDL_RenderSetScale(wState.m_renderer, wState.m_io.DisplayFramebufferScale.x, wState.m_io.DisplayFramebufferScale.y);
            SDL_RenderClear(wState.m_renderer);
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), wState.m_renderer);
            SDL_RenderPresent(wState.m_renderer);
        }
    }

    deinitImGui();
    deinitSDL(wState);
    return 0;
}
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
