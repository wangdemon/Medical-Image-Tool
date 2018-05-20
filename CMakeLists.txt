cmake_minimum_required(VERSION 2.8.12)

project(MedicalImageTool)

# Find the Qt VTK ITK directory
set(ITK_DIR "D:/InstallApplication/ITK/ITK_bin" CACHE PATH "ITK directory")
set(VTK_DIR "D:/InstallApplication/VTK/VTK_bin" CACHE PATH "VTK directory")

find_package( ITK )
find_package( VTK REQUIRED )
# find_package( Qt4 REQUIRED )

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtOpenGL QtNetwork REQUIRED)

include( ${ITK_USE_FILE} )
include( ${VTK_USE_FILE} )
include( ${QT_USE_FILE} )

# Set Qt file directory of source code
set(Qt_HDR_files
	About.h
	Crosshair.h
	DistanceMeasurement.h
	VolumeRenderer.h
	ImageToSTL.h
	STLSet.h
	MedicalImageTool.h)
	
set(Qt_SRC_files
	main.cpp
	About.cpp
	Crosshair.cpp
	DistanceMeasurement.cpp
	VolumeRenderer.cpp
	ImageToSTL.cpp
	STLSet.cpp
	MedicalImageTool.cpp)

set(Qt_RES_files
	About.qrc
	Crosshair.qrc
	DistanceMeasurement.qrc
	VolumeRenderer.qrc
	ImageToSTL.qrc
	STLSet.qrc
	MedicalImageTool.qrc)
	
set(Qt_UI_files
	About.ui
	Crosshair.ui
	DistanceMeasurement.ui
	VolumeRenderer.ui
	ImageToSTL.ui
	STLSet.ui
	MedicalImageTool.ui)

qt4_wrap_ui(MOC_UI ${Qt_UI_files})
qt4_wrap_cpp(MOC_SRC ${Qt_HDR_files})
qt4_add_resources(MOC_RES ${Qt_RES_files})	

source_group("Form Files" FILES ${Qt_UI_files})
source_group("Generated Files" FILES ${MOC_UI} ${MOC_SRC})
source_group("Resources Files" FILES ${Qt_RES_files} ${MOC_RES})

# CTK------------------------------------------------------------------------------------------------------------
# CTK paths
set(CTK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CTK" CACHE PATH "CTK directory")
set(CTK_Libs_Core_DIR "${CTK_DIR}/Libs/Core")
set(CTK_Libs_Widgets_DIR "${CTK_DIR}/Libs/Widgets")
set(CTK_Libs_Visualization_VTK_Core_DIR "${CTK_DIR}/Libs/Visualization/VTK/Core")
set(CTK_Libs_Visualization_VTK_Widgets_DIR "${CTK_DIR}/Libs/Visualization/VTK/Widgets")
file(GLOB CTK_header_files
    ${CTK_Libs_Core_DIR}/*.h
    ${CTK_Libs_Widgets_DIR}/*.h
    ${CTK_Libs_Visualization_VTK_Core_DIR}/*.h
    ${CTK_Libs_Visualization_VTK_Widgets_DIR}/*.h)
	
file(GLOB CTK_source_files
    ${CTK_Libs_Core_DIR}/*.h
    ${CTK_Libs_Core_DIR}/*.cpp
    ${CTK_Libs_Widgets_DIR}/*.h
    ${CTK_Libs_Widgets_DIR}/*.cpp
    ${CTK_Libs_Visualization_VTK_Core_DIR}/*.h
    ${CTK_Libs_Visualization_VTK_Core_DIR}/*.cpp
    ${CTK_Libs_Visualization_VTK_Widgets_DIR}/*.h
    ${CTK_Libs_Visualization_VTK_Widgets_DIR}/*.cpp)
	
file(GLOB CTK_qrc_files
    ${CTK_Libs_Widgets_DIR}/Resources/*.qrc
    ${CTK_Libs_Visualization_VTK_Widgets_DIR}/Resources/*.qrc)
	
file(GLOB CTK_ui_files
    ${CTK_Libs_Widgets_DIR}/Resources/UI/*.ui
    ${CTK_Libs_Visualization_VTK_Widgets_DIR}/Resources/UI/*.ui)

# CTK
source_group(CTK FILES ${CTK_header_files} ${CTK_source_files})
message(STATUS "CTK header files: " ${CTK_header_files})
message(STATUS "CTK source files: " ${CTK_source_files})
QT4_WRAP_CPP(CTK_moc_source_files ${CTK_header_files})
message(STATUS "CTK moc source files: " ${CTK_moc_source_files})
source_group(CTK_moc FILES ${CTK_moc_source_files})
QT4_WRAP_UI(CTK_ui_header_files ${CTK_ui_files})
message(STATUS "CTK ui header files: " ${CTK_ui_header_files})
QT4_ADD_RESOURCES(CTK_qrc_complied ${CTK_qrc_files})
source_group(CTK_ui FILES ${CTK_ui_header_files} ${CTK_ui_files} ${CTK_qrc_files} ${CTK_qrc_complied})

# Slicer
set(Slicer_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Slicer/Modules/Loadable/VolumeRendering/VolumeRenderingReplacements)
file(GLOB Slicer_files
    ${Slicer_DIR}/*.h
    ${Slicer_DIR}/*.cxx)
	
source_group(Slicer FILES ${Slicer_files})
	
# Set ITK file directory of source code
set(ITK_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ITK" CACHE PATH "ITK SRC directory")
file(GLOB ITK_files
    ${ITK_SRC_DIR}/*.h
	${ITK_SRC_DIR}/*.hxx)
	
# Set VTK file directory of source code
set(VTK_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/VTK" CACHE PATH "ITK SRC directory")
file(GLOB VTK_files
    ${VTK_SRC_DIR}/*.h
	${VTK_SRC_DIR}/*.cpp
	${VTK_SRC_DIR}/*.cxx)


# Add the include directories for VTK and Qt 4 Widgets module to the compile lines.
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} 
					${PROJECT_SOURCE_DIR}
					${VTK_DIR} ${ITK_DIR}
					${ITK_SRC_DIR} ${VTK_SRC_DIR}
					${CTK_Libs_Core_DIR} ${CTK_Libs_Widgets_DIR} ${CTK_Libs_Visualization_VTK_Core_DIR} ${CTK_Libs_Visualization_VTK_Widgets_DIR}
					${Slicer_DIR})
				   
add_executable(${PROJECT_NAME} 
			   ${Qt_HDR_files} ${Qt_SRC_files} ${Qt_UI_files} ${Qt_QRC_files}
			   ${MOC_UI} ${MOC_SRC} ${MOC_RES} 
			   ${ITK_files} ${VTK_files}
			   ${CTK_header_files} ${CTK_source_files} ${CTK_moc_source_files} ${CTK_ui_header_files} ${CTK_qrc_complied}
			   ${Slicer_files})

target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES} ${ITK_LIBRARIES})		   

			   
				   
