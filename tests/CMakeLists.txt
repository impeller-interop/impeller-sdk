cmake_minimum_required(VERSION 3.20)
project(impeller_sdk_native_test LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

get_filename_component(sdk_root "${CMAKE_CURRENT_LIST_DIR}/../sdk" ABSOLUTE)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(sdk_os "windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(sdk_os "macos")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(sdk_os "linux")
else()
  message(FATAL_ERROR "unsupported SDK platform: ${CMAKE_SYSTEM_NAME}")
endif()

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" sdk_processor)
if(sdk_processor STREQUAL "x86_64" OR sdk_processor STREQUAL "amd64")
  set(sdk_arch "x64")
elseif(sdk_processor STREQUAL "aarch64" OR sdk_processor STREQUAL "arm64")
  set(sdk_arch "arm64")
else()
  message(FATAL_ERROR "unsupported SDK architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(sdk_include_dir "${sdk_root}/include")
set(sdk_lib_dir "${sdk_root}/lib/${sdk_os}/${sdk_arch}")

if(NOT EXISTS "${sdk_lib_dir}")
  message(FATAL_ERROR "missing SDK library directory: ${sdk_lib_dir}")
endif()

add_executable(impeller_native_test native.c)
target_include_directories(impeller_native_test PRIVATE "${sdk_include_dir}")

if(MSVC)
  target_compile_options(impeller_native_test PRIVATE /W4 /WX)
else()
  target_compile_options(impeller_native_test PRIVATE -Wall -Wextra -Werror)
endif()

if(WIN32)
  set(sdk_runtime_name "impeller.dll")
  target_link_libraries(impeller_native_test PRIVATE "${sdk_lib_dir}/impeller.dll.lib")
elseif(APPLE)
  set(sdk_runtime_name "libimpeller.dylib")
  target_link_libraries(impeller_native_test PRIVATE "${sdk_lib_dir}/${sdk_runtime_name}")
  set_target_properties(impeller_native_test PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    BUILD_RPATH "@executable_path"
    INSTALL_RPATH "@executable_path"
  )
else()
  set(sdk_runtime_name "libimpeller.so")
  target_link_libraries(impeller_native_test PRIVATE "${sdk_lib_dir}/${sdk_runtime_name}")
  set_target_properties(impeller_native_test PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    BUILD_RPATH "$ORIGIN"
    INSTALL_RPATH "$ORIGIN"
  )
endif()

add_custom_command(TARGET impeller_native_test POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${sdk_lib_dir}/${sdk_runtime_name}"
    "$<TARGET_FILE_DIR:impeller_native_test>/${sdk_runtime_name}"
  VERBATIM
)

enable_testing()
add_test(NAME native COMMAND impeller_native_test)
