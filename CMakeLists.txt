cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)

#>References
#----------------------------------------------------------------------------------------------------
# http://wagavulin.hatenablog.com/entry/2011/11/27/222642
# Cmake samples

# http://stackoverflow.com/questions/9689183/cmake-googletest
# To integrate cmake and google test

# https://github.com/lbaehren/CMakePorts/blob/master/ports/database/sqlite/CMakeLists.txt
# To integrate cmake and SQLite

# https://github.com/snikulov/sqlite.cmake.build/blob/master/CMakeLists.txt
# integrate SQLite into CMake

# http://stackoverflow.com/questions/10522654/ruby-and-swig-with-cmake
# To build ruby extension by using swig on cmake

# https://github.com/libyui/libyui-bindings
# sample

# http://www.swig.org/Doc1.3/Introduction.html#Introduction%5Fbuild%5Fsystem
# A sample of integratation swig and cmake for python

# http://wiki.livedoor.jp/kou1okada/d/CMake
# To cross compile with toolchain (MinGW etc)
#----------------------------------------------------------------------------------------------------
#>End


#>Includes
#----------------------------------------------------------------------------------------------------
include(ExternalProject)
#----------------------------------------------------------------------------------------------------
#>End


#>Options   Turn on with 'cmake -Dvarname=ON'.
#----------------------------------------------------------------------------------------------------
#option(varname text defaultvalue)
option(test  "Build all tests." OFF)
option(quick "Quick build without check all external projects." OFF)
option(skip_gtest "Quick build without check gtest." OFF)
option(skip_sqlite "Quick build without check sqlite." OFF)
option(main  "Build main." OFF)
option(qt    "Build on Qt5" OFF)
#----------------------------------------------------------------------------------------------------
#>End


#>Variavles
#----------------------------------------------------------------------------------------------------
set(PROJECT_NAME   "fuzzysynony")
set(SRC            "src")
set(INCLUDE        "include")
set(CMAKELISTS_TXT "CMakeLists.txt")

#for installing thirdparty library
set(THIRDPARTY       "ThirdParty")
set(TH_DEBUG         "DebugLibs")
set(TH_RELEASE       "ReleaseLibs")
set(TH_BINARY_PREFIX "-build")
set(TH_STATIC_PREFIX "-static")

#for googletest
set(GOOGLETEST           "googletest")
set(GTEST_LIB_NAME       "gtest")
set(GTEST_REPO           "http://googletest.googlecode.com/svn/trunk/")
set(GTEST_REV            "675")
set(GTEST_PATCH_FILENAME "gtest.patch")

#for sqlite
set(SQLITE                "sqlite")
set(SQLITE_VERSION        "3080100")
set(SQLITE_MAJOR_VERSION  "3")
set(SQLITE_LIB_NAME       "${SQLITE}${SQLITE_MAJOR_VERSION}${TH_STATIC_PREFIX}")
set(SQLITE_SOURCE_TYPE    "amalgamation")
set(SQLITE_SOURCE_ARCHIVE "sqlite-${SQLITE_SOURCE_TYPE}-${SQLITE_VERSION}.zip")
set(SQLITE_RELEASE_YEAR   "2013")
set(SQLITE_URL            "http://www.sqlite.org/${SQLITE_RELEASE_YEAR}/${SQLITE_SOURCE_ARCHIVE}")
set(SQLITE_CMAKE_FILENAME "sqlite_cmake.txt")

#initialize
project(${PROJECT_NAME})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

if(quick)
	set(skip_gtest ON)
	set(skip_sqlite ON)
endif(quick)

if(NOT test)
	set(skip_gtest ON)
	set(skip_sqlite ON)
endif(NOT test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -U__STRICT_ANSI__")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11")

if(MINGW)
	string(REPLACE ";" "" fixed_prefixes ${CMAKE_FIND_LIBRARY_PREFIXES})
else(MINGW)
	set(fixed_prefixes ${CMAKE_FIND_LIBRARY_PREFIXES})
endif(MINGW)

if(qt)
	find_package(Qt5Core REQUIRED)
endif(qt)

# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/${THIRDPARTY})
#----------------------------------------------------------------------------------------------------
#>End


#>Build Google Test (External Project)
#----------------------------------------------------------------------------------------------------
set(gtest_source_dir ${CMAKE_BINARY_DIR}/${THIRDPARTY}/${SRC}/${GOOGLETEST})
set(gtest_binary_dir ${CMAKE_BINARY_DIR}/${THIRDPARTY}/${SRC}/${GOOGLETEST}${TH_BINARY_PREFIX})

if(skip_gtest)# Skip download and build google test

	#Nothing to do now.

else(skip_gtest)# Download and build google test

	set(gtest_patch ${CMAKE_SOURCE_DIR}/${GTEST_PATCH_FILENAME})
	if(MSVC_VERSION EQUAL 1700) # Create patch file for gtest with MSVC 2012
		file(WRITE  ${gtest_patch} "Index: cmake/internal_utils.cmake\n")
		file(APPEND ${gtest_patch} "===================================================================\n")
        file(APPEND ${gtest_patch} "--- cmake/internal_utils.cmake   (revision ${GTEST_REV})\n")
		file(APPEND ${gtest_patch} "+++ cmake/internal_utils.cmake   (working copy)\n")
		file(APPEND ${gtest_patch} "@@ -66,6 +66,9 @@\n")
		file(APPEND ${gtest_patch} "       # Resolved overload was found by argument-dependent lookup.\n")
		file(APPEND ${gtest_patch} "       set(cxx_base_flags \"\${cxx_base_flags} -wd4675\")\n")
		file(APPEND ${gtest_patch} "     endif()\n")
		file(APPEND ${gtest_patch} "+    if (MSVC_VERSION EQUAL 1700)\n")
		file(APPEND ${gtest_patch} "+      set(cxx_base_flags \"\${cxx_base_flags} -D_VARIADIC_MAX=10\")\n")
		file(APPEND ${gtest_patch} "+    endif ()\n")
		file(APPEND ${gtest_patch} "     set(cxx_base_flags \"\${cxx_base_flags} -D_UNICODE -DUNICODE -DWIN32 -D_WIN32\")\n")
		file(APPEND ${gtest_patch} "     set(cxx_base_flags \"\${cxx_base_flags} -DSTRICT -DWIN32_LEAN_AND_MEAN\")\n")
		file(APPEND ${gtest_patch} "     set(cxx_exception_flags \"-EHsc -D_HAS_EXCEPTIONS=1\")\n")
	else(MSVC_VERSION EQUAL 1700) # Create empty patch file
		file(WRITE ${gtest_patch} "")
	endif(MSVC_VERSION EQUAL 1700)

	ExternalProject_Add(
		${GOOGLETEST}
		SVN_REPOSITORY  ${GTEST_REPO}
		SVN_REVISION    -r ${GTEST_REV}
		TIMEOUT         10
		PATCH_COMMAND   svn patch ${gtest_patch} ${gtest_source_dir}
		CMAKE_ARGS      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}                       # Force separate output paths for debug and release builds
		                -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=${TH_DEBUG}
		                -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=${TH_RELEASE}
		                -Dgtest_force_shared_crt=ON
		INSTALL_COMMAND "" # Disable install step
		LOG_DOWNLOAD    ON # Wrap download, configure and build steps in a script to log output
		LOG_CONFIGURE   ON
		LOG_BUILD       ON
	)

	#Specify include dir
	ExternalProject_Get_Property(${GOOGLETEST} source_dir)
	ExternalProject_Get_Property(${GOOGLETEST} binary_dir)
	set(gtest_source_dir ${source_dir})
	set(gtest_binary_dir ${binary_dir})
endif(skip_gtest)
#----------------------------------------------------------------------------------------------------
#>End


#>Build SQLite (External Project)
#----------------------------------------------------------------------------------------------------
set(sqlite_source_dir ${CMAKE_BINARY_DIR}/${THIRDPARTY}/${SRC}/${SQLITE})
set(sqlite_binary_dir ${CMAKE_BINARY_DIR}/${THIRDPARTY}/${SRC}/${SQLITE}${TH_BINARY_PREFIX})

if(skip_sqlite)

	#Nothing to do now.

else(skip_sqlite)

	set(SQLITE_SOURCE ${SQLITE_URL})

	if(MINGW)
		set(sqlite_cmake ${sqlite_source_dir}/${CMAKELISTS_TXT})
	else(MINGW)
		set(sqlite_cmake ${CMAKE_SOURCE_DIR}/${SQLITE_CMAKE_FILENAME})
	endif(MINGW)

	file(WRITE  ${sqlite_cmake} "cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)\n")
	file(APPEND ${sqlite_cmake} "set(PROJECT_NAME "${SQLITE}")\n")
	file(APPEND ${sqlite_cmake} "option(SQLITE_BUILD_STATIC \"Build SQLite static library\" ON)\n")
	file(APPEND ${sqlite_cmake} "option(SQLITE_BUILD_SHARED \"Build SQLite shared library\" ON)\n")
	file(APPEND ${sqlite_cmake} "option(SQLITE_BUILD_SHELL  \"Build SQLite shell\" ON)\n")
	file(APPEND ${sqlite_cmake} "\n")
	file(APPEND ${sqlite_cmake} "set(SQLITE_WIN_BUILD OFF)\n")
	file(APPEND ${sqlite_cmake} "\n")
	file(APPEND ${sqlite_cmake} "if(\${CMAKE_SYSTEM_NAME} MATCHES \"Windows\")\n")
	file(APPEND ${sqlite_cmake} "	set(SQLITE_WIN_BUILD ON)\n")
	file(APPEND ${sqlite_cmake} "endif()\n")
	file(APPEND ${sqlite_cmake} "\n")
	file(APPEND ${sqlite_cmake} "set(SQLITE_DYN_NAME \${PROJECT_NAME}"${SQLITE_MAJOR_VERSION}")\n")
	file(APPEND ${sqlite_cmake} "set(SQLITE_STATIC_NAME \"\${SQLITE_DYN_NAME}-static\")\n")
	file(APPEND ${sqlite_cmake} "\n")
	file(APPEND ${sqlite_cmake} "include_directories(\${PROJECT_SOURCE_DIR})\n")
	file(APPEND ${sqlite_cmake} "\n")
	file(APPEND ${sqlite_cmake} "set(SRC_LIB_FILE \${PROJECT_SOURCE_DIR}/sqlite"${SQLITE_MAJOR_VERSION}".c)\n")
	file(APPEND ${sqlite_cmake} "set(SRC_SHELL_FILE \${PROJECT_SOURCE_DIR}/shell.c)\n")
	file(APPEND ${sqlite_cmake} "\n")
	file(APPEND ${sqlite_cmake} "if (SQLITE_BUILD_STATIC)\n")
	file(APPEND ${sqlite_cmake} "    add_library(\${SQLITE_STATIC_NAME} STATIC \${SRC_LIB_FILE})\n")
	file(APPEND ${sqlite_cmake} "endif()\n")
	file(APPEND ${sqlite_cmake} "\n")
	file(APPEND ${sqlite_cmake} "if (SQLITE_BUILD_SHARED)\n")
	file(APPEND ${sqlite_cmake} "    add_library(\${SQLITE_DYN_NAME} SHARED \${SRC_LIB_FILE})\n")
	file(APPEND ${sqlite_cmake} "    if (SQLITE_WIN_BUILD)\n")
	file(APPEND ${sqlite_cmake} "        set_target_properties(\${SQLITE_DYN_NAME} PROPERTIES DEFINE_SYMBOL \"SQLITE_API=__declspec(dllexport)\")\n")
	file(APPEND ${sqlite_cmake} "    endif()\n")
	file(APPEND ${sqlite_cmake} "endif()\n")
	file(APPEND ${sqlite_cmake} "\n")
	file(APPEND ${sqlite_cmake} "if(SQLITE_BUILD_SHELL)\n")
	file(APPEND ${sqlite_cmake} "	add_executable(shell \${SRC_SHELL_FILE})\n")
	file(APPEND ${sqlite_cmake} "	\n")
	file(APPEND ${sqlite_cmake} "	if (SQLITE_BUILD_SHARED)\n")
	file(APPEND ${sqlite_cmake} "	    target_link_libraries(shell \${SQLITE_DYN_NAME})\n")
	file(APPEND ${sqlite_cmake} "	elseif(SQLITE_BUILD_STATIC)\n")
	file(APPEND ${sqlite_cmake} "	    target_link_libraries(shell \${SQLITE_STATIC_NAME})\n")
	file(APPEND ${sqlite_cmake} "	else()\n")
	file(APPEND ${sqlite_cmake} "	    message(FATAL_ERROR \"either static or dynamic option should be selected\")\n")
	file(APPEND ${sqlite_cmake} "	endif()\n")
	file(APPEND ${sqlite_cmake} "endif(SQLITE_BUILD_SHELL)\n")

	if(MINGW)
	    ExternalProject_Add (
			sqlite
			URL ${SQLITE_SOURCE}
			CMAKE_ARGS	-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
						-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
						-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		                -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=${TH_DEBUG}
		                -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=${TH_RELEASE}
		                -DSQLITE_BUILD_SHARED=OFF
	        PATCH_COMMAND ""
			INSTALL_COMMAND "" # Disable install step
			LOG_DOWNLOAD    ON # Wrap download, configure and build steps in a script to log output
			LOG_CONFIGURE   ON
			LOG_BUILD       ON
		)
	else(MINGW)
		ExternalProject_Add (
			sqlite
			URL ${SQLITE_SOURCE}
			CMAKE_ARGS	-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
						-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
						-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		                -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=${TH_DEBUG}
		                -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=${TH_RELEASE}
		                -DSQLITE_BUILD_SHARED=OFF
	        PATCH_COMMAND cp ${sqlite_cmake} ${sqlite_source_dir}/${CMAKELISTS_TXT}
			INSTALL_COMMAND "" # Disable install step
			LOG_DOWNLOAD    ON # Wrap download, configure and build steps in a script to log output
			LOG_CONFIGURE   ON
			LOG_BUILD       ON
		)
	endif(MINGW)

	ExternalProject_Get_Property(sqlite source_dir)
	ExternalProject_Get_Property(sqlite binary_dir)
	set(sqlite_source_dir ${source_dir})
	set(sqlite_binary_dir ${binary_dir})

endif(skip_sqlite)
#----------------------------------------------------------------------------------------------------
#>End


#>Build test
#----------------------------------------------------------------------------------------------------
if(test)
	include_directories(${gtest_source_dir}/${INCLUDE})
	include_directories(${sqlite_source_dir})

	# Build Test Main
	list(APPEND tests_src ${PROJECT_SOURCE_DIR}/${SRC}/gtest_main.cpp)
	list(APPEND tests_src ${PROJECT_SOURCE_DIR}/${SRC}/fuzzysynony.cpp)
	
	add_executable(RunAllTests ${tests_src})
	add_dependencies(RunAllTests googletest sqlite)

	# Specify MainTest's link libraries
	if(MSVC)
		set(suffix ".lib")
	else(MSVC)
		set(suffix ".a")
		set(pthread "-pthread")
    endif(MSVC)

	target_link_libraries(
		RunAllTests
        debug "${gtest_binary_dir}/${TH_DEBUG}/${CMAKE_FIND_LIBRARY_PREFIXES}${GTEST_LIB_NAME}${suffix}"
        optimized ${gtest_binary_dir}/${TH_RELEASE}/${CMAKE_FIND_LIBRARY_PREFIXES}${GTEST_LIB_NAME}${suffix}
		${pthread}
	)
	target_link_libraries(
		RunAllTests
        debug ${sqlite_binary_dir}/${TH_DEBUG}/${CMAKE_FIND_LIBRARY_PREFIXES}${SQLITE_LIB_NAME}${suffix}
        optimized ${sqlite_binary_dir}/${TH_RELEASE}/${CMAKE_FIND_LIBRARY_PREFIXES}${SQLITE_LIB_NAME}${suffix}
		${pthread}
	)
endif(test)
#----------------------------------------------------------------------------------------------------
#>End


#>Build main
#----------------------------------------------------------------------------------------------------
if(main)

	include_directories(${sqlite_source_dir})

	# Build Test Main
	list(APPEND main_src ${PROJECT_SOURCE_DIR}/${SRC}/main.cpp)
	list(APPEND main_src ${PROJECT_SOURCE_DIR}/${SRC}/fuzzysynony.cpp)
	
	add_executable(${PROJECT_NAME} ${main_src})

        add_dependencies(${PROJECT_NAME} sqlite)

        target_link_libraries(
		${PROJECT_NAME}
        debug ${sqlite_binary_dir}/${TH_DEBUG}/${fixed_prefixes}${SQLITE_LIB_NAME}${suffix}
		optimized ${sqlite_binary_dir}/${TH_RELEASE}/${fixed_prefixes}${SQLITE_LIB_NAME}${suffix}
		${pthread}
	)

endif(main)
#----------------------------------------------------------------------------------------------------
#>End

