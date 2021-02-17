//
// Created by Bob
//

#ifdef WWISE_D
#ifndef GAMEAUDIOENGINE_AK_AUDIOMANAGERV1_H
#define GAMEAUDIOENGINE_AK_AUDIOMANAGERV1_H

#define GLOBAL_SCOPE "GlobalScope"

#include <cassert>
#include <iostream>
#include <map>
#include <vector>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>      // 1) Memory Manager interface
#include <AK/SoundEngine/Common/AkModule.h>         // 1) Default Memory Manager
#include <AK/SoundEngine/Common/IAkStreamMgr.h>     // 2) Streaming Manager
#include <AK/Tools/Common/AkPlatformFuncs.h>        // 2) Thread defines
#include <AkFilePackageLowLevelIOBlocking.h>        // 2) Simple default low-level I/O implementation
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // 3) Sound Engine
#include <AK/MusicEngine/Common/AkMusicEngine.h>    // 4) Music Engine // optional
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>  // 5) Spatial Audio // optional
#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>                // 6) Communications // Authoring profiler
#endif // AK_OPTIMIZED
#include "Wwise_IDs.h"


struct Vector3{
    explicit Vector3(float x=0, float y=0, float z=0) : x(x), y(y), z(z) {}
    float x;
    float y;
    float z;
};

// Interface code //

class AK_AudioManagerV1 {
public:
    static void Init();
    static void Update();
    static void Shutdown();

    void LoadBank(const std::string& strBankName);
    void UnloadBank(const std::string& strBankName);

    AkGameObjectID RegisterGameObject(const std::string& strAkGameObjectName);
    void UnRegisterGameObject(const std::string& strAkGameObjectName);

    // Playback
    AkGameObjectID PostEvent(const std::string& strEventName, const std::string& strAkGameObjectName, bool unRegAkObj = false);
    void StopEvent(const std::string& strEventName, bool immediate = false);
    bool IsEventPlaying(const std::string& strEventName) const;

    // Parameters
    void Set3dListenerAndOrientation(const Vector3& posision, const Vector3& look, const Vector3& up);
    void SetAkObject3dPosition(const std::string& strAkGameObjectName, const Vector3 &position);
    void SetObjectDryVolume(const std::string& strAkGameObjectName, float volumedB=0.f);
    void SetMasterVolume(float volumedB=0.f);

    void SetRTPC(const std::string& strRTPCName, float fValue, const std::string& strAkGameObjectName=GLOBAL_SCOPE);
    void SetSwitch(const std::string& strSwitchGroup, const std::string& strSwitchState, const std::string& strAkGameObjectName);
    void SetState(const std::string& strStateGroup, const std::string& strState);

    // Misc
    static AkVector VectorToAK(const Vector3& position){
        AkVector akVec{};
        akVec.X = position.x;
        akVec.Y = position.y;
        akVec.Z = position.z;
        return akVec;
    }
    static float dBToVolume(float dB){
        return powf(10.f, 0.05f *dB);
    }
    static float VolumeTodB (float volume){
        return 20.f * log10f(volume);
    }

    static void ErrorCheck(AKRESULT result);

    void Check(const std::string& strEventName);
};

#endif //GAMEAUDIOENGINE_AK_AUDIOMANAGERV1_H
#endif //WWISE_D
