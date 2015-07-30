# Include required CMake packages
include(ExternalProject)

set(BOOST_BINARY_DIR ${CMAKE_BINARY_DIR}/Dependencies/boost)

# Build Boost
ExternalProject_Add(
    boost
    URL http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.7z
    URL_HASH SHA1=d9abe6c6b1888dc33cbc872437a11bcbe1468183
    PREFIX ${BOOST_BINARY_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(boost source_dir)
include_directories("${source_dir}")
link_directories("${BOOST_BINARY_DIR}/src/boost/stage/lib")

add_custom_target(boost-bootstrap
   COMMAND bootstrap
   WORKING_DIRECTORY = ${source_dir}
)

add_custom_target(boost-b2
   COMMAND b2.exe --build-dir=..
   WORKING_DIRECTORY = ${source_dir}
)

add_dependencies(boost-bootstrap boost)
add_dependencies(boost-b2 boost-bootstrap)

# Get sources
file(GLOB SRC *.cpp)
file(GLOB CK2World CK2World/*.cpp)
file(GLOB EU3World EU3World/*.cpp)
file(GLOB ModWorld ModWorld/*.cpp)
file(GLOB Parsers Parsers/*.cpp)

# Group sources
source_group("Source Files" FILES ${SRC})
source_group("Source Files\\CK2World" FILES ${CK2World})
source_group("Source Files\\EU3World" FILES ${EU3World})
source_group("Source Files\\ModWorld" FILES ${ModWorld})
source_group("Source Files\\Parsers" FILES ${Parsers})

# Make executable
add_executable(
	CK2ToEU3
	${SRC}
	${CK2World}
	${EU3World}
	${ModWorld}
	${Parsers})

add_custom_command(TARGET CK2ToEU3
	POST_BUILD
	COMMAND Copy_Files ${CMAKE_CURRENT_BINARY_DIR}/Release
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# Add "build to archive" target
add_custom_target(build-to-archive)
add_custom_command(TARGET build-to-archive
	POST_BUILD
	COMMAND build_to_archive ${CMAKE_CURRENT_BINARY_DIR}/Release
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# Add integration testing target
add_custom_target(test-all-configs)
add_custom_command(TARGET test-all-configs
	PRE_BUILD
	COMMAND test_all_configs ${CMAKE_CURRENT_BINARY_DIR}/Release
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})