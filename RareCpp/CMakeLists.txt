cmake_minimum_required(VERSION 3.10)

set(This RareCpp)

set(Headers
  Main.h
)
set(Sources
  Main.cpp
)

add_executable(${This} ${Sources} ${Headers})
