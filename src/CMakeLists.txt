# Sme common variables
cmake_minimum_required(VERSION 3.14)
# set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=-*,google-*")
# set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Name of the project
project(CPP2_my_containers VERSION 1.0.0)

#clang-format 
file(GLOB_RECURSE ALL_SOURCE_FILES *.cc *.h)
add_custom_target(
   fmt
   ALL
   COMMAND clang-format
   -style=Google
   -i
   ${ALL_SOURCE_FILES}
)
#
# include (FetchContent)
# FetchContent_Declare(
# 	googletest
#   	## Specify the commit you depend on and update it regularly.
# 	# SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}../third-party/googletest"
# 	URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
# )
# # For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

## Alternatively this can be used if gtest package is
## installed system-wide from distribution's package repositories
find_package(GTest REQUIRED)

include(GoogleTest)

#lib
set(LIB_TARGET "my_containers")
add_library(${LIB_TARGET} STATIC 
	containers/my_vector.cc  
	containers/my_list.cc  
	containers/my_map.cc  
	containers/my_set.cc  
)
target_include_directories(${LIB_TARGET} PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_options(${LIB_TARGET} PUBLIC 
	-Wall
	-Wextra
	# -Werror
)
set(TEST_TARGET "test")
add_executable(${TEST_TARGET} 
	tests/test_main.cc
	tests/test_vector.cc
	tests/test_list.cc
	tests/test_array.cc
	tests/test_stack.cc
	tests/test_queue.cc
	tests/test_tree.cc
	tests/test_set.cc
	tests/test_map.cc
	tests/test_multiset.cc
	tests/test_coverage.cc
)
target_link_libraries(${TEST_TARGET} PRIVATE
	${LIB_TARGET}
	GTest::gtest_main
)
gtest_discover_tests(${TEST_TARGET})

# Source - https://stackoverflow.com/a/37967856
# Posted by galsh83, modified by community. See post 'Timeline' for change history
# Retrieved 2026-07-27, License - CC BY-SA 3.0
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    include(CodeCoverage)

    # 1. Добавляем флаги покрытия к тестовому таргету
    append_coverage_compiler_flags_to_target(${TEST_TARGET})

    # 2. Создаём цель для генерации отчёта (например, с lcov)
    setup_target_for_coverage_lcov(
        NAME coverage                   # имя цели: make coverage
        EXECUTABLE ${TEST_TARGET}       # исполняемый файл (тесты)
        DEPENDENCIES ${TEST_TARGET}     # сборка перед запуском
        BASE_DIRECTORY ${CMAKE_SOURCE_DIR}
		LCOV_ARGS --ignore-errors inconsistent,inconsistent 
		# GENHTML_ARGS --ignore-errors inconsistent
		EXCLUDE "${CMAKE_SOURCE_DIR}/tests/*" "/usr/include/*"
    )
endif()
