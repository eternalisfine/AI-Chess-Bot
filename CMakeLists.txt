cmake_minimum_required(VERSION 3.10)

# Name of your project
project(ChessProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find curl library
find_package(CURL REQUIRED)

# Windows needs extra socket libraries
if(WIN32)
    set(EXTRA_LIBS ws2_32 wldap32 crypt32)
else()
    set(EXTRA_LIBS "")
endif()
 

# This is the important part: it links your files together
add_executable(test_1  
    Board.cpp
    test_step1.cpp
)

add_executable(test_2
    Board.cpp
    MoveValidator.cpp
    test_step2.cpp
)

add_executable(test_3
    Board.cpp
    MoveValidator.cpp
    GameEngine.cpp
    Player.cpp
    test_step3.cpp
)

add_executable(main
    Board.cpp
    MoveValidator.cpp
    GameEngine.cpp
    Player.cpp
    BotPlayer.cpp
    main.cpp
    OllamaClient.cpp
)

# Link curl — works on Arch, Ubuntu, Mac, Windows
target_include_directories(main PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(main PRIVATE CURL::libcurl ${EXTRA_LIBS})