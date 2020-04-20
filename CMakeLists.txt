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

set(ENV{PKG_CONFIG_PATH} "/opt/mongo-cxx-driver/lib/pkgconfig")
find_package(PkgConfig REQUIRED)
pkg_search_module(MONGOCXX REQUIRED IMPORTED_TARGET libmongocxx)

link_libraries(
        ${TORCH_LIBRARIES}
        PkgConfig::MONGOCXX
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
        "src/control/mdp.cpp"
        "src/control/actor.cpp"
        "src/control/actorcritic/acactor.cpp"
        "src/control/actorcritic/trainer.cpp"
        "src/control/sac/replay_buffer.cpp"
        "src/control/sac/sacactor.cpp"
        "src/control/distributions/distribution.cpp"
        "src/control/distributions/normal.cpp"
        "src/control/distributions/bernoulli.cpp"
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
        "src/control/actor.h"
        "src/control/actorcritic/acactor.h"
        "src/control/actorcritic/trainer.h"
        "src/control/sac/sacactor.h"
        "src/control/sac/replay_buffer.h"
        "src/control/controller.h"
        "src/control/mdp.h"
        "src/control/distributions/distribution.h"
        "src/control/distributions/normal.h"
        "src/control/distributions/bernoulli.h"
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
