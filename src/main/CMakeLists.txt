include_directories(${INCLUDE_DIRECTORIES} include)

file(GLOB SOURCES "src/*.cpp")

add_executable(server ${SOURCES})

target_link_libraries(server osc_interface)
target_link_libraries(server websockets_interface)
target_link_libraries(server scope_interface)

if(UNIX)
    target_link_libraries(server pthread)
    if(NOT APPLE)
        target_link_libraries(server rt)
    endif()
endif()

target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(server ${OPENSSL_LIBRARIES})

if(MSYS)
    target_link_libraries(server ws2_32 wsock32)
endif()
