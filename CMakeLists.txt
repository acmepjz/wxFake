Project (wxTest)
CMake_Minimum_Required (VERSION 2.6)

#Find the required libraries.
Find_Package (wxWidgets COMPONENTS core base REQUIRED)

#Add the include directories of the (found) libraries.
Include_Directories(
	${wxWidgets_INCLUDE_DIRS}
)

#List the source files.
File (SOURCES 1.cpp)
File (GLOB WXFAKE_SOURCES wxFake/*.cpp)

Add_Executable (meandmyshadow ${SOURCES} ${WXFAKE_SOURCES})

Target_Link_Libraries (
	wxTest
	${wxWidgets_LIBRARIES}
)
