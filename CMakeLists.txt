cmake_minimum_required(VERSION 3.13)
project(CPN_PNML_PARSE)

set(CMAKE_CXX_STANDARD 14)

add_executable(
        CPN_PNML_PARSE
        main.cpp
        CPN.h
        CPN.cpp
        condition_tree.cpp
        condition_tree.h
        tinystr.cpp
        tinystr.h
        tinyxml.cpp
        tinyxml.h
        tinyxmlerror.cpp
        tinyxmlparser.cpp
)