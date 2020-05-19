cmake_minimum_required(VERSION 3.10)
project(dicom_tools)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Required for DCMTK
add_definitions(-DHAVE_CONFIG_H)


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif ()


# Find OpenMP
IF(USE_OPENMP)
    SET(OpenMP_CXX_FLAGS "-fopenmp")
    IF(APPLE)
        IF(CLANG_COMPILER)
            SET(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
            SET(OpenMP_CXX_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
            SET(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
            SET(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
            SET(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
            SET(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
        ENDIF()
    ENDIF()
    FIND_PACKAGE(OpenMP QUIET)
    IF(OpenMP_FOUND)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        ADD_DEFINITIONS(-DOPENMP_ENABLED)
    ENDIF()
ENDIF()


# our source file
set(SOURCE_FILES dicom_tools/main.cpp dicom_tools/filesystem.cpp dicom_tools/filesystem.h dicom_tools/tags.h dicom_tools/database.cpp dicom_tools/database.h dicom_tools/io/cli.h)

# search for DCMTK library and header files
find_package(DCMTK REQUIRED)
include_directories(${DCMTK_INCLUDE_DIRS})

find_package(Boost 1.62 COMPONENTS program_options filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(SQLite3 REQUIRED)
include_directories(${SQLITE3_INCLUDE_DIRS})

add_executable(dicomtosql ${SOURCE_FILES})

target_link_libraries(dicomtosql ${DCMTK_LIBRARIES} ${Boost_LIBRARIES} SQLite::SQLite3)
