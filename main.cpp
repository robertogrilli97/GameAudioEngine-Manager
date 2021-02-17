#include <iostream>
#include <SFML/Window.hpp>

#ifdef FMOD_D
#include "FMOD_AudioManagerV0.h"
#include "FMOD_AudioManagerV1.h"
#endif // FMOD_D
#ifdef WWISE_D
#include "AK_AudioManagerV1.h"
#endif // WWISE_D

int main() {

    // Window and clock initialisation
    sf::Window window(sf::VideoMode(320,240), "AudioPlayback");
    sf::Clock clock;

//---WWISE ENGINE-------------------------------------------------------

#ifdef WWISE_D
    std::cout << "WWISE CODE" << std::endl;

    // Init the Audio Engine
    AK_AudioManagerV1::Init();
    AK_AudioManagerV1 audioManagerV1;

    std::string objectBank = "SFX_Test.bnk";

    std::string accordionAkObj = "SFX_Accordion";
    std::string accordionPlayEvent = "Play_SFX_Accordion";
    std::string windLongAkObj = "SFX_WindLong";
    std::string windLongPlayEvent = "Start_SFX_Wind_Long";
    std::string alienGunFireAkObj = "SFX_AlienGunFire";
    std::string alienGunFirePlayEvent = "Play_SFX_AlienGun_Fire";
    std::string doorLockAkObj = "SFX_DoorLock";
    std::string doorLockPlayEvent = "Play_SFX_DoorLock";

    // Start game loop
    while (window.isOpen())
    {
        // Run approx 60fps
        float elapsed = clock.getElapsedTime().asSeconds();
        if (elapsed < 1.0f / 60.0f) continue; // hold until 1/60 of second is elapsed, then continue the while loop (60fps)
        clock.restart(); // restart the clock so the next while cycle it will hold until 1/60 of sec
        sf::Event event {};

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Z){
                printf("Loading bank\n");
                audioManagerV1.LoadBank(objectBank);
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::X){
                printf("Registering GameObject\n");
                audioManagerV1.RegisterGameObject(accordionAkObj);
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::C){
                printf("Unregistering GameObject\n");
                audioManagerV1.UnRegisterGameObject(accordionAkObj);
            }
            if (event.type == sf::Event::KeyPressed &&
                 event.key.code == sf::Keyboard::V){
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::B){

            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::N){
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::M){
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Comma){
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Equal){
                audioManagerV1.UnloadBank(objectBank);
                // Post your own message to the CaptureLog of the AuthoringTool Profiler
                AK::Monitor::PostString("Bank Unloaded",AK::Monitor::ErrorLevel_Error);
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Space){
                printf("Posting Event\n");
                audioManagerV1.PostEvent(accordionPlayEvent, accordionAkObj);
            }

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Enter){
                window.close();
            }
        }
        // Place your update and draw logic here
//        audioManagerV0.ProcessAudio();
        AK_AudioManagerV1::Update();
    }
#endif // WWISE_D

//---FMOD ENGINE-------------------------------------------------------

#ifdef FMOD_D
    // System initialisation
//    FMOD_AudioManagerV0& audioManagerV0 = FMOD_AudioManagerV0::getInstance(); // Singleton

    FMOD_AudioManagerV1::Init();
    FMOD_AudioManagerV1 audioManagerV1;

    // LoadSFX sound sources into objects (see Assets folder for more SFXs)
    std::string musicPath1 = "../Assets/SFX_Wind_Long.wav"; // Add any music file
    std::string oneShot1 = "../Assets/SFX_DoorLock.wav";

//    audioManagerV0.LoadSong(musicPath1);
//    audioManagerV0.LoadSFX(oneShot1);

    audioManagerV1.LoadSound(musicPath1, false, false, true);
    audioManagerV1.LoadSound(oneShot1, false, false, false);

    int musicSongChannel;  // Int used to access the referenced channel in cache memory

    // Start game loop
    while (window.isOpen())
    {
        // Run approx 60fps
        float elapsed = clock.getElapsedTime().asSeconds();
        if (elapsed < 1.0f / 60.0f) continue; // hold until 1/60 of second is elapsed, then continue the while loop (60fps)
        clock.restart(); // restart the clock so the next while cycle it will hold until 1/60 of sec
        sf::Event event {};

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Z){
//                audioManagerV0.PlaySong(musicPath1);
                musicSongChannel = audioManagerV1.PlaySound(musicPath1, Vector3(), -3.f);
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::X){
                std::cout << audioManagerV1.IsPlaying(musicSongChannel) << std::endl;
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Space){
//                audioManagerV0.PlaySFX(oneShot1, 1, 1, -12, 12);
                audioManagerV1.PlaySound(oneShot1, Vector3(), -6.f);
                audioManagerV1.SetChannelVolume(musicSongChannel, -12.f);
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::M){
                audioManagerV1.StopAllChannels();
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Enter)
                window.close();
        }
        // Place your update and draw logic here
//        audioManagerV0.Update(elapsed);
        audioManagerV1.Update();
    }
    // Place your shutdown logic here
    FMOD_AudioManagerV1::Shutdown();
#endif // FMOD_D

}
