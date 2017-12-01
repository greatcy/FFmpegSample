cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_BUILD_TYPE "Debug")

add_library( # Sets the name of the library.
             ffmpeg-jni-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp)

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

#添加编译头文件
include_directories(${PROJECT_SOURCE_DIR}/src/main/cpp/include)

#添加avcodec-57编译链接
add_library( avcodec-57
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
              avcodec-57
              # Specifies the parameter you want to define.
              PROPERTIES IMPORTED_LOCATION
              # Provides the path to the library you want to import.
              ${PROJECT_SOURCE_DIR}/nativeLibs/${ANDROID_ABI}/libavcodec-57.so )

#添加avdevice-57
add_library( avdevice-57
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
              avdevice-57
              # Specifies the parameter you want to define.
              PROPERTIES IMPORTED_LOCATION
              # Provides the path to the library you want to import.
              ${PROJECT_SOURCE_DIR}/nativeLibs/${ANDROID_ABI}/libavdevice-57.so )

#添加avfilter-6
add_library( avfilter-6
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
              avfilter-6
              # Specifies the parameter you want to define.
              PROPERTIES IMPORTED_LOCATION
              # Provides the path to the library you want to import.
              ${PROJECT_SOURCE_DIR}/nativeLibs/${ANDROID_ABI}/libavfilter-6.so )


#添加avformat-57
add_library( avformat-57
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
              avformat-57
              # Specifies the parameter you want to define.
              PROPERTIES IMPORTED_LOCATION
              # Provides the path to the library you want to import.
              ${PROJECT_SOURCE_DIR}/nativeLibs/${ANDROID_ABI}/libavformat-57.so )


#添加avutil-55
add_library( avutil-55
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
              avutil-55
              # Specifies the parameter you want to define.
              PROPERTIES IMPORTED_LOCATION
              # Provides the path to the library you want to import.
              ${PROJECT_SOURCE_DIR}/nativeLibs/${ANDROID_ABI}/libavutil-55.so )


#添加postproc-54
add_library( postproc-54
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
              postproc-54
              # Specifies the parameter you want to define.
              PROPERTIES IMPORTED_LOCATION
              # Provides the path to the library you want to import.
              ${PROJECT_SOURCE_DIR}/nativeLibs/${ANDROID_ABI}/libpostproc-54.so )

#添加swresample-2
add_library( swresample-2
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
              swresample-2
              # Specifies the parameter you want to define.
              PROPERTIES IMPORTED_LOCATION
              # Provides the path to the library you want to import.
              ${PROJECT_SOURCE_DIR}/nativeLibs/${ANDROID_ABI}/libswresample-2.so )

#添加swscale-4
add_library( swscale-4
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
              swscale-4
              # Specifies the parameter you want to define.
              PROPERTIES IMPORTED_LOCATION
              # Provides the path to the library you want to import.
              ${PROJECT_SOURCE_DIR}/nativeLibs/${ANDROID_ABI}/libswscale-4.so )


#message(STATUS "The path "${PROJECT_SOURCE_DIR}/nativeLibs/${ANDROID_ABI}/libavcodec-57.so)

target_link_libraries( # Specifies the target library.
                       ffmpeg-jni-lib

                       avdevice-57
                       avfilter-6
                       avformat-57
                       avutil-55
                       postproc-54
                       swresample-2
                       swscale-4
                       avcodec-57

                       ${log-lib})