cmake_minimum_required(VERSION 3.10)
project(RayForce)

# 0. OUTPUT CONFIGURATION
# Executables will be placed in the /bin folder within the build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

# 1. BUILD CONFIGURATION
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization Flags for Linux (Performance focused)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math")
    add_definitions(-DNDEBUG)
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    add_definitions(-D_DEBUG)
endif()

# Disable CUDA for general Linux compatibility
add_definitions(-DDONT_USE_CUDA)

# 2. DEPENDENCY DISCOVERY
find_package(raylib 5.5 QUIET) 

# PhysX Path Setup
if(NOT PHYSX_PATH)
    set(PHYSX_PATH "/usr/local")
endif()

set(PHYSX_INCLUDE_DIR "${PHYSX_PATH}/include")
set(PHYSX_LIB_DIR "${PHYSX_PATH}/lib")

# 3. CORE LIBRARY DEFINITION
# We compile the main source code into a static library to avoid recompiling it for every example
file(GLOB_RECURSE CORE_SOURCES "RayForce/src/*.cpp")
add_library(${PROJECT_NAME}_lib STATIC ${CORE_SOURCES})

target_include_directories(${PROJECT_NAME}_lib PUBLIC 
    ${PHYSX_INCLUDE_DIR}
    "${PHYSX_INCLUDE_DIR}/physx"
    "${CMAKE_CURRENT_SOURCE_DIR}/RayForce/src"
)

# 4. AUTOMATIC EXAMPLES COMPILATION
# This block iterates through every subfolder in the "examples" directory
file(GLOB EXAMPLE_DIRS "examples/*")

foreach(DIR ${EXAMPLE_DIRS})
    if(IS_DIRECTORY ${DIR})
        # Extract the folder name to use as the executable name
        get_filename_component(EXAMPLE_NAME ${DIR} NAME)
        
        # Find all .cpp source files inside the specific example folder
        file(GLOB_RECURSE EXAMPLE_SOURCES "${DIR}/*.cpp")
        
        if(EXAMPLE_SOURCES)
            message(STATUS "Configuring example: ${EXAMPLE_NAME}")
            
            # Define the executable using the folder name
            add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCES})
            
            # Link the example with the core library and external dependencies
            target_link_libraries(${EXAMPLE_NAME}
                ${PROJECT_NAME}_lib
                raylib
                m pthread dl rt GL X11
                "${PHYSX_LIB_DIR}/libPhysXExtensions_static_64.a"
                "${PHYSX_LIB_DIR}/libPhysXCooking_static_64.a"
                "${PHYSX_LIB_DIR}/libPhysX_static_64.a"
                "${PHYSX_LIB_DIR}/libPhysXCommon_static_64.a"
                "${PHYSX_LIB_DIR}/libPhysXFoundation_static_64.a"
            )
        endif()
    endif()
endforeach()