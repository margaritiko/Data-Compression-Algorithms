include_directories(../src)

include_directories(.)

add_executable(tests 
    # array tests
    packing_test.cpp
    coders_test.cpp
    # packers sources
    ../src/common/Packer.cpp
    ../src/common/Unpacker.cpp
    # coders sources
    ../src/coders/LZ77Coder.cpp
    ../src/coders/LZWCoder.cpp
    ../src/coders/ShannonFanoCoder.cpp
    # gtest sources
    gtest/gtest-all.cc
    gtest/gtest_main.cc
)

# add pthread for unix systems
if (UNIX)
    target_link_libraries(tests pthread)
endif ()