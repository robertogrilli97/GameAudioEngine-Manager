//
// Created by Bob on 25/01/2021.
//
#ifdef FMOD_D

#include "FMOD_AudioManagerV1.h"

// ************* // pImpl implementation Code// ************************************************************************* //

struct Implementation {
    Implementation();
    ~Implementation();

    void Update();

    FMOD::Studio::System* mpStudioSystem;
    FMOD::System* mpSystem;

    int mNextChannelId {};

    typedef std::map<std::string, FMOD::Sound*> SoundMap;
    typedef std::map<int, FMOD::Channel*> ChannelMap;
    typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;
    typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;

    SoundMap mSounds;
    ChannelMap mChannels;
    BankMap mBanks;
    EventMap mEvents;
};

Implementation::Implementation()
{
    // Create instance of the sound engine
    mpStudioSystem = nullptr;
    FMOD_AudioManagerV1::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
    FMOD_AudioManagerV1::ErrorCheck(
            mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, nullptr));

    mpSystem = nullptr;
    FMOD_AudioManagerV1::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
}

Implementation::~Implementation()
{
    // Release assets
    FMOD_AudioManagerV1::ErrorCheck(mpStudioSystem->unloadAll());
    FMOD_AudioManagerV1::ErrorCheck(mpStudioSystem->release());
}

void Implementation::Update()
{
//    for (auto &channel : mChannels) {
//        bool isPlaying = false;
//        channel.second->isPlaying(&isPlaying);
//        if (!isPlaying)
//            mChannels.erase(channel.first);
//    }

    // iterate through channels
    std::vector<ChannelMap::iterator> pStoppedChannels;
    for (auto it = mChannels.begin(); it != mChannels.end(); ++it)
    {
        bool isPlaying = false;
        it->second->isPlaying(&isPlaying); // If we call ErrorCheck here it will return an error
        if (!isPlaying) // "take note" if a channel is stopped
        {
            pStoppedChannels.push_back(it);
        }
    }
    for (auto& it : pStoppedChannels)
    {
        mChannels.erase(it); // erase the stopped channel
    }
    FMOD_AudioManagerV1::ErrorCheck(mpSystem->update());
}

// ************* // Static/Global Initialisation of the Audio engine // ********************************************* //
    // static methods because we want init one instance only (as a singleton)
    // we don't provide to this class an instance of the impl or a pointer to it
    // neither we override the copy constructor and the assignment operator (to communicate with the impl class)

Implementation* sgpImplementation = nullptr;

void FMOD_AudioManagerV1::Init() {
    sgpImplementation = new Implementation;
}

void FMOD_AudioManagerV1::Update() {
    sgpImplementation->Update();
}

void FMOD_AudioManagerV1::Shutdown() {
    delete sgpImplementation;
}

// ************* // Interface Code, Functions definitions // ******************************************************** //

void FMOD_AudioManagerV1::ErrorCheck(FMOD_RESULT result)
{
    if (result != FMOD_OK) {
//        assert("!Something") ???
        std::cout << "Error: "<< result << "; "<< FMOD_ErrorString(result) <<  std::endl; exit(-1);
    }
}

void FMOD_AudioManagerV1::Set3dListenerPositionAndOrientation(const Vector3 &position, const Vector3 &look, const Vector3 &up)
{
    FMOD_VECTOR vPosition = VectorToFmod(position);
    FMOD_VECTOR vLook = VectorToFmod(look);
    FMOD_VECTOR vUp = VectorToFmod(up);
    FMOD_AudioManagerV1::ErrorCheck(sgpImplementation->mpSystem->set3DListenerAttributes(0, &vPosition, nullptr, &vLook, &vUp));
}


void FMOD_AudioManagerV1::LoadBank(const std::string &strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
    auto foundIt = sgpImplementation->mBanks.find(strBankName);
    if (foundIt != sgpImplementation->mBanks.end())
        return;
    FMOD::Studio::Bank* pBank;
    FMOD_AudioManagerV1::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
    if (pBank)
        sgpImplementation->mBanks[strBankName] = pBank; // assign bank to bankMap
}

void FMOD_AudioManagerV1::LoadEvent(const std::string &strEventName)
{
    auto foundIt = sgpImplementation->mEvents.find(strEventName);
    if (foundIt != sgpImplementation->mEvents.end())
        return;
    FMOD::Studio::EventDescription* pEventDescription = nullptr;
    FMOD_AudioManagerV1::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
    if (pEventDescription)
    {
        FMOD::Studio::EventInstance* pEventInstance = nullptr;
        FMOD_AudioManagerV1::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
        if (pEventInstance)
            sgpImplementation->mEvents[strEventName] = pEventInstance;
    }
}

void FMOD_AudioManagerV1::LoadSound(const std::string &strSoundName, bool is3d, bool isLooping, bool isStreaming)
{
    // check cache, if we already have loaded the sound previously
    auto foundIt = sgpImplementation->mSounds.find(strSoundName);
    if (foundIt != sgpImplementation->mSounds.end())
        return;
    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= is3d ? FMOD_3D : FMOD_2D;
    eMode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= isStreaming ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD::Sound* pSound = nullptr;
    FMOD_AudioManagerV1::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
    if (pSound) // add sound to the cache
        sgpImplementation->mSounds[strSoundName] = pSound;
}

void FMOD_AudioManagerV1::UnloadSound(const std::string &strSoundName)
{
    // check cache for sound
    auto foundIt = sgpImplementation->mSounds.find(strSoundName);
    if (foundIt == sgpImplementation->mSounds.end())
        return;
    FMOD_AudioManagerV1::ErrorCheck(foundIt->second->release()); // unload sound
    sgpImplementation->mSounds.erase(foundIt); // erase from the cache
}

void FMOD_AudioManagerV1::UnloadEvent(const std::string &strEventName)
{
    // check cache for sound
    auto foundIt = sgpImplementation->mEvents.find(strEventName);
    if (foundIt == sgpImplementation->mEvents.end())
        return;
    FMOD_AudioManagerV1::ErrorCheck(foundIt->second->release()); // unload event
    sgpImplementation->mEvents.erase(foundIt); // erase from the cache
}

void FMOD_AudioManagerV1::UnloadBank(const std::string &strBankName)
{
    // check cache for sound
    auto foundIt = sgpImplementation->mBanks.find(strBankName);
    if (foundIt == sgpImplementation->mBanks.end())
        return;
    FMOD_AudioManagerV1::ErrorCheck(foundIt->second->unload()); // unload bank
    sgpImplementation->mBanks.erase(foundIt); // erase from the cache
}

// Playback

int FMOD_AudioManagerV1::PlaySound(const std::string &strSoundName, const Vector3 &pos, float volumedB)
{
    int channelId = sgpImplementation->mNextChannelId++; // report next unused/free channelID,
                                                            // assign it to current channel and increase the counter

    auto foundIt = sgpImplementation->mSounds.find(strSoundName);
    if (foundIt == sgpImplementation->mSounds.end())
    {
        LoadSound(strSoundName);
        foundIt = sgpImplementation->mSounds.find(strSoundName);
        if (foundIt == sgpImplementation->mSounds.end())
        {
            std::cout << "Could not find sound: " << strSoundName << std::endl;
            return channelId;
        }
    }

    FMOD::Channel* pChannel = nullptr;
    FMOD_AudioManagerV1::ErrorCheck(sgpImplementation->mpSystem->playSound(foundIt->second, nullptr, true, &pChannel));
    if (pChannel)
    {
        FMOD_MODE currMode;
        FMOD_AudioManagerV1::ErrorCheck(foundIt->second->getMode(&currMode));
        if (currMode & FMOD_3D)
        {
            FMOD_VECTOR position = VectorToFmod(pos);
            FMOD_AudioManagerV1::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
        }
        FMOD_AudioManagerV1::ErrorCheck(pChannel->setVolume(dBToVolume(volumedB)));
        FMOD_AudioManagerV1::ErrorCheck(pChannel->setPaused(false));
        sgpImplementation->mChannels[channelId] = pChannel;
    }
    return channelId;
}

void FMOD_AudioManagerV1::PlayEvent(const std::string &strEventName)
{
    auto foundIt = sgpImplementation->mEvents.find(strEventName);
    if (foundIt == sgpImplementation->mEvents.end())
    {
        LoadEvent(strEventName);
        auto foundIt = sgpImplementation->mEvents.find(strEventName);
        if (foundIt == sgpImplementation->mEvents.end())
            return;
    }

    FMOD_AudioManagerV1::ErrorCheck(foundIt->second->start());
}

void FMOD_AudioManagerV1::StopChannel(int channelId)
{
    auto foundIt = sgpImplementation->mChannels.find(channelId);
    if (foundIt == sgpImplementation->mChannels.end())
        return;
    FMOD_AudioManagerV1::ErrorCheck(foundIt->second->stop());
}

void FMOD_AudioManagerV1::StopEvent(const std::string &strEventName, bool immediate)
{
    auto foundIt = sgpImplementation->mEvents.find(strEventName);
    if (foundIt == sgpImplementation->mEvents.end())
        return;

    FMOD_STUDIO_STOP_MODE eMode;
    eMode = immediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    FMOD_AudioManagerV1::ErrorCheck(foundIt->second->stop(eMode));
}

void FMOD_AudioManagerV1::StopAllChannels()
{
    for (auto [name, channel] : sgpImplementation->mChannels)
        FMOD_AudioManagerV1::ErrorCheck(channel->stop());
}

bool FMOD_AudioManagerV1::IsPlaying(int channelId) const
{
    auto foundIt = sgpImplementation->mChannels.find(channelId);
    if (foundIt == sgpImplementation->mChannels.end())
    {
        std::cout << "Channel ID: " << channelId << ", not found.\n";
        return false;
    }

    bool isPlaying = false;
    FMOD_AudioManagerV1::ErrorCheck(foundIt->second->isPlaying(&isPlaying));
    if (isPlaying) // it should always be playing if found
    {
        std::cout << "Channel ID: " << channelId << ", playing.\n";
        return isPlaying;
    }
    return isPlaying;
}

bool FMOD_AudioManagerV1::IsEventPlaying(const std::string &strEventName) const {
    auto foundIt = sgpImplementation->mEvents.find(strEventName);
    if (foundIt == sgpImplementation->mEvents.end())
        return false;

    FMOD_STUDIO_PLAYBACK_STATE state;
    FMOD_AudioManagerV1::ErrorCheck(foundIt->second->getPlaybackState(&state));
    if (state == FMOD_STUDIO_PLAYBACK_PLAYING)
        return true;

    return false;
}

// Parameters

void FMOD_AudioManagerV1::SetChannelVolume(int channelId, float volumedB)
{
    auto foundIt = sgpImplementation->mChannels.find(channelId);
    if(foundIt == sgpImplementation->mChannels.end())
        return;

    FMOD_AudioManagerV1::ErrorCheck(foundIt->second->setVolume(dBToVolume(volumedB)));
}

void FMOD_AudioManagerV1::SetChannel3dPosition(int channelId, const Vector3 &position)
{
    auto foundIt = sgpImplementation->mChannels.find(channelId);
    if (foundIt == sgpImplementation->mChannels.end())
        return;

    FMOD_VECTOR vPosition = VectorToFmod(position);
    FMOD_AudioManagerV1::ErrorCheck(foundIt->second->set3DAttributes(&vPosition, nullptr));
}

void FMOD_AudioManagerV1::GetEventParameter(const std::string &strEventName, const std::string &strParameterName, float *outParValue)
{
    auto foundIt = sgpImplementation->mEvents.find(strEventName);
    if (foundIt == sgpImplementation->mEvents.end())
        return;

    /*
    // get global parameter complete info, could also be retrieved from EventDescription
    FMOD_STUDIO_PARAMETER_DESCRIPTION *parameter = nullptr;
    sgpImplementation->mpStudioSystem->getParameterDescriptionByName(strParameterName.c_str(), parameter);
    // get global parameter value
    FMOD_AudioManagerV1::ErrorCheck(sgpImplementation->mpStudioSystem->getParameterByName(strParameterName.c_str(), outParValue));
    */

    // get local event parameter value
    FMOD_AudioManagerV1::ErrorCheck(foundIt->second->getParameterByName(strParameterName.c_str(), outParValue));
}

void FMOD_AudioManagerV1::SetEventParameter(const std::string &strEventName, const std::string &strParameterName, float value)
{
    auto foundIt = sgpImplementation->mEvents.find(strParameterName);
    if (foundIt == sgpImplementation->mEvents.end())
        return;

//    // set local event parameter value
//    FMOD_AudioManagerV1::ErrorCheck(sgpImplementation->mpStudioSystem->setParameterByName(strParameterName.c_str(), value));

    // set local event parameter value
    FMOD_AudioManagerV1::ErrorCheck(foundIt->second->setParameterByName(strParameterName.c_str(), value));
}

#endif //FMOD_D
