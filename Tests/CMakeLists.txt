find_package(Catch2 3 REQUIRED)

add_executable(testlib TestsMain.cpp)

target_link_libraries(testlib PRIVATE PeanutGL Catch2::Catch2WithMain glad glfw readerwriterqueue)

