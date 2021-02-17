//
// Created by Bob
//

#ifdef FMOD_D
#ifndef GAMEAUDIOENGINE_FMOD_AUDIOMANAGERV0_H
#define GAMEAUDIOENGINE_FMOD_AUDIOMANAGERV0_H

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <map>
#include "RandomNumberGen.h"

class FMOD_AudioManagerV0 {
public:
    // Static Instance of the Audio Manager
    static FMOD_AudioManagerV0& getInstance();
    FMOD_AudioManagerV0(const FMOD_AudioManagerV0&) = delete; // copy constructor (or private)
    const FMOD_AudioManagerV0& operator=(const FMOD_AudioManagerV0&) = delete; // assignment operator (or private)
    ~FMOD_AudioManagerV0();

    void Update(float elapsed);

    void LoadSFX(const std::string& path);
    void LoadSong(const std::string& path);

    // Playback
    void PlaySFX(const std::string& path,
                 float minVolume, float maxVolume,
                 float minPitch, float maxPitch);
    void PlaySong(const std::string& path);
    void StopSFXs();
    void StopSongs();

    // Parameters
    void SetMasterVolume(float volume) const;
    void SetSFXsVolume(float volume) const;
    void SetSongsVolume(float volume) const;

private:
    FMOD_AudioManagerV0();
    FMOD::System* mpSystem = nullptr; // instance of the system
    FMOD::Studio::System* mpStudioSystem = nullptr;

    static void CheckForErrors(FMOD_RESULT result);
    static float ChangeSemitone(float frequency, float variation);
    float RandomBetween(float min, float max); // migrate inside RandomGenerator Class

    typedef std::map<std::string, FMOD::Sound*> SoundMap;
    enum Category { CATEGORY_SFX, CATEGORY_SONG, CATEGORY_COUNT };
    void Load(Category type, const std::string& path);
    FMOD::ChannelGroup* mpMaster = nullptr;
    FMOD::ChannelGroup* mpGroups [CATEGORY_COUNT]{};
    SoundMap mSounds [CATEGORY_COUNT]{};
    FMOD_MODE mModes [CATEGORY_COUNT]{};

    // Songs Playback FadeIn/Out
    FMOD::Channel* mpCurrentSong = nullptr;
    std::string mCurrentSongPath;
    std::string mNextSongPath;

    enum FadeState { FADE_NONE, FADE_IN, FADE_OUT};
    FadeState mFade = FADE_NONE;
};

#endif // GAMEAUDIOENGINE_FMOD_AUDIOMANAGERV0_H
#endif // FMOD_D