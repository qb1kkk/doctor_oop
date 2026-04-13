cmake_minimum_required(VERSION 3.16)
project(doctor_oop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(doctor_oop
    src/main.cpp
    src/Disease.cpp
    src/Patient.cpp
    src/Doctor.cpp
    src/ClinicHttpServer.cpp
    src/Database.cpp
)

target_include_directories(doctor_oop PRIVATE src)

find_package(PostgreSQL REQUIRED)
target_link_libraries(doctor_oop PRIVATE PostgreSQL::PostgreSQL)
