set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(back)

qt_add_executable(CrackHashWorker
  frontserver.hpp
  httpconnection.hpp
  settings.hpp
  main.cpp
)

target_link_libraries(CrackHashWorker BackLib)
