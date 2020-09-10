
file(GLOB imagenetConsoleSources *.cpp)
file(GLOB imagenetConsoleIncludes *.h )


ADD_LIBRARY(pmlib SHARED IMPORTED)
SET_TARGET_PROPERTIES(pmlib PROPERTIES IMPORTED_LOCATION /home/nvidia/software/PMLib/Python/client/libpmlib.so)

cuda_add_executable(imagenet-console ${imagenetConsoleSources})
target_link_libraries(imagenet-console nvcaffe_parser nvinfer jetson-inference pmlib)

