cmake_minimum_required(VERSION 2.8.4)

#>References
#----------------------------------------------------------------------------------------------------
# http://wagavulin.hatenablog.com/entry/2011/11/27/222642
# Cmake samples

# http://stackoverflow.com/questions/9689183/cmake-googletest
# To integrate cmake and google test

# https://github.com/lbaehren/CMakePorts/blob/master/ports/database/sqlite/CMakeLists.txt
# To integrate cmake and SQLite

# http://stackoverflow.com/questions/10522654/ruby-and-swig-with-cmake
# To build ruby extension by using swig on cmake

# https://github.com/libyui/libyui-bindings
# sample

# http://www.swig.org/Doc1.3/Introduction.html#Introduction%5Fbuild%5Fsystem
# A sample of integratation swig and cmake for python
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
option(quick "Quick build without check external projects." OFF)
option(main  "Build main." OFF)
#----------------------------------------------------------------------------------------------------
#>End


#>Variavles
#----------------------------------------------------------------------------------------------------
set(PROJECT_NAME fuzzysynony)
set(SRC          src)
set(INCLUDE      include)

#used by installing thirdparty library
set(THIRDPARTY       ThirdParty)
set(TH_DEBUG         DebugLibs)
set(TH_RELEASE       ReleaseLibs)
set(TH_BINARY_PREFIX -build)

#googletest
set(GOOGLETEST googletest)
set(GTEST_REPO http://googletest.googlecode.com/svn/trunk/)
set(GTEST_REV  668)

#initialize
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/${THIRDPARTY})
#----------------------------------------------------------------------------------------------------
#>End


#>Build main
#----------------------------------------------------------------------------------------------------
if(main)
	project(${PROJECT_NAME})

	add_executable(${PROJECT_NAME} ${SRC}/main.cpp)
endif(main)
#----------------------------------------------------------------------------------------------------
#>End


#>Build test
#----------------------------------------------------------------------------------------------------
if(test)

	if(quick)# Skip download and build google test
		set(source_dir ${CMAKE_BINARY_DIR}/${THIRDPARTY}/${SRC}/${GOOGLETEST})
		set(binary_dir ${CMAKE_BINARY_DIR}/${THIRDPARTY}/${SRC}/${GOOGLETEST}${TH_BINARY_PREFIX})

	else(quick)# Download and build google test
	
		if(MSVC_VERSION EQUAL 1700) # Create patch file for gtest with MSVC 2012
			file(WRITE gtest.patch "Index: cmake/internal_utils.cmake\n")
			file(APPEND gtest.patch "===================================================================\n")
			file(APPEND gtest.patch "--- cmake/internal_utils.cmake   (revision "${GTEST_REV}")\n")
			file(APPEND gtest.patch "+++ cmake/internal_utils.cmake   (working copy)\n")
			file(APPEND gtest.patch "@@ -66,6 +66,9 @@\n")
			file(APPEND gtest.patch "       # Resolved overload was found by argument-dependent lookup.\n")
			file(APPEND gtest.patch "       set(cxx_base_flags \"\${cxx_base_flags} -wd4675\")\n")
			file(APPEND gtest.patch "     endif()\n")
			file(APPEND gtest.patch "+    if (MSVC_VERSION EQUAL 1700)\n")
			file(APPEND gtest.patch "+      set(cxx_base_flags \"\${cxx_base_flags} -D_VARIADIC_MAX=10\")\n")
			file(APPEND gtest.patch "+    endif ()\n")
			file(APPEND gtest.patch "     set(cxx_base_flags \"\${cxx_base_flags} -D_UNICODE -DUNICODE -DWIN32 -D_WIN32\")\n")
			file(APPEND gtest.patch "     set(cxx_base_flags \"\${cxx_base_flags} -DSTRICT -DWIN32_LEAN_AND_MEAN\")\n")
			file(APPEND gtest.patch "     set(cxx_exception_flags \"-EHsc -D_HAS_EXCEPTIONS=1\")\n")
		else(MSVC_VERSION EQUAL 1700) # Create empty patch file
			file(WRITE gtest.patch "")
		endif(MSVC_VERSION EQUAL 1700)

		ExternalProject_Add(
			${GOOGLETEST}
			SVN_REPOSITORY  ${GTEST_REPO}
			SVN_REVISION    -r ${GTEST_REV}
			TIMEOUT         10
			PATCH_COMMAND   svn patch ${CMAKE_SOURCE_DIR}/gtest.patch ${CMAKE_BINARY_DIR}/${THIRDPARTY}/${SRC}/${GOOGLETEST}
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
	endif(quick)

	include_directories(${source_dir}/${INCLUDE})

	# Build Test Main
	add_executable(RunAllTests ${PROJECT_SOURCE_DIR}/${SRC}/run_all_tests.cpp)
	add_dependencies(RunAllTests googletest)
	
	# Specify MainTest's link libraries
	if(MSVC)
		set(suffix ".lib")
	else(MSVC)
		set(suffix ".a")
		set(pthread "-pthread")
	endif(MSVC)

	target_link_libraries(
		RunAllTests
		debug ${binary_dir}/${TH_DEBUG}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${suffix}
		optimized ${binary_dir}/${TH_RELEASE}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${suffix}
		${pthread}
	)

endif(test)
#----------------------------------------------------------------------------------------------------
#>End
