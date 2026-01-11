cmake_minimum_required(VERSION 3.10)
project(BrickBattle)

# 1. OPTIMIZACIÓN AGRESIVA PARA RASPBERRY PI 5
# Forzamos modo Release si no se especifica
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Banderas específicas para el procesador de la Pi 5 (Cortex-A76)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # -O3: Máxima optimización de velocidad
    # -ffast-math: Acelera cálculos matemáticos (vital para físicas)
    # -fno-common: Mejora el acceso a variables globales
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -fno-common")
    add_definitions(-DNDEBUG)
    message(STATUS "Optimizaciones de Raspberry Pi 5 ACTIVADAS")
else()
    add_definitions(-D_DEBUG)
endif()

add_definitions(-DDONT_USE_CUDA)

# 2. Rutas de PhysX
set(PHYSX_INCLUDE_DIR "/usr/local/include")
set(PHYSX_LIB_DIR "/usr/local/lib")

# 3. Búsqueda de código
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 4. Definir el ejecutable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_executable(BrickBattle ${SOURCES})

# 5. Includes
target_include_directories(BrickBattle PUBLIC 
    ${PHYSX_INCLUDE_DIR}
    "${PHYSX_INCLUDE_DIR}/physx"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# 6. ENLACE DE LIBRERÍAS (Orden optimizado para GCC)
# En Linux, el orden de las librerías estáticas es de vida o muerte
target_link_libraries(BrickBattle
    # Raylib y Gráficos
    raylib
    GL
    X11
    
    # PhysX (Orden de dependencia: de más complejo a base)
    ${PHYSX_LIB_DIR}/libPhysXExtensions_static_64.a
    ${PHYSX_LIB_DIR}/libPhysXCooking_static_64.a
    ${PHYSX_LIB_DIR}/libPhysX_static_64.a
    ${PHYSX_LIB_DIR}/libPhysXCommon_static_64.a
    ${PHYSX_LIB_DIR}/libPhysXFoundation_static_64.a

    # Sistema (Siempre al final para resolver símbolos de las anteriores)
    m
    pthread
    dl
    rt
)