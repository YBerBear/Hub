echo OFF

SET INCLUDE=%INCLUDE%;C:\Program Files\Microsoft SDKs\Windows\v7.0A\Include

CD C:\Program Files\Microsoft Visual Studio 10.0\VC

REM echo %1, %$1%

set extension=%1
REM echo %extension%

set source=c:\Eugene\cpp\IlMessaggero_%extension%.cpp

set executable=c:\eugene\cpp\Ilmessaggero_%extension%.exe

REM echo %source%, %executable%

echo ON

cl "%source%" /Fe"%executable%"

cd c:\eugene\cpp