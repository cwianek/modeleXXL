add_executable(app main.cpp)

include_directories(../MPIWrapper)

target_link_libraries(app MPIWrapper)
