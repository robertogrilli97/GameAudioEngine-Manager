cmake_minimum_required(VERSION 3.15)
project(GameAudioEngine)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp FMOD_AudioManagerV1.cpp FMOD_AudioManagerV1.h FMOD_AudioManagerV0.cpp FMOD_AudioManagerV0.h AK_AudioManagerV1.cpp AK_AudioManagerV1.h)

target_compile_definitions(${PROJECT_NAME} PUBLIC PROJECT_DIR="${PROJECT_SOURCE_DIR}")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG=1)
endif()

    ## SFML API ##
find_library(SFML_F SFML CMAKE_FRAMEWORK_PATH)
find_library(SFML-GRAPHICS_F sfml-graphics CMAKE_FRAMEWORK_PATH)
find_library(SFML-NETWORK_F sfml-network CMAKE_FRAMEWORK_PATH)
find_library(SFML-SYSTEM_F sfml-system CMAKE_FRAMEWORK_PATH)
find_library(SFML-WINDOW_F sfml-window CMAKE_FRAMEWORK_PATH)
set(SFML_FRAMEWORKS ${SFML_F} ${SFML-GRAPHICS_F} ${SFML-NETWORK_F} ${SFML-SYSTEM_F} ${SFML-WINDOW_F})
target_link_libraries(${PROJECT_NAME} ${SFML_FRAMEWORKS})

    ## FMOD API ##
if (FMOD_D)
    target_compile_definitions(${PROJECT_NAME} PUBLIC FMOD_D=1)

    set(FMOD_INCLUDE_PATHS
            ${PROJECT_SOURCE_DIR}/includes/FMOD/api/core/inc
            ${PROJECT_SOURCE_DIR}/includes/FMOD/api/studio/inc
            )
    target_include_directories(${PROJECT_NAME} PUBLIC ${FMOD_INCLUDE_PATHS})

    # Gather libraries with find_library()
    find_library(FMOD_LIB NAMES fmod PATHS "${PROJECT_SOURCE_DIR}/includes/FMOD/api/core/lib")
    find_library(FMODL_LIB fmodL "${PROJECT_SOURCE_DIR}/includes/FMOD/api/core/lib")
    find_library(FMODSTUDIO_LIB fmodstudio "${PROJECT_SOURCE_DIR}/includes/FMOD/api/studio/lib")
    find_library(FMODSTUDIOL_LIB fmodstudioL "${PROJECT_SOURCE_DIR}/includes/FMOD/api/studio/lib")

    target_link_libraries(${PROJECT_NAME}
            ${FMOD_LIB} ${FMODL_LIB} ${FMODSTUDIO_LIB} ${FMODSTUDIOL_LIB}
            )

    ## WWISE API ##
elseif (WWISE_D)

    target_compile_definitions(${PROJECT_NAME} PUBLIC WWISE_D=1)

    add_subdirectory(${PROJECT_SOURCE_DIR}/includes/Wwise/StreamManager)

    # No need to specify again directories because previously included in the library in the subfolder
#    set(WWISE_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/includes/Wwise)
#    target_include_directories(${PROJECT_NAME} PUBLIC ${WWISE_INCLUDE_PATH})

    # Find Mac Frameworks needed for the Wwise library to work
    find_library(AUDIOUNIT_F AudioUnit CMAKE_FRAMEWORK_PATH)
    find_library(AUDIOTOOLBOX_F AudioToolbox CMAKE_FRAMEWORK_PATH)
    find_library(COREAUDIO_F CoreAudio CMAKE_FRAMEWORK_PATH)
    find_library(COREFOUNDATION_F CoreFoundation CMAKE_FRAMEWORK_PATH)
    set(MAC_FRAMEWORKS ${AUDIOUNIT_F} ${AUDIOTOOLBOX_F} ${COREAUDIO_F} ${COREFOUNDATION_F})

    # Gather libraries with target_link_directories() and set()
    target_link_directories(${PROJECT_NAME} PRIVATE
            ${PROJECT_SOURCE_DIR}/includes/Wwise/lib
            ${PROJECT_SOURCE_DIR}/includes/Wwise/StreamManager # load the default StreamManager
            )
    set(AK_LIBRARIES Wwise_Integration
            AKSoundEngine AkMemoryMgr AkStreamMgr AkMusicEngine AkSpatialAudio CommunicationCentral)
    target_link_libraries(${PROJECT_NAME}
            ${MAC_FRAMEWORKS}
            ${AK_LIBRARIES}
            )

endif()

# Alternative way to target framework libraries
#
# target_link_libraries(${PROJECT_NAME} stdc++ "-framework AudioUnit" "-framework AudioToolbox"
#        "-framework CoreAudio" "-framework CoreFoundation" objc)