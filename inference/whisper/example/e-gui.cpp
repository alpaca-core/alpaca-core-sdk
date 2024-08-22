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
    ImGuiIO* m_io;
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
    state.m_io = &ImGui::GetIO();
    state.m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

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

std::string_view get_filename(std::string_view path) {
    return path.substr(path.find_last_of('/') + 1);
}

// void my_audio_callback(void *userdata, Uint8 *stream, int len) {
//     if (audio_len ==0)
//         return;

//     len = ( len > audio_len ? audio_len : len );
//     //SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
//     SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

//     audio_pos += len;
//     audio_len -= len;
// }

class UAudio {
public:
    UAudio(std::string path)
        : m_path(std::move(path))
        , m_name(get_filename(m_path))
    {}

    bool load() {
        if (isLoaded()) {
            return true;
        }
        return ac::audio::readWav(m_path, m_pcmf32, m_pcmf32s, false);
    }

    void unload() {
        m_pcmf32.clear();
        m_pcmf32s.clear();
    }

    bool isLoaded() const {
        return m_pcmf32.size();
    }

    void play() {
        // set the callback function
        // wav_spec.callback = my_audio_callback;
        // wav_spec.userdata = NULL;
        // // set our global static variables
        // audio_pos = wav_buffer; // copy sound buffer
        // audio_len = wav_length; // copy file length
        // SDL_OpenAudio()
        // SDL_MixAudio()
        // SDL_Pause
    }

    const std::vector<float>& pcmf32() { return m_pcmf32; }

    const std::vector<std::vector<float>>& pcmf32s() { return m_pcmf32s; }

    std::string_view name() const { return m_name; }

private:
    std::string m_path;
    std::string m_name;
    std::vector<float> m_pcmf32;               // mono-channel F32 PCM
    std::vector<std::vector<float>> m_pcmf32s; // stereo-channel F32 PCM
};

// unloadable model
class UModel {
public:
    UModel(std::string binPath) // intentionally implicit
        : m_binPath(std::move(binPath))
        , m_name(get_filename(m_binPath))
    {}

    const std::string& name() const { return m_name; }

    class State {
    public:
        State(const std::string& binPath, const ac::whisper::Model::Params& modelParams)
            : m_model(binPath.c_str(), modelParams)
        {}

        class Instance {
        public:
            Instance(std::string name, ac::whisper::Model& model, const ac::whisper::Instance::InitParams& params)
                : m_name(std::move(name))
                , m_instance(model, params)
            {}

            const std::string& name() const { return m_name; }

            std::string transcribe(UAudio* audio) {
                assert(audio->isLoaded());
                std::string res;
                m_instance.runOp("transcribe", audio->pcmf32(), audio->pcmf32s(), [&res](std::string result){
                    res = result;
                });

                return res;
            }

        private:
            std::string m_name;
            ac::whisper::Instance m_instance;
        };

        Instance* createInstance(const ac::whisper::Instance::InitParams& params) {
            auto name = std::to_string(m_nextInstanceId++);
            m_instance.reset(new Instance(name, m_model, params));
            return m_instance.get();
        }

        void dropInstance() {
            m_instance.reset();
        }

        Instance* instance() const { return m_instance.get(); }
    private:
        ac::whisper::Model m_model;

        int m_nextInstanceId = 0;
        std::unique_ptr<Instance> m_instance;
    };

    State* state() { return m_state.get(); }

    void unload() {
        m_state->dropInstance();
        m_state.reset();
        JALOG(Info, "unloaded ", m_name);
    }
    void load() {
        ac::whisper::Model::Params modelParams;
        m_state.reset(new State(m_binPath, modelParams));
        m_state->createInstance({});
        JALOG(Info, "loaded ", m_name);
    }
private:
    std::string m_binPath;
    std::string m_name;

    std::unique_ptr<State> m_state;
};

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
        UModel(AC_TEST_DATA_WHISPER_DIR "/whisper-base.en-f16.bin"),
        UModel(AC_TEST_DATA_WHISPER_DIR "/whisper-tiny.en-f16.bin"),
        UModel(AC_TEST_DATA_WHISPER_DIR "/whisper-base-q5_1.bin")
    });
    UModel* selectedModel = models.data();

    auto audioSamples = std::to_array({
        UAudio(AC_TEST_DATA_WHISPER_DIR "/as-she-sat.wav"),
        UAudio(AC_TEST_DATA_WHISPER_DIR "/prentice-hall.wav"),
        UAudio(AC_TEST_DATA_WHISPER_DIR "/yes.wav")
    });
    auto selectedWav = audioSamples.data();
    std::string lastOutput;

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
        }

        // prepare frame
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

            ImGui::Text("FPS: %.2f (%.2gms)", wState.m_io->Framerate, wState.m_io->Framerate ? 1000.0f / wState.m_io->Framerate : 0.0f);
            ImGui::Separator();

            ImGui::BeginTable("##main", 2, ImGuiTableFlags_Resizable);
            ImGui::TableNextColumn();

            ImGui::Text("Models");
            ImGui::BeginListBox("##models", {-1, 0});
            for (auto& m : models) {
                ImGui::PushID(&m);

                std::string name = m.name();
                name += m.state() ? " (loaded)" : " (unloaded)";

                if (ImGui::Selectable(name.c_str(), selectedModel == &m)) {
                    selectedModel = &m;
                }
                ImGui::PopID();
            }
            ImGui::EndListBox();

            UModel::State* modelState = nullptr;

            if (selectedModel) {
                if (selectedModel->state()) {
                    if (ImGui::Button("Unload")) {
                        selectedModel->unload();
                    }
                }
                else {
                    if (ImGui::Button("Load")) {
                        selectedModel->load();
                    }
                }

                modelState = selectedModel->state();
            }

            if (modelState) {
                ImGui::Separator();
                ImGui::Text("Audio samples");
                ImGui::BeginListBox("##samples", { -1, 0 });

                for (auto& s : audioSamples) {
                    std::string name(s.name());
                    name += s.isLoaded() ? " (loaded)" : " (unloaded)";
                    ImGui::PushID(&s);

                    if (ImGui::Selectable(name.c_str(), selectedWav == &s)) {
                        selectedWav = &s;
                    }
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }

            if (modelState) {
                auto instance = modelState->instance();
                if (selectedWav && instance) {
                    if (ImGui::Button("Transcribe")) {
                        if (!selectedWav->isLoaded()) {
                            selectedWav->load();
                        }
                        lastOutput = std::string(selectedWav->name()) + " transcription:\n\n";
                        lastOutput += instance->transcribe(selectedWav);
                    }
                }
            }

            ImGui::Separator();
            ImGui::TextWrapped("%s", lastOutput.c_str());
            ImGui::Separator();

            ImGui::EndTable();
            ImGui::End();
        }

        // render frame
        ImGui::Render();
        SDL_RenderSetScale(wState.m_renderer, wState.m_io->DisplayFramebufferScale.x, wState.m_io->DisplayFramebufferScale.y);
        SDL_RenderClear(wState.m_renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), wState.m_renderer);
        SDL_RenderPresent(wState.m_renderer);
    }

    deinitImGui();
    deinitSDL(wState);
    return 0;
}
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
