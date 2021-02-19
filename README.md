# GameAudioEngine-Manager

## Introduction

Hello there! In this project, I wrapped the FMOD and Wwise APIs each in their own **Audio Manager** class.

Both [FMOD](https://fmod.com/resources/documentation-api?version=2.1&page=welcome.html) and the [Wwise](https://www.audiokinetic.com/fr/library/edge/?source=SDK&id=index.html) APIs provide their audio engine; the audio manager controls the calls to that engine.
The project shows the default implementation of the APIs and provides an environment in which to test each audio engine.

## Information

- The project is built on macOS with CMake;

- The [SFML API](https://www.sfml-dev.org/index.php) is employed to generate the game loop. 
[Download](https://www.sfml-dev.org/download/sfml/2.5.1/) it and follow the [installation instructions](https://www.sfml-dev.org/tutorials/2.5/start-osx.php), install the API as a framework:
Copy the content of *Frameworks* to `/Library/Frameworks`.

- FMOD and Wwise *libraries* and *header* files are contained in the **includes** folder.
It is required to extract the `lib.zip` within `./includes/Wwise`. (I am using the lib files to build the Wwise sound engine *Profile* Configuration)

- The folder `./includes/Wwise/StreamManager` contains the default implementation files of the Wwise audio engine *StreamManager*.

- **AKWAAPI_Test** folder contains the Wwise authoring tool project, containing the generated soundbanks and more…

- **Assets** folder contains royalty-free audio files used in the project. The files were downloaded from the [Game Audio GDC Archive](https://sonniss.com/gameaudiogdc/) and [freesound.org](https://freesound.org/)

## Build 
All the files needed to build FMOD and Wwise audio engines are present in the **includes** folder.

The project should build two different configurations; one employs the FMOD engine (with two distinct versions of the *AudioManager* class V0/V1) and the other employs the Wwise engine. 

From within the project directory:

### FMOD
```
$ cmake -S . -B cmake-build-debug_fmod -DFMOD_D=1
$ cd cmake-build-debug_fmod
$ make
$ ./GameAudioEngine 
```

### WWISE
```
$ cmake -S . -B cmake-build-debug_wwise -DWWISE_D=1 
$ cd cmake-build-debug_wwise
$ make 
$ ./GameAudioEngine 
```

Each build defines a *preprocessor directive* macro (**FMOD_D** or **WWISE_D**) that through conditional inclusions includes/excludes parts of the code for the corresponding build.

## Further work

... Virtual voices states, a step-by-step [tutorial blogpost](https://robertogrilli.home.blog/audio_blog/) and more...

## References

Gouveia, D. (2013). *Getting Started with C++ Audio Programming for Game Development*. Packt Publishing.

Somberg, G. (2015). *How to Build an Audio Engine*. GDC2015. Available at: [link](https://www.gdcvault.com/play/1022060/How-to-Write-an-Audio) [Accessed 02/2021].

Somberg, G. (2017). *Game Audio Programming: Principles and Practices*. CRC Press.

Claborn, C. (2018). *FmodStudioEngine*. GitHub. Available at: [link](https://github.com/cxsquared/FmodStudioEngine) [Accessed 02/2021]
