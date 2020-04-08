cmake_minimum_required (VERSION 3.10)

project (spacerl)

set(HALLEY_PROJECT_EMBED 1)
set(HALLEY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/halley)
set(HOTRELOAD 0 CACHE BOOL "Sets whether to hot-reload this project")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${HALLEY_PATH}/cmake/")
include(HalleyProject)

set(CHIPMUNKPP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/chipmunkpp)

set(CMAKE_PREFIX_PATH "/home/amiani/projects/spacerl/lib/libtorch")
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_library(libchipmunk STATIC IMPORTED)
set_target_properties(libchipmunk PROPERTIES IMPORTED_LOCATION "${CHIPMUNKPP_PATH}/chipmunk/build/src/libchipmunk.a")
add_library(libchipmunkpp STATIC IMPORTED)
set_target_properties(libchipmunkpp PROPERTIES IMPORTED_LOCATION "${CHIPMUNKPP_PATH}/libchipmunk++.a")

link_libraries(
        ${TORCH_LIBRARIES}
        libchipmunkpp
        libchipmunk
)

include_directories(
        ${CHIPMUNKPP_PATH}/src
        ${CHIPMUNKPP_PATH}/chipmunk/include/chipmunk
)


set (SOURCES
        "src/game.cpp"
        "src/control/controller.cpp"
        "src/control/policy.cpp"
        "src/control/observation.cpp"
        "src/services/input_service.cpp"
        "src/services/controller_service.cpp"
        "src/services/ship_service.cpp"
        "src/services/asteroid_service.cpp"
        "src/title_stage.cpp"
        "src/game_stage.cpp"
        "src/utils.cpp"
        "src/config/weapon_config.cpp"
        "src/config/asteroid_config.cpp"
        "src/config/ship_config.cpp"
     )

set (HEADERS
        "src/game.h"
        "src/services/input_service.h"
        "src/services/controller_service.h"
        "src/services/ship_service.h"
        "src/services/asteroid_service.h"
        "src/title_stage.h"
        "src/game_stage.h"
        "src/control/action.h"
        "src/control/observation.h"
        "src/control/policy.h"
        "src/control/controller.h"
        "src/utils.h"
        "src/config/hardpoint.h"
        "src/config/weapon_config.h"
        "src/config/asteroid_config.h"
        "src/config/ship_config.h"
     )

set(GEN_DEFINITIONS
        )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-copy")
halleyProject(spacerl "${SOURCES}" "${HEADERS}" "${GEN_DEFINITIONS}" ${CMAKE_CURRENT_SOURCE_DIR}/bin)
