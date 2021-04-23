

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(ASSIMP REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})

file(GLOB testgamefiles
    "*.h"
    "*.cpp"
)

add_executable(testgame ${testgamefiles})
target_link_libraries(testgame ${GLEW_LIBRARIES})
target_link_libraries(testgame ${OPENGL_LIBRARIES})
target_link_libraries(testgame ${SDL2_LIBRARIES})
target_link_libraries(testgame ${ASSIMP_LIBRARIES})

add_custom_command(
         TARGET testgame POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E create_symlink
             ${CMAKE_SOURCE_DIR}/src/shaders $<TARGET_FILE_DIR:testgame>/shaders
         COMMENT "Symlinking shaders" VERBATIM
)



