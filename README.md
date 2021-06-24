# Medical-Image-Tool
Using ITK, VTK, Qt  to browse medical images and other functions.
The source code has been compiled with Qt 4.8.5, ITK 4.9, VTK 6.3, Visual Studio 2010, CMake 2.12 and X64 Window7.

**CMake settings for compiling VTK**  
Note that compiler compilation cannot be done by default compilation when compiling VTK.
You need to set the following settings

set(VTK_QT_VERSION 4 CACHE STRING "Qt version")  
set(VTK_NO_LIBRARY_VERSION ON CACHE BOOL "VTK_NO_LIBRARY_VERSION")  
set(VTK_Group_Qt ON CACHE BOOL "VTK_Group_Qt")  
set(Module_vtkGUISupportQt ON CACHE BOOL "Module_vtkGUISupportQt")  
set(Module_vtkGUISupportQtOpenGL ON CACHE BOOL "Module_vtkGUISupportQtOpenGL")  
set(Module_vtkRenderingQt ON CACHE BOOL "Module_vtkRenderingQt")  
set(BUILD_TESTING OFF CACHE BOOL "BUILD_TESTING")  
set(VTK_RENDERING_BACKEND_DEFAULT "OpenGL")
# Result 1
![image](https://github.com/wangdemon/Medical-Image-Tool/blob/master/Result/pic1.png)
# Result 2
![image](https://github.com/wangdemon/Medical-Image-Tool/blob/master/Result/pic2.png)
# Result 3
![image](https://github.com/wangdemon/Medical-Image-Tool/blob/master/Result/pic3.png)
