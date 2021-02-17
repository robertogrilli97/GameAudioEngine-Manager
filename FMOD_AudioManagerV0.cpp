//
// Created by Bob on 04/02/2021.
//

#ifdef FMOD_D
#include "FMOD_AudioManagerV0.h"

FMOD_AudioManagerV0 &FMOD_AudioManagerV0::getInstance() {
    static FMOD_AudioManagerV0 audioManager;
    return audioManager;
}

FMOD_AudioManagerV0::FMOD_AudioManagerV0() {
    CheckForErrors(FMOD::Studio::System::create(&mpStudioSystem));
    CheckForErrors(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, nullptr));
    CheckForErrors(mpStudioSystem->getCoreSystem(&mpSystem));

    // Create channel groups
    CheckForErrors(mpSystem->getMasterChannelGroup(&mpMaster));
    for (auto& mpGroup : mpGroups){
        CheckForErrors(mpSystem->createChannelGroup(nullptr, &mpGroup));
        CheckForErrors(mpMaster->addGroup(mpGroup));
    }

    // Set up modes for each sound category
    mModes[CATEGORY_SFX] = FMOD_DEFAULT;
    mModes[CATEGORY_SONG] = FMOD_DEFAULT | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL;

}

FMOD_AudioManagerV0::~FMOD_AudioManagerV0() {
    // Release every sound object and clear the map
    for (auto& mSound : mSounds) {
        for (auto& [name, sound] : mSound)
            CheckForErrors(sound->release()); // release each FMOD channel in the soundMap
        mSound.clear(); // remove all elements in the soundMap
    }
//    for (int i = 0; i < CATEGORY_COUNT; ++i) {
//        for (auto iter = mSounds[i].begin(); iter != mSounds[i].end(); ++iter){
//            ErrorCheck(iter->second->release());
//        }
//        mSounds[i].clear();
//    }

    // Release the system object
    CheckForErrors(mpSystem->release());
    mpSystem = nullptr;
    mpStudioSystem = nullptr;
}

void FMOD_AudioManagerV0::Update(float elapsed) {
    const float fadeTime = 1.0f; // in seconds
    if(mpCurrentSong != nullptr && mFade == FADE_IN){
        float volume;
        mpCurrentSong->getVolume(&volume);
        float nextVolume = volume + elapsed / fadeTime;
        std::cout << "volume: " << volume << " + elapsed: " << elapsed << "/ 1 = nextVolume " << elapsed << std::endl;
        if (nextVolume >= 1.0f){
            mpCurrentSong->setVolume(1.0f);
            mFade = FADE_NONE;
        } else {
            mpCurrentSong->setVolume(nextVolume);
        }
    } else if (mpCurrentSong != nullptr && mFade == FADE_OUT){
        float volume;
        mpCurrentSong->getVolume(&volume);
        float nextVolume = volume - elapsed / fadeTime;
        if (nextVolume <= 0.0f){
            mpCurrentSong->stop();
            mpCurrentSong = nullptr;
            mCurrentSongPath.clear();
            mFade = FADE_NONE;
        } else {
            mpCurrentSong->setVolume(nextVolume);
        }
    } else if (mpCurrentSong == nullptr && !mNextSongPath.empty()){
        PlaySong(mNextSongPath);
        mNextSongPath.clear();
    }
    mpSystem->update();
}


void FMOD_AudioManagerV0::LoadSFX(const std::string &path) {
    Load(CATEGORY_SFX, path);
}

void FMOD_AudioManagerV0::LoadSong(const std::string &path) {
    Load(CATEGORY_SONG, path);
}

void FMOD_AudioManagerV0::Load(Category type, const std::string& path) {
    // Check if sound path name is already stored in the corresponding map, if so, return
    if (mSounds[type].find(path) != mSounds[type].end()) return;

    // Create sound object and stream or load depending on the sound type
    FMOD::Sound* sound; // creating a pointer holding the address in which we will load the sound source
    CheckForErrors(mpSystem->createSound(path.c_str(), mModes[type], nullptr, &sound));

    // Store sound object in the corresponding map with the associated path name
    mSounds[type].insert(std::make_pair(path, sound));
}

// Playback

void FMOD_AudioManagerV0::PlaySFX(const std::string &path,
                                  float minVolume, float maxVolume,
                                  float minPitch, float maxPitch) {
    // If sound is not loaded it doesn't play
    SoundMap::iterator sound = mSounds[CATEGORY_SFX].find(path);
    if (sound == mSounds[CATEGORY_SFX].end()) return;

    // Calculate random volume and pitch in selected range
    float volume = RandomBetween(minVolume, maxVolume);
    float pitch = RandomBetween(minPitch, maxPitch);

    // Load into channel and Play sound effect with set values
    FMOD::Channel* channel;
    CheckForErrors(mpSystem->playSound(sound->second, mpGroups[CATEGORY_SFX], true, &channel));
    CheckForErrors(channel->setVolume(volume));
    float frequency;
    CheckForErrors(channel->getFrequency(&frequency));
    CheckForErrors(channel->setFrequency(ChangeSemitone(frequency,pitch)));
    CheckForErrors(channel->setPaused(false));
}

void FMOD_AudioManagerV0::PlaySong(const std::string &path) {
    // If the song is already playing, return
    if (path == mCurrentSongPath) return;
    if (mpCurrentSong != nullptr){
        StopSongs();
        mNextSongPath = path;
        return;
    }

    // If the song has not been loaded, return
    SoundMap::iterator sound = mSounds[CATEGORY_SONG].find(path);
    if (sound == mSounds[CATEGORY_SONG].end()) return;

    // Start playing song with volume set to 0 and fade in
    mCurrentSongPath = path;
    CheckForErrors(mpSystem->playSound(sound->second, mpGroups[CATEGORY_SONG], true, &mpCurrentSong));
    mpCurrentSong->setVolume(0.0f);
    mpCurrentSong->setPaused(false);
    mFade = FADE_IN;
}

void FMOD_AudioManagerV0::StopSFXs() {
    CheckForErrors(mpGroups[CATEGORY_SFX]->stop());
}

void FMOD_AudioManagerV0::StopSongs() {
    if (mpCurrentSong != nullptr)
        mFade = FADE_OUT;
    mNextSongPath.clear();
}

// Parameters

void FMOD_AudioManagerV0::SetMasterVolume(float volume) const {
    CheckForErrors(mpMaster->setVolume(volume));
}

void FMOD_AudioManagerV0::SetSFXsVolume(float volume) const {
    CheckForErrors(mpGroups[CATEGORY_SFX]->setVolume(volume));
}

void FMOD_AudioManagerV0::SetSongsVolume(float volume) const {
    CheckForErrors(mpGroups[CATEGORY_SONG]->setVolume(volume));
}


void FMOD_AudioManagerV0::CheckForErrors(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        std::cout << FMOD_ErrorString(result) << std::endl; exit(-1);
    }
}

float FMOD_AudioManagerV0::RandomBetween(float min, float max){
    if(min == max) return min;
    std::uniform_real_distribution<> floatDistr(min,max);
    float result = floatDistr(RandomNumberGen::getGen()); // retrieve the singleton instance of the generator
    printf("random: %f , ", result);
    return result;
}

float FMOD_AudioManagerV0::ChangeSemitone(float frequency, float variation) {
    static float semitone_ratio = pow(2.0f, 1.0f / 12.0f);
    float result =  frequency * pow(semitone_ratio, variation);
    printf("variation: %f, result: %f; \n", variation, result);
    return result;
}

#endif //FMOD_D
