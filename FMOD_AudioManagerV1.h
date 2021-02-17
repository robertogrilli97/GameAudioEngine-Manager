//
// Created by Bob
//

#ifdef FMOD_D
#ifndef GAMEAUDIOENGINE_FMOD_AUDIOMANAGERV1_H
#define GAMEAUDIOENGINE_FMOD_AUDIOMANAGERV1_H

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <map>
#include "RandomNumberGen.h"

struct Vector3{
    explicit Vector3(float x=0, float y=0, float z=0) : x(x), y(y), z(z) {}
    float x;
    float y;
    float z;
};

    // Interface code //

class FMOD_AudioManagerV1 {
public:
    static void Init();
    static void Update();
    static void Shutdown();

    // Load both MasterBank and MasterBank.strings before any other
    void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags = FMOD_STUDIO_LOAD_BANK_NORMAL);
    void LoadEvent(const std::string& strEventName); // better to load with GUID instead
    void LoadSound(const std::string& strSoundName,
                   bool is3d = true,
                   bool isLooping = false,
                   bool isStreaming = false);
    void UnloadSound(const std::string &strSoundName);
    void UnloadEvent(const std::string &strEventName);
    void UnloadBank(const std::string &strBankName);

    // Playback
    int PlaySound(const std::string& strSoundName,
                  const Vector3& pos=Vector3(0,0,0),
                  float volumedB=0.f);
    void PlayEvent(const std::string& strEventName);
    void StopChannel(int channelId);
    void StopEvent(const std::string& strEventName, bool immediate = false);
    void StopAllChannels();
    bool IsPlaying(int channelId) const;
    bool IsEventPlaying(const std::string& strEventName) const;

    // Parameters
    void Set3dListenerPositionAndOrientation(const Vector3& posision, const Vector3& look, const Vector3& up);
    void SetChannelVolume(int channelId, float volumedB=0.f);
    void SetChannel3dPosition(int channelId, const Vector3& position);
    void GetEventParameter(const std::string& strEventName, const std::string& strParameterName, float* outParValue);
    void SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);

    // Misc
    static float dBToVolume(float dB){
        return powf(10.f, 0.05f *dB);
    }
    static float VolumeTodB (float volume){
        return 20.f * log10f(volume);
    }
    static FMOD_VECTOR VectorToFmod(const Vector3& position){
        FMOD_VECTOR fVec;
        fVec.x = position.x;
        fVec.y = position.y;
        fVec.z = position.z;
        return fVec;
    }

    static void ErrorCheck(FMOD_RESULT result);
};

#endif // GAMEAUDIOENGINE_FMOD_AUDIOMANAGERV1_H
#endif // FMOD_D