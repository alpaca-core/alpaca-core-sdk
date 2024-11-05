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
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>

// audio decoding helper
#include <ac-audio.hpp>

// logging
#include <ac/jalog/Instance.hpp>
#include <ac/jalog/Log.hpp>
#include <ac/jalog/sinks/ColorSink.hpp>

// model source directory
#include "ac-test-data-whisper-dir.h"

#include <iostream>
#include <string>
#include <vector>
#include <array>

struct WindowState {
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    ImGuiIO* m_io;

};

constexpr uint16_t MAX_RECORDING_BUFFER_SECONDS = 30;
constexpr uint32_t RECORDING_BUFFER_TYPE_SIZE = sizeof(float);

struct AudioState {
    SDL_AudioDeviceID m_recordingDeviceId = 0;
    SDL_AudioDeviceID m_playbackDeviceId = 0;
    SDL_AudioSpec m_defaultSpec;
    SDL_AudioSpec m_returnedRecordingSpec;
    SDL_AudioSpec m_returnedPlaySpec;
    uint32_t m_bytesPerSecond;

    // Recording data buffer
    std::vector<float>* m_currentRecordingBuffer;

    // Size of data buffer
    uint32_t m_maxBufferByteSize = 0;

    // Position in data buffer
    uint32_t m_bufferBytePosition = 0;

    // Last recorded position in data buffer
    uint32_t m_actualBufferByteSize = 0;
};

int sdlError(const char* msg) {
    std::cerr << msg << ": " << SDL_GetError() << "\n";
    return -1;
}

void audioRecordingCallback(void* userData, Uint8* stream, int len) {
    AudioState* aState = static_cast<AudioState*>(userData);
    if (aState->m_bufferBytePosition > aState->m_maxBufferByteSize) {
        return;
    }
    //Copy audio from stream
    auto bufferPos = aState->m_currentRecordingBuffer->data() + (aState->m_bufferBytePosition / RECORDING_BUFFER_TYPE_SIZE);
    std::memcpy(bufferPos, stream, len);

    //Move along buffer
    aState->m_bufferBytePosition += len;
}

void audioPlaybackCallback(void* userData, Uint8* stream, int len) {
    AudioState* aState = static_cast<AudioState*>(userData);
    if (aState->m_bufferBytePosition > aState->m_actualBufferByteSize) {
        return;
    }

    //Copy audio to stream
    auto bufferPos = aState->m_currentRecordingBuffer->data() + (aState->m_bufferBytePosition / RECORDING_BUFFER_TYPE_SIZE);
    std::memcpy(stream, bufferPos, len);

    //Move along buffer
    aState->m_bufferBytePosition += len;
}

int initSDL(WindowState& wState, AudioState& aState) {
    auto sdlError = [](const char* msg){
        std::cerr << msg << ": " << SDL_GetError() << "\n";
        return -1;
    };

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0) {
        sdlError("Error: SDL_Init");
    }

    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    wState.m_window = SDL_CreateWindow(
        "Alpaca Core whisper.cpp example",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!wState.m_window) {
        return sdlError("Error: SDL_CreateWindow");
    }
    wState.m_renderer = SDL_CreateRenderer(wState.m_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (wState.m_renderer == nullptr) {
        return sdlError("Error: SDL_CreateRenderer");
    }

    SDL_RendererInfo info;
    SDL_GetRendererInfo(wState.m_renderer, &info);
    AC_JALOG(Info, "SDL_Renderer: ", info.name);

    char* buff;
    SDL_GetDefaultAudioInfo(&buff, &aState.m_defaultSpec, SDL_TRUE);
    std::string defaultDeviceName(buff);
    SDL_free(buff);

    SDL_zero(aState.m_defaultSpec);
    aState.m_defaultSpec.freq = 16000;
    aState.m_defaultSpec.format = AUDIO_F32;
    aState.m_defaultSpec.channels = 1;
    aState.m_defaultSpec.samples = 16;
    aState.m_defaultSpec.userdata = &aState;

    SDL_AudioSpec desiredRecordingSpec;
    desiredRecordingSpec = aState.m_defaultSpec;
    desiredRecordingSpec.callback = audioRecordingCallback;

#if _DEBUG
    int recordingDeviceCount = SDL_GetNumAudioDevices(SDL_TRUE);
    for(int i = 0; i < recordingDeviceCount; ++i)
    {
        //Get capture device name
        std::string deviceName(SDL_GetAudioDeviceName(i, SDL_TRUE));
        if (deviceName == defaultDeviceName) {
            AC_JALOG(Info, "[Default]: %d - %s\n", i, deviceName);
        } else {
            AC_JALOG(Info, "%d - %s\n", i, deviceName);
        }
    }
#endif

    //Open recording device
    aState.m_recordingDeviceId = SDL_OpenAudioDevice(defaultDeviceName.c_str(),
        SDL_TRUE,
        &desiredRecordingSpec,
        &aState.m_returnedRecordingSpec,
        SDL_AUDIO_ALLOW_FORMAT_CHANGE);

    // Device failed to open
    if(aState.m_recordingDeviceId == 0)
    {
        //Report error
        AC_JALOG(Error, "Failed to open recording device! SDL Error: %s", SDL_GetError());
        return 1;
    }

    //Default audio spec
    SDL_AudioSpec desiredPlaybackSpec;
    desiredPlaybackSpec = aState.m_defaultSpec;
    desiredPlaybackSpec.callback = audioPlaybackCallback;

    //Open playback device
    aState.m_playbackDeviceId = SDL_OpenAudioDevice(NULL,
        SDL_FALSE,
        &desiredPlaybackSpec,
        &aState.m_returnedPlaySpec,
        SDL_AUDIO_ALLOW_FORMAT_CHANGE );

    //Device failed to open
    if(aState.m_playbackDeviceId == 0)
    {
        //Report error
        AC_JALOG(Error, "Failed to open playback device! SDL Error: %s", SDL_GetError());
        return 1;
    }

    //Calculate per sample bytes
    int bytesPerSample = aState.m_returnedRecordingSpec.channels * (SDL_AUDIO_BITSIZE(aState.m_returnedRecordingSpec.format) / 8);

    //Calculate bytes per second
    aState.m_bytesPerSecond = aState.m_returnedRecordingSpec.freq * bytesPerSample;

    //Calculate buffer size
    aState.m_maxBufferByteSize = MAX_RECORDING_BUFFER_SECONDS * aState.m_bytesPerSecond;

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

class UAudio {
public:
    UAudio(std::string path, bool recorded = false)
        : m_path(std::move(path))
        , m_name(get_filename(m_path))
        , m_isSaved(!recorded)
    {}

    void load() {
        if (isLoaded()) {
            return;
        }

        m_isSaved = true;
        m_pcmf32 = ac::audio::loadWavF32Mono(m_path);
    }

    void unload() {
        m_pcmf32.clear();
    }

    bool isLoaded() const {
        return m_pcmf32.size();
    }

    bool isSaved() {
        return m_isSaved;
    }

    bool save(const AudioState& aState) {
        if (m_isSaved) {
            return true;
        }

        auto fileName = std::string(AC_TEST_DATA_WHISPER_DIR) + "/" + m_name + ".wav";
        ac::audio::WavWriter writer;
         // /2 is because when it was saved, it sounded high pitched without it
        auto bitPerSample = SDL_AUDIO_BITSIZE(aState.m_returnedRecordingSpec.format) * aState.m_returnedRecordingSpec.channels / 2;
        writer.open(fileName,
            aState.m_returnedRecordingSpec.freq,
            uint16_t(bitPerSample),
            aState.m_returnedRecordingSpec.channels);
        writer.write(m_pcmf32.data(), m_pcmf32.size());
        writer.close();
        m_isSaved = true;
        return m_isSaved;
    }

    std::vector<float>& pcmf32() { return m_pcmf32; }

    std::string_view name() const { return m_name; }

private:
    std::string m_path;
    std::string m_name;
    bool m_isSaved;
    std::vector<float> m_pcmf32;               // mono-channel F32 PCM
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

                return m_instance.transcribe(audio->pcmf32());
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
        AC_JALOG(Info, "unloaded ", m_name);
    }
    void load() {
        ac::whisper::Model::Params modelParams;
        m_state.reset(new State(m_binPath, modelParams));
        m_state->createInstance({});
        AC_JALOG(Info, "loaded ", m_name);
    }
private:
    std::string m_binPath;
    std::string m_name;

    std::unique_ptr<State> m_state;
};

int main(int, char**) {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::ColorSink>();

    WindowState wState;
    AudioState aState;
    int res = initSDL(wState, aState);
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

    auto audioSamples = std::vector<UAudio>({
        UAudio(AC_TEST_DATA_WHISPER_DIR "/as-she-sat.wav"),
        UAudio(AC_TEST_DATA_WHISPER_DIR "/prentice-hall.wav"),
        UAudio(AC_TEST_DATA_WHISPER_DIR "/yes.wav")
    });
    auto selectedWav = audioSamples.data();
    std::string lastOutput;

    bool done = false;
    bool recording = false;
    bool playing = false;
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

        if (recording) {
            SDL_PauseAudioDevice(aState.m_recordingDeviceId, SDL_FALSE );

            // Lock callback
            SDL_LockAudioDevice(aState.m_recordingDeviceId);

            // Finished recording
            if(aState.m_bufferBytePosition > aState.m_maxBufferByteSize) {
                // Stop recording audio
                SDL_PauseAudioDevice(aState.m_recordingDeviceId, SDL_TRUE );
                recording = false;
            }

            // Unlock callback
            SDL_UnlockAudioDevice(aState.m_recordingDeviceId);
        }

        if (playing) {
            SDL_PauseAudioDevice(aState.m_playbackDeviceId, SDL_FALSE);

            // Lock callback
            SDL_LockAudioDevice(aState.m_playbackDeviceId);

            // Finished playback
            if(aState.m_bufferBytePosition > aState.m_actualBufferByteSize) {
                // Stop playing audio
                SDL_PauseAudioDevice(aState.m_playbackDeviceId, SDL_TRUE);
                playing = false;
            }

            // Unlock callback
            SDL_UnlockAudioDevice(aState.m_playbackDeviceId);
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
                    ImGui::PushID(&s);
                    std::string name(s.name());
                    name += s.isLoaded() ? " (loaded)" : " (unloaded)";
                    name += s.isSaved() ? "" : " (unsaved)";

                    if (ImGui::Selectable(name.c_str(), selectedWav == &s)) {
                        selectedWav = &s;
                    }
                    ImGui::PopID();
                }
                ImGui::EndListBox();
            }

            if (modelState) {
                auto instance = modelState->instance();
                if (selectedWav && instance && !recording) {
                    if (ImGui::Button("Transcribe")) {
                        if (!selectedWav->isLoaded()) {
                            selectedWav->load();
                        }
                        lastOutput = std::string(selectedWav->name()) + " transcription:\n\n";
                        lastOutput += instance->transcribe(selectedWav);
                    }
                }

                if (!recording && !playing && ImGui::Button("Record")) {
                    static int32_t cnt = 0;
                    audioSamples.push_back(UAudio("unnamed" + std::to_string(++cnt), true));

                    //Allocate and initialize byte buffer
                    audioSamples.back().pcmf32().resize(aState.m_maxBufferByteSize / RECORDING_BUFFER_TYPE_SIZE);
                    std::memset(audioSamples.back().pcmf32().data(), 0, aState.m_maxBufferByteSize);

                    aState.m_currentRecordingBuffer = &audioSamples.back().pcmf32();
                    aState.m_bufferBytePosition = 0;
                    recording = true;
                    selectedWav = &audioSamples.back();
                }

                if (recording && ImGui::Button("Stop recording")) {
                    SDL_PauseAudioDevice(aState.m_recordingDeviceId, SDL_TRUE );
                    aState.m_actualBufferByteSize = aState.m_bufferBytePosition;
                    aState.m_currentRecordingBuffer->resize(aState.m_bufferBytePosition / RECORDING_BUFFER_TYPE_SIZE);
                    recording = false;
                }

                if (!recording && !playing && ImGui::Button("Play Audio")) {
                    aState.m_bufferBytePosition = 0;
                    if (selectedWav) {
                        if (!selectedWav->isLoaded()) {
                            selectedWav->load();
                        }
                        aState.m_currentRecordingBuffer = &selectedWav->pcmf32();
                        aState.m_actualBufferByteSize = uint32_t(aState.m_currentRecordingBuffer->size()) * RECORDING_BUFFER_TYPE_SIZE;
                        playing = true;
                    }
                }

                if (playing && ImGui::Button("Stop Audio")) {
                    SDL_PauseAudioDevice(aState.m_playbackDeviceId, SDL_TRUE);
                    playing = false;
                }

                if (playing) {
                    const float max = aState.m_actualBufferByteSize / float(aState.m_bytesPerSecond);
                    float currPos = aState.m_bufferBytePosition / float(aState.m_bytesPerSecond);
                    const float beforeMut = currPos;
                    ImGui::SliderFloat("Audio: ", &currPos, 0, max);

                    if (beforeMut != currPos) {
                        aState.m_bufferBytePosition = int32_t(currPos) * aState.m_bytesPerSecond;
                    }
                }

                if (!recording && !selectedWav->isSaved() && ImGui::Button("Save Audio")) {
                    selectedWav->save(aState);
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
