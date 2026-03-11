set path_temp=%path%
set path=%path%;..\..\..\Bin\x64\Release
javac -d . ..\LibMWCapture\src\com\magewell\libmwcapture\*.java ..\AVCaptureJava\src\com\magewell\avcapture\*.java -Xlint:deprecation
set path=%path_temp%

set dir_dir=distribute
set dir_folder=AVCapture
set dir_file=file
set dir_com=com

set path_dir=%dir_dir%
if exist %path_dir% (
    echo folder exists
    echo folder will be deleted
    rd /s %path_dir%
) else (
    echo create %path_dir%
    md %path_dir%
)
set path_dir_folder=%path_dir%\%dir_folder%
if exist %path_dir_folder% (
    echo folder exists
    echo folder will be deleted
    rd /s %path_dir_folder%
    md %path_dir_folder%
) else (
    echo create %path_dir_folder%
    md %path_dir_folder%
)

set path_dir_folder_file=%path_dir_folder%\%dir_file%
if exist %path_dir_folder_file% (
    echo folder exists
    echo folder will be deleted
    rd /s %path_dir_folder_file%
    md %path_dir_folder_file%
) else (
    echo create %path_dir_folder_file%
    md %path_dir_folder_file%
)

set path_dir_com=%path_dir_folder_file%\%dir_com%
if exist %path_dir_com% (
    echo folder exists
    echo folder will be deleted
    rd /s %path_dir_com%
    md %path_dir_com%
) else (
    echo create %path_dir_com%
    md %path_dir_com%
)

set path_com=com
if exist %path_com% (
    echo folder exists
    xcopy %path_com% %path_dir_com% /s
) else (
    echo com doesn't exist
    pause
    exit(-1)
)

set path_file_manifest=manifest\AVCapture\MANIFEST.MF
if exist %path_file_manifest% (
    echo file exists
    copy %path_file_manifest% %path_dir_folder%
) else (
    echo file not exists
)

set history_path=%cd%
cd %path_dir_folder%
jar cvfm AVCapture.jar MANIFEST.MF -C %dir_file%/ .

cd %history_path%
copy %path_dir_folder%\AVCapture.jar ..\..\..\Bin\x64\Release

cd ..\..\..\Bin\x64\Release
java -jar ..\..\..\Bin\x64\Release\AVCapture.jar
