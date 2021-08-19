# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

project("library_zap")

#set(PUBLISH TRUE)

include_directories(src/main/cpp)
aux_source_directory(src/main/cpp/ DIR_SOURCE)
add_library(
        dolin-zap
        SHARED
        ${DIR_SOURCE}
)

#动态方式加载 STATIC：表示静态的.a的库 SHARED：表示.so的库
#add_library(dolin-comm SHARED IMPORTED)

#设置要连接的so的相对路径
#${CMAKE_SOURCE_DIR}：表示CMake.txt的当前文件夹路径
#${ANDROID_ABI}：编译时会自动根据CPU架构去选择相应的库
#set_target_properties(dolin-comm PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../soLibs/comm/${ANDROID_ABI}/libdolin-comm.so)
#if (${PUBLISH})
#    set_target_properties(dolin-comm PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../soLibs/comm/${ANDROID_ABI}/libdolin-comm.so)
#else ()
#    set_target_properties(dolin-comm PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../soLibs/comm/${ANDROID_ABI}/libdolin-comm.so)
#endif ()
#添加第三方头文件
#target_include_directories(dolin-zap PRIVATE ${CMAKE_SOURCE_DIR}/src/main/cpp/third_part)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        dolin-zap
#        dolin-comm
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})