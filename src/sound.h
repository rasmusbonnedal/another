#pragma once

#include <SDL.h>

class Audio {
   public:
    Audio() {
        init();
    }
    ~Audio() {
        SDL_CloseAudioDevice(m_device);
    }

    void init() {
        SDL_Init(SDL_INIT_AUDIO);
        // opening an audio device:
        SDL_AudioSpec audio_spec;
        SDL_zero(audio_spec);
        audio_spec.freq = 11025;
        audio_spec.format = AUDIO_S16SYS;
        audio_spec.channels = 1;
        audio_spec.samples = 1024;
        audio_spec.callback = NULL;
        m_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
    }

    void queue(short* buf, size_t n) {
        SDL_QueueAudio(m_device, buf, (Uint32)n * sizeof(short));
    }

    size_t queue_size() const {
        return SDL_GetQueuedAudioSize(m_device);
    }

    void play() {
        SDL_PauseAudioDevice(m_device, 0);
    }

   private:
    SDL_AudioDeviceID m_device;
};

void playsound(Audio& audio, const char* buf, size_t len);
