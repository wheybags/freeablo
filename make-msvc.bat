set path=%PATH%;c:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE
devenv "StormLib_v09.sln" /build "Debug|Win32" /project "StormLib_test.vcproj" /projectconfig "Debug|Win32"
