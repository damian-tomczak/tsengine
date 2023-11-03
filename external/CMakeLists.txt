# TODO: refactor for https://github.com/KhronosGroup/Vulkan-Hpp.git
add_subdirectory(vulkan)
add_subdirectory(glslang)
add_subdirectory(openxr)
add_subdirectory(tinyobjloader)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CybSDK_Cpp)
    set(CYBSDK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CybSDK_Cpp)
endif()

if (EXISTS ${CYBSDK_DIR})
    message(STATUS "CybSDK was found: ${CYBSDK_DIR}")
else()
    message(WARNING
        "CybSDK was not found\n"
        "If you want to use Cyberith you have to copy CybSDK_Cpp to external dir "
        "or specify CYBSDK_CPP_DIR"
    )
endif()

if (EXISTS ${CYBSDK_DIR})
    if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        set(CYBSDK_PATH ${CYBSDK_DIR}/x64)
    else()
        set(CYBSDK_PATH ${CYBSDK_DIR}/x86)
    endif()
    find_library(CYBSDK_LIB
        NAMES CybSDK.lib
        PATHS ${CYBSDK_PATH}
        NO_DEFAULT_PATH
    )

    find_path(CYBSDK_INCLUDE_DIR
        NAMES include
        PATHS ${CYBSDK_DIR}
        NO_DEFAULT_PATH
    )

    if(NOT CYBSDK_LIB)
        message(FATAL_ERROR "CybSDK dir was found but lib can not be found!")
    endif()
    if(CYBSDK_INCLUDE_DIR)
        set(CYBSDK_INCLUDE_DIR ${CYBSDK_INCLUDE_DIR}/include PARENT_SCOPE)
    else()
        message(FATAL_ERROR "CybSDK dir was found but include dir can not be found!")
    endif()
endif()

file(GLOB_RECURSE EXTERNAL_DLL_FILES
    ${CYBSDK_PATH}/CybSDK.dll
)
set(EXTERNAL_DLL_FILES ${EXTERNAL_DLL_FILES} PARENT_SCOPE)