cmake_minimum_required(VERSION 3.10)

project(go)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# JSON
include(FetchContent)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

# CPR
FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG dd967cb48ea6bcbad9f1da5ada0db8ac0d532c06)



# Catch 2 pour faire des tests
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)

FetchContent_MakeAvailable(cpr json Catch2)


include_directories(src)
# ──────── MAIN GAME ────────
add_executable(go
    src/Chatbot.cpp
    src/main.cpp
    src/Devinette.cpp)

# CPR
target_link_libraries(go PRIVATE cpr::cpr nlohmann_json::nlohmann_json)


# ──────── TEST EXECUTABLE ────────
# Enable CTest support
enable_testing()
include(CTest)
include(Catch)

# Test 1
add_executable(test1
    tests/test1.cpp
    src/Chatbot.cpp
    src/Devinette.cpp
)

target_include_directories(test1 PRIVATE src)
target_link_libraries(test1
    PRIVATE Catch2::Catch2WithMain
    PRIVATE cpr::cpr nlohmann_json::nlohmann_json
)

target_compile_definitions(test1 PRIVATE TEST_MODE) # Define TEST_MODE for test1
catch_discover_tests(test1)

# Test 2
add_executable(test2
    tests/test2.cpp
    src/Devinette.cpp
    src/Chatbot.cpp
)
target_link_libraries(test2 PRIVATE Catch2::Catch2WithMain cpr::cpr nlohmann_json::nlohmann_json)
target_compile_definitions(test2 PRIVATE TEST_MODE)
target_include_directories(test2 PRIVATE src)
catch_discover_tests(test2)
