set path_temp=%path%
set path=%path%;..\..\..\Bin\x64\Release
javac -d . ..\LibMWCapture\src\com\magewell\libmwcapture\*.java ..\AVCaptureJava\src\com\magewell\avcapture\*.java -Xlint:deprecation
java com.magewell.avcapture.AVCapture
set path=%path_temp%
set path_temp=