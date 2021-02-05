
project(ex03)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenMP)
add_executable(ex04 main.cpp)
target_link_libraries(ex04 PUBLIC OpenMP::OpenMP_CXX)
