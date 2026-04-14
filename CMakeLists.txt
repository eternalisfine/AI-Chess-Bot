cmake_minimum_required(VERSION 3.10)

# Name of your project
project(ChessProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This is the important part: it links your files together
add_executable(chess_game 
    test_step1.cpp 
    Board.cpp
)