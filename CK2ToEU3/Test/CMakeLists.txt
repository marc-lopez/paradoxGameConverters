# Get tests
file(GLOB TEST *.cpp)
file(GLOB CK2World CK2World/*.cpp)
file(GLOB EU3World EU3World/*.cpp)
file(GLOB ModWorld ModWorld/*.cpp)
file(GLOB Parsers Parsers/*.cpp)

# Group tests
source_group("Source Files" FILES ${TEST})
source_group("Source Files\\CK2World" FILES ${CK2World})
source_group("Source Files\\EU3World" FILES ${EU3World})
source_group("Source Files\\ModWorld" FILES ${ModWorld})
source_group("Source Files\\Parsers" FILES ${Parsers})

# Prepare test framework
if (MSVC11)
    get_filename_component(VS_DIR [HKEY_CURRENT_USER\\Software\\Microsoft\\VisualStudio\\11.0_Config\\Setup\\VS;ProductDir] REALPATH CACHE)
elseif (MSVC10)
    get_filename_component(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\10.0\\Setup\\VS;ProductDir] REALPATH CACHE)
elseif (MSVC90)
    get_filename_component(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0\\Setup\\VS;ProductDir] REALPATH CACHE)
elseif (MSVC80)
    get_filename_component(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0\\Setup\\VS;ProductDir] REALPATH CACHE)
endif()

message(STATUS "The Visual Studio install directory is: " ${VS_DIR})

include_directories("${VS_DIR}/VC/UnitTest/include")

# Add unit testing target
add_library(
	UnitTests
	SHARED
	${TEST}
	${CK2World}
	${EU3World}
	${ModWorld}
	${Parsers})

target_link_libraries(UnitTests "${VS_DIR}/VC/UnitTest/lib/Microsoft.VisualStudio.TestTools.CppUnitTestFramework.lib")