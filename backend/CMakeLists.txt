cmake_minimum_required(VERSION 3.14)
project(backend)

set(CMAKE_CXX_STANDARD 11)

# Автоматично изтегляне на cpp-httplib
include(FetchContent)
FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG        v0.15.3
)
FetchContent_MakeAvailable(httplib)

# Намиране на hiredis (клиента за Redis)
find_package(PkgConfig REQUIRED)
pkg_check_modules(HIREDIS REQUIRED hiredis)

add_executable(backend main.cpp)
target_include_directories(backend PRIVATE ${HIREDIS_INCLUDE_DIRS})
target_link_libraries(backend PRIVATE ${HIREDIS_LIBRARIES} httplib::httplib pthread)