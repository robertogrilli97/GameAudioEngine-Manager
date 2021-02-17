//
// Created by Bob
//
#ifdef WWISE_D

#include "AK_AudioManagerV1.h"


// ************* // pImpl implementation Code// ************************************************************************* //

struct Implementation {
    Implementation();
    ~Implementation();
    void SetFirstSettings();

    void Update();

    CAkFilePackageLowLevelIOBlocking g_lowLevelIO; // We're using the default Low-Level I/O implementation of the SDK's sample code
    const char* banksBasePath = PROJECT_DIR"/AKWAAPI_Test/GeneratedSoundBanks/Mac";

    typedef std::map<std::string, AkBankID> BankMap;
    typedef std::map<std::string, AkGameObjectID> AkGameObjectMap;
    typedef std::map<std::string, AkPlayingID> EventMap;

    static void CallbackFunc(AkCallbackType eType, AkCallbackInfo *pCallbackInfo) {

//            auto *pCookie = (Event*)pCallbackInfo->pCookie; // Not used
        auto* pCookie = (Implementation*) pCallbackInfo->pCookie;
        auto *pEventCallbackInfo = (AkEventCallbackInfo *) pCallbackInfo;

        if (eType == AK_EndOfEvent) {
            printf("End Of Event - PlayingID: %d, EventID: %d\n", pEventCallbackInfo->playingID, pEventCallbackInfo->eventID);
            if(!pCookie)
                return;

            // cookie holding sgpImplementation
            std::vector<EventMap::iterator> pStoppedEvents;
            for (auto it = pCookie->mEvents.begin(); it != pCookie->mEvents.end(); ++it)
                if (it->second == pEventCallbackInfo->playingID)
                    pStoppedEvents.push_back(it);

            for (auto& it : pStoppedEvents)
                pCookie->mEvents.erase(it); // erase the stopped event

            std::vector<AkGameObjectMap::iterator> pStoppedObjects;
            for (auto it = pCookie->mAkGameObjects.begin(); it != pCookie->mAkGameObjects.end(); ++it)
                if (it->second == pCallbackInfo->gameObjID)
                    pStoppedObjects.push_back(it);

            for (auto& it : pStoppedObjects){
                AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::UnregisterGameObj(it->second));
                pCookie->mAkGameObjects.erase(it); // erase the object
            }

            // cookie with Event - not used
//            pCookie->isPlaying = false;
//            printf("From Cookie Event: playingID %d; isPlaying, %d\n", pCookie->playingId, pCookie->isPlaying);
//            delete pCookie;
        }
    };

    BankMap mBanks;
    AkGameObjectMap mAkGameObjects;
    EventMap mEvents;

//    AkTimeMs lastTimeMs {};
    AkGameObjectID mNextGameObjectId = 1;
};

Implementation::Implementation() {
    // Create instance of the sound engine
    // Init Audio Engine //

    // Init Memory Manager
    AkMemSettings memSettings;
    AK::MemoryMgr::GetDefaultSettings(memSettings);
    AK_AudioManagerV1::ErrorCheck(AK::MemoryMgr::Init(&memSettings));
    // You can override the Memory Manger, refer to docs on site

    //
    // Init Stream Manager (default instance, can be overridden)
    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);

    // Customize Stream Manager settings here

    if (!AK::StreamMgr::Create(stmSettings))
    {assert(!"Could not create the Streaming Manager");}

    //
    // Create streaming device with blocking I/O handshaking (default device, can be overridden)
    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

    // Customize streaming device settings here

    // CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device
    // in the Stream Manager, and registers itself as the File Location Resolver.
    AK_AudioManagerV1::ErrorCheck(g_lowLevelIO.Init(deviceSettings));

    //
    // Create Sound Engine (default init parameters)

    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::Init(&initSettings, &platformInitSettings));

    //
    // Init Music Engine (default init parameters)
    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);
    AK_AudioManagerV1::ErrorCheck(AK::MusicEngine::Init(&musicInit));

    //
    // Init Spatial Audio (default init parameters)
    AkSpatialAudioInitSettings settings; // The constructor itself fills the variable with default values
    // with the recommended default settings
    AK_AudioManagerV1::ErrorCheck(AK::SpatialAudio::Init(settings));

    // Add communication with Authoring App if needed
#ifndef AK_OPTIMIZED
    AkCommSettings commSettings;
    AK::Comm::GetDefaultInitSettings(commSettings);
    AK_AudioManagerV1::ErrorCheck(AK::Comm::Init(commSettings));
#endif // AK_OPTIMIZED

    //
    // ********************************************************************************************
    //

    SetFirstSettings();
}

void Implementation::SetFirstSettings() {
    // Setup banks path
    AK_AudioManagerV1::ErrorCheck(g_lowLevelIO.SetBasePath(AKTEXT(banksBasePath)));
    // Setup language
    AK_AudioManagerV1::ErrorCheck(AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"))); // ???

    AkBankID bankID; // not used, banks can be unloaded with file name only
    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::LoadBank("Init.bnk", bankID));

    // Register a new game object as the main listener.
    AkGameObjectID MY_DEFAULT_LISTENER = 0;
    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::RegisterGameObj(MY_DEFAULT_LISTENER, "DefaultListener"));
    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::SetDefaultListeners(&MY_DEFAULT_LISTENER, 1));
}

Implementation::~Implementation() {
#ifndef AK_OPTIMIZED
    AK::Comm::Term(); // Terminate Comm Services
#endif // AK_OPTIMIZED
    // ?? Term Spatial Audio ??
    AK::MusicEngine::Term();
    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::UnloadBank(AKTEXT("Init.bnk"), nullptr));
    AK::SoundEngine::Term();

    // Terminate streaming device and streaming manager
    g_lowLevelIO.Term(); // it also destroy the associated streaming device
    if (AK::IAkStreamMgr::Get())
        AK::IAkStreamMgr::Get()->Destroy();

    AK::MemoryMgr::Term();
}

void Implementation::Update() {
    // Process bank requests, events, positions, RTPCs, etc.
    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::RenderAudio()); // With ErrorCheck?
}


// ************* // Static/Global Initialisation of the Audio engine // ********************************************* //

// static methods because we want init one instance only (as a singleton)
// we don't provide to this class an instance of the impl or a pointer to it
// neither we override the copy constructor and the assignment operator (to communicate with the impl class)

Implementation* sgpImplementation = nullptr;

void AK_AudioManagerV1::Init() {
    sgpImplementation = new Implementation;
}

void AK_AudioManagerV1::Update() {
    sgpImplementation->Update();
}

void AK_AudioManagerV1::Shutdown() {
    delete sgpImplementation;
}


// ************* // Interface Code, Functions definitions // ******************************************************** //


void AK_AudioManagerV1::LoadBank(const std::string& strBankName) {
    auto foundIt = sgpImplementation->mBanks.find(strBankName); // if not loaded in cache
    if (foundIt != sgpImplementation->mBanks.end()){
        std::cout << "Bank Already Loaded." << std::endl;
        return;
    }

    // Load banks from file name
    AkBankID bankID; // not used, banks can be unloaded with file name only
    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::LoadBank(strBankName.c_str(), bankID)); // then store in cache
/*
        // Alternatives
    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::LoadBank(AK::BANKS::OBJECTS));
    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::PrepareBank(AK::SoundEngine::Preparation_Load,"Objects.bnk"));
*/
    std::cout << "Bank Loaded." << std::endl;
    sgpImplementation->mBanks[strBankName] = bankID;
}

void AK_AudioManagerV1::UnloadBank(const std::string& strBankName) {
    // if loaded in cache, otherwise nothing
    auto foundIt = sgpImplementation->mBanks.find(strBankName);
    if (foundIt == sgpImplementation->mBanks.end()){
        std::cout << "Bank Already Unloaded." << std::endl;
        return;
    }

    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::UnloadBank(foundIt->second,nullptr));
//    AK::SoundEngine::PrepareBank(AK::SoundEngine::Preparation_Unload,foundIt->second);

    sgpImplementation->mBanks.erase(foundIt); // release from cache
    std::cout << "Bank Unloaded. " << std::endl;
}


AkGameObjectID AK_AudioManagerV1::RegisterGameObject(const std::string& strAkGameObjectName) {
    auto foundIt = sgpImplementation->mAkGameObjects.find(strAkGameObjectName);
    if (foundIt != sgpImplementation->mAkGameObjects.end()){
        std::cout << "Object already registered.\n" << std::endl;
        return foundIt->second;
    }

    AkGameObjectID gameObjectId = sgpImplementation->mNextGameObjectId++;

    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::RegisterGameObj( gameObjectId, strAkGameObjectName.c_str() ));
    sgpImplementation->mAkGameObjects[strAkGameObjectName] = gameObjectId;

    std::cout << "Object Resgistered: " << strAkGameObjectName << ", " << std::to_string(gameObjectId) << std::endl;
    return gameObjectId;
}

void AK_AudioManagerV1::UnRegisterGameObject(const std::string& strAkGameObjectName) {
    auto foundIt = sgpImplementation->mAkGameObjects.find(strAkGameObjectName);
    if (foundIt == sgpImplementation->mAkGameObjects.end()){
        std::cout << "Object not registered.\n" << std::endl;
        return;
    }

    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::UnregisterGameObj(foundIt->second));
    sgpImplementation->mAkGameObjects.erase(foundIt);
    std::cout << "Object Unregistered.\n" << std::endl;
}


// Playback

// With this function we can create/play one instance only of an event at a time
AkGameObjectID AK_AudioManagerV1::PostEvent(const std::string& strEventName, const std::string& strAkGameObjectName, bool unRegAkObj) {

    // if event is already found in map Stop and Play from start
    auto eventFoundIt = sgpImplementation->mEvents.find(strEventName);
    if(eventFoundIt != sgpImplementation->mEvents.end()){
        AK::SoundEngine::CancelEventCallback(eventFoundIt->second);
        AK::SoundEngine::StopPlayingID(eventFoundIt->second);
    }

    // if object is not yet registered, register it and then post event on it
    auto objFoundIt = sgpImplementation->mAkGameObjects.find(strAkGameObjectName);
    if (objFoundIt == sgpImplementation->mAkGameObjects.end()){
        auto regObj = RegisterGameObject(strAkGameObjectName);
        std::cout << "Object Created before event  posted, objID: " << regObj << std::endl;

        objFoundIt = sgpImplementation->mAkGameObjects.find(strAkGameObjectName);
        if (objFoundIt == sgpImplementation->mAkGameObjects.end()){
            std::cout << "Error creating the Ak object.\n" << std::endl;
            return regObj;}
    }

    // Additional method, not used // Post Event and save info on an Event object //
/*
    auto* pEvent = new Implementation::Event();
    std::cout << pEvent << std::endl;

    pEvent->playingId = AK::SoundEngine::PostEvent( strEventName.c_str(), objFoundIt->second,
                                                    AK_EndOfEvent | AK_EnableGetSourcePlayPosition,
                                                    &Implementation::Event::CallbackFunc,
                                                    pEvent );
*/
    // Additional method, not used // Prepared Post Event //
/*
    // (Implicit Media Loading: seek for media files in the whole project, here synchronously)
    AkUniqueID eventToPrepare = AK::EVENTS::PLAY_ACCORDION;
    AK::SoundEngine::PrepareEvent(AK::SoundEngine::Preparation_Load,&eventToPrepare,1);
*/

    auto eventPlayId = AK::SoundEngine::PostEvent( strEventName.c_str(), objFoundIt->second,
                                                   AK_EndOfEvent | AK_EnableGetSourcePlayPosition,
                                                   &Implementation::CallbackFunc, // Callback function
                                                   sgpImplementation); // Implementation instance as cookie to callback func

    if (eventPlayId == AK_INVALID_PLAYING_ID){
        std::cout << "Post Event Failed - Invalid Event, Check SoundBanks" << std::endl;
        return objFoundIt->second;
    }

    sgpImplementation->mEvents[strEventName] = eventPlayId;
    std::cout << "Posting event on Object name: " << objFoundIt->first << "; ObjectID: " << objFoundIt->second << std::endl;

    // Unregister Game Object // for one-shot sound that doesn't need game object
    if (unRegAkObj == true){
        UnRegisterGameObject(strAkGameObjectName);
        std::cout << "Object Unregistered after Posted." << std::endl;
    } else
        return objFoundIt->second;
}

void AK_AudioManagerV1::StopEvent(const std::string& strEventName, bool immediate) {
    auto eventFoundIt = sgpImplementation->mEvents.find(strEventName);
    if(eventFoundIt == sgpImplementation->mEvents.end())
        return;

    if (immediate){
        AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::ExecuteActionOnEvent(strEventName.c_str(),
                                                                            AK::SoundEngine::AkActionOnEventType_Stop,
                                                                            AK_INVALID_GAME_OBJECT,0,
                                                                            AkCurveInterpolation_SineRecip,
                                                                            eventFoundIt->second));
        return;
    }
    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::ExecuteActionOnEvent(strEventName.c_str(),
                                                                        AK::SoundEngine::AkActionOnEventType_Stop,
                                                                        AK_INVALID_GAME_OBJECT,
                                                                        1000, // One second fade time
                                                                        AkCurveInterpolation_SineRecip,
                                                                        eventFoundIt->second));
}

bool AK_AudioManagerV1::IsEventPlaying(const std::string &strEventName) const {
    auto foundIt = sgpImplementation->mEvents.find(strEventName);
    if (foundIt != sgpImplementation->mEvents.end())
        return true;

    return false;
}


// Parameters

void AK_AudioManagerV1::Set3dListenerAndOrientation(const Vector3 &position, const Vector3 &look, const Vector3 &up) {

    AkListenerPosition lisPosition;
    lisPosition.Set(VectorToAK(position), VectorToAK(look), VectorToAK(up));

    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::SetPosition(0, lisPosition));
//    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::SetPosition(sgpImplementation->mAkGameObjects["DefaultListener"],lisPosition));
}

void AK_AudioManagerV1::SetAkObject3dPosition(const std::string &strAkGameObjectName, const Vector3 &position) {
    auto foundIt = sgpImplementation->mAkGameObjects.find(strAkGameObjectName);
    if (foundIt == sgpImplementation->mAkGameObjects.end()){
        std::cout << "Object not registered.\n" << std::endl;
        return;
    }

    AkSoundPosition objPosition;
    objPosition.Set(VectorToAK(position),VectorToAK(Vector3(0,0,1)),VectorToAK(Vector3(0,1,0)));
    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::SetPosition(foundIt->second, objPosition));
}

void AK_AudioManagerV1::SetObjectDryVolume(const std::string& strAkGameObjectName, float volumedB) {
    auto foundIt = sgpImplementation->mAkGameObjects.find(strAkGameObjectName);
    if (foundIt == sgpImplementation->mAkGameObjects.end()){
        std::cout << "Set Volume Fail - Object not registered." << std::endl;
        return;
    }

    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::SetGameObjectOutputBusVolume(foundIt->second,AK_INVALID_GAME_OBJECT,dBToVolume(volumedB))); // in the range [0.0f:16.0f, -96.3dB to +24dB]
}

void AK_AudioManagerV1::SetMasterVolume(float volumedB) {
    AkOutputDeviceID outputId = AK::SoundEngine::GetOutputID(AK_INVALID_UNIQUE_ID,0);
    AK_AudioManagerV1::ErrorCheck(AK::SoundEngine::SetOutputVolume(outputId, dBToVolume(volumedB))); // [range 0.f:1.f , -96.3dB to 0dB]
}


void AK_AudioManagerV1::SetRTPC(const std::string& strRTPCName, float fValue, const std::string& strAkGameObjectName) {
    auto akGameObject = AK_INVALID_GAME_OBJECT;
    if (strAkGameObjectName != GLOBAL_SCOPE) { // if not global scope, search in map for object
        auto foundIt = sgpImplementation->mAkGameObjects.find(strAkGameObjectName);
        if (foundIt == sgpImplementation->mAkGameObjects.end()) {
            std::cout << "Set RTPC Fail - Object not registered." << std::endl;
            return;
        }
        akGameObject = foundIt->second;
    }
    AKRESULT eResult = AK::SoundEngine::SetRTPCValue(strRTPCName.c_str(), fValue, akGameObject);
    if (eResult == AK_IDNotFound)
        std::cout << "Invalid RTPC name" << std::endl;
}

void AK_AudioManagerV1::SetSwitch(const std::string &strSwitchGroup, const std::string &strSwitchState, const std::string &strAkGameObjectName) {
    auto foundIt = sgpImplementation->mAkGameObjects.find(strAkGameObjectName);
    if (foundIt == sgpImplementation->mAkGameObjects.end()) {
        std::cout << "Set Switch Fail - Object not registered." << std::endl;
        return;
    }
    AKRESULT eResult = AK::SoundEngine::SetSwitch(strSwitchGroup.c_str(), strSwitchState.c_str(), foundIt->second);
    if (eResult == AK_IDNotFound)
        std::cout << "Invalid Switch State or SwitchGroup name" << std::endl;
}

void AK_AudioManagerV1::SetState(const std::string &strStateGroup, const std::string &strState) {
    AKRESULT eResult = AK::SoundEngine::SetState(strStateGroup.c_str(), strState.c_str());
    if (eResult == AK_IDNotFound)
        std::cout << "Invalid State or StateGroup name" << std::endl;
}


void AK_AudioManagerV1::ErrorCheck(AKRESULT result) {
    if (result != AK_Success) {

        std::cout << "Error: " << result << std::endl;
        assert(result == AK_Success);
    }
}

void AK_AudioManagerV1::Check(const std::string& strEventName) {

    std::cout << sgpImplementation->mAkGameObjects[strEventName] << std::endl;

    // Info stored on Event Object
//    printf("CHECK - Name: %s, Playing ID: %d,  isPlaying: %d\n\n",
//           sgpImplementation->mEvents[strEventName]->eventName.c_str(),
//           sgpImplementation->mEvents[strEventName]->playingId,
//           sgpImplementation->mEvents[strEventName]->isPlaying);
}

#endif// WWWISE_D

