add_library(coders coders/ShannonFanoCoder.cpp coders/LZ77Coder.cpp coders/LZWCoder.cpp)
add_library(packers common/Packer.cpp common/Unpacker.cpp)
add_executable(coding
    main.cpp
)
