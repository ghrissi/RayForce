cmake_minimum_required(VERSION 3.10)
project(RayForce)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# 1. CONFIGURACIÓN DE COMPILACIÓN
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SOLUCIÓN AL ERROR DE PUNTEROS Y RUTAS
# -fpermissive: Ignora el error de cast de puntero a int (común en código viejo de Windows)
# -Wno-error: Asegura que los warnings no detengan la compilación
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -Wno-error")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
else()
    add_definitions(-DNDEBUG)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math")
endif()

add_definitions(-DDONT_USE_CUDA)

# 2. DEPENDENCIAS
find_package(raylib 5.5 QUIET) 
if(NOT PHYSX_PATH)
    set(PHYSX_PATH "/usr/local")
endif()
set(PHYSX_LIB_DIR "${PHYSX_PATH}/lib")

# ---------------------------------------------------------
# 3. TRUCO "BRUTE FORCE" PARA INCLUDES
# ---------------------------------------------------------
# Buscamos TODAS las subcarpetas dentro de RayForce/src
file(GLOB_RECURSE ALL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/RayForce/src/*.h")
set(MOTOR_DIRS "")
foreach(_header ${ALL_HEADERS})
    get_filename_component(_dir ${_header} DIRECTORY)
    list(APPEND MOTOR_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES MOTOR_DIRS)

file(GLOB_RECURSE ENGINE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/RayForce/src/*.cpp"
)

add_library(RayForceLib STATIC ${ENGINE_SOURCES})

# Añadimos CADA subcarpeta como un include path independiente (-I)
# Esto hace que "../entities/entity.h" se encuentre aunque la ruta relativa sea falsa
target_include_directories(RayForceLib BEFORE PUBLIC 
    ${MOTOR_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/RayForce/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/RayForce"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${PHYSX_PATH}/include"
    "${PHYSX_PATH}/include/physx"
)

# ---------------------------------------------------------
# 4. COMPILAR EJEMPLOS
# ---------------------------------------------------------
file(GLOB EXAMPLE_DIRS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/examples" "examples/*")

foreach(DIR_NAME ${EXAMPLE_DIRS})
    set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/examples/${DIR_NAME}")
    if(IS_DIRECTORY ${DIR})
        string(REPLACE " " "_" SAFE_NAME ${DIR_NAME})
        file(GLOB_RECURSE EXAMPLE_SOURCES "${DIR}/*.cpp")
        
        if(EXAMPLE_SOURCES)
            add_executable(${SAFE_NAME} ${EXAMPLE_SOURCES})

            target_include_directories(${SAFE_NAME} PRIVATE 
                "${DIR}" 
                "${CMAKE_CURRENT_SOURCE_DIR}/examples"
                "${CMAKE_CURRENT_SOURCE_DIR}"
                ${MOTOR_DIRS}
            )

            target_link_libraries(${SAFE_NAME}
                RayForceLib
                raylib m pthread dl rt GL X11
                "${PHYSX_LIB_DIR}/libPhysXExtensions_static_64.a"
                "${PHYSX_LIB_DIR}/libPhysXCooking_static_64.a"
                "${PHYSX_LIB_DIR}/libPhysX_static_64.a"
                "${PHYSX_LIB_DIR}/libPhysXCommon_static_64.a"
                "${PHYSX_LIB_DIR}/libPhysXFoundation_static_64.a"
            )
        endif()
    endif()
endforeach()
