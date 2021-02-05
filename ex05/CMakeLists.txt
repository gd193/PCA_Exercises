project(ex05)

set(CMAKE_CXX_STANDARD 14)
set(MPI_FIX_FLAG "-Wall -Werror")
#set(CMAKE_CXX_FLAGS ${MPI_FIX_FLAG})

find_package(MPI)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
add_executable(PingPong PingPong.cpp)
add_executable(PingExchange PingExchange.cpp)
target_link_libraries(PingPong ${MPI_C_LIBRARIES})
target_link_libraries(PingExchange ${MPI_C_LIBRARIES})