 add_library(Wwise_Integration STATIC
        Common/AkDefaultLowLevelIODispatcher.cpp
        Common/AkDefaultLowLevelIODispatcher.h
        Common/AkFileLocationBase.cpp
        Common/AkFileLocationBase.h
        Common/AkFilePackage.cpp
        Common/AkFilePackage.h
        Common/AkFilePackageLowLevelIO.h
        Common/AkFilePackageLowLevelIO.inl
        Common/AkFilePackageLUT.cpp
        Common/AkFilePackageLUT.h
        Common/AkMultipleFileLocation.cpp
        Common/AkMultipleFileLocation.h
        POSIX/AkDefaultIOHookBlocking.cpp
        POSIX/AkDefaultIOHookBlocking.h
        POSIX/AkDefaultIOHookDeferred.cpp
        POSIX/AkDefaultIOHookDeferred.h
        POSIX/AkFileHelpers.h
        POSIX/AkFilePackageLowLevelIOBlocking.h
        POSIX/AkFilePackageLowLevelIODeferred.h
        POSIX/stdafx.cpp
        POSIX/stdafx.h
        )

set(WWISE_INCLUDE_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/../
        ${CMAKE_CURRENT_SOURCE_DIR}/POSIX
        ${CMAKE_CURRENT_SOURCE_DIR}/Common
        )
    # PUBLIC include directories will also pass their paths
    # to any parent linked target consuming this library
target_include_directories(Wwise_Integration PUBLIC ${WWISE_INCLUDE_PATH})

