set path_temp=%path%
set path=%path%;..\..\..\Bin\x64\Release
javac -d . ..\LibMWCapture\src\com\magewell\libmwcapture\*.java ..\AVCapture2Java\src\com\magewell\avcapture2\*.java -Xlint:deprecation
java com.magewell.avcapture2.AVCapture2
set path=%path_temp%
set path_temp=