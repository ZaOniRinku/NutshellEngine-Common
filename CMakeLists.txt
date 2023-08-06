cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
set(NAME NutshellEngine-Common)
project(${NAME} VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND NTSHENGN_COMMON_DEFINES NTSHENGN_COMPILER_GCC)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND NTSHENGN_COMMON_DEFINES NTSHENGN_COMPILER_CLANG)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND NTSHENGN_COMMON_DEFINES NTSHENGN_COMPILER_MSVC)
else()
    message(WARNING "Using an untested compiler '${CMAKE_CXX_COMPILER_ID}'. "
        "Consider using GCC, Clang or MSVC.")
    list(APPEND NTSHENGN_COMMON_DEFINES NTSHENGN_COMPILER_UNKNOWN)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND NTSHENGN_COMMON_DEFINES NTSHENGN_OS_WINDOWS _CRT_SECURE_NO_WARNINGS NOMINMAX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND NTSHENGN_COMMON_DEFINES NTSHENGN_OS_LINUX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND NTSHENGN_COMMON_DEFINES NTSHENGN_OS_MACOS)
endif()

set(NTSHENGN_DEFINES ${NTSHENGN_COMMON_DEFINES} PARENT_SCOPE)

set(ASSET_MANAGER_SOURCES
    asset_manager/ntshengn_asset_manager.cpp)
set(ASSET_MANAGER_HEADERS
    asset_manager/ntshengn_asset_manager.h)

set(ECS_COMPONENTS_HEADERS
    ecs/components/ntshengn_ecs_aabb_collidable.h
    ecs/components/ntshengn_ecs_audio_emitter.h
    ecs/components/ntshengn_ecs_audio_listener.h
    ecs/components/ntshengn_ecs_camera.h
    ecs/components/ntshengn_ecs_capsule_collidable.h
    ecs/components/ntshengn_ecs_light.h
    ecs/components/ntshengn_ecs_renderable.h
    ecs/components/ntshengn_ecs_rigidbody.h
    ecs/components/ntshengn_ecs_scriptable.h
    ecs/components/ntshengn_ecs_sphere_collidable.h
    ecs/components/ntshengn_ecs_transform.h)

set(ECS_SOURCES
    ecs/ntshengn_ecs.cpp)
set(ECS_HEADERS
    ${ECS_COMPONENTS_HEADERS}
    ecs/ntshengn_ecs.h)

set(JOB_SYSTEM_SOURCES
    job_system/ntshengn_job_system.cpp)
set(JOB_SYSTEM_HEADERS
    job_system/ntshengn_job_queue.h
    job_system/ntshengn_job_system.h)

set(MODULE_INTERFACES_HEADERS
    module_interfaces/ntshengn_asset_loader_module_interface.h
    module_interfaces/ntshengn_audio_module_interface.h
    module_interfaces/ntshengn_graphics_module_interface.h
    module_interfaces/ntshengn_module_interface.h
    module_interfaces/ntshengn_physics_module_interface.h
    module_interfaces/ntshengn_system_module_interface.h
    module_interfaces/ntshengn_window_module_interface.h)

set(RESOURCES_HEADERS
    job_system/ntshengn_resources_audio.h
    job_system/ntshengn_resources_graphics.h
    job_system/ntshengn_resources_physics.h
    job_system/ntshengn_resources_scripting.h
    job_system/ntshengn_resources_window.h)

set(UTILS_SOURCES
    ntshengn_utils_buffer.cpp
    ntshengn_utils_file.cpp
    ntshengn_utils_json.cpp
    ntshengn_utils_math.cpp)
set(UTILS_HEADERS
    ntshengn_defines.h
    ntshengn_enums.h
    ntshengn_utils_bimap.h
    ntshengn_utils_buffer.h
    ntshengn_utils_file.h
    ntshengn_utils_json.h
    ntshengn_utils_math.h)

set(SOURCES ${ASSET_MANAGER_SOURCES} ${ECS_SOURCES} ${JOB_SYSTEM_SOURCES} ${UTILS_SOURCES})
set(HEADERS ${ASSET_MANAGER_HEADERS} ${ECS_SOURCES} ${JOB_SYSTEM_HEADERS} ${MODULE_INTERFACES_HEADERS} ${RESOURCES_HEADERS} ${UTILS_HEADERS})
    
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_options(${PROJECT_NAME}
	PUBLIC
		$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror>)

target_compile_definitions(${PROJECT_NAME}
	PUBLIC
		${NTSHENGN_DEFINES}
		$<$<CONFIG:Debug>:NTSHENGN_DEBUG>
		$<$<CONFIG:MinSizeRel>:NTSHENGN_RELEASE>
		$<$<CONFIG:Release>:NTSHENGN_RELEASE>
		$<$<CONFIG:RelWithDebInfo>:NTSHENGN_DEBUG>)