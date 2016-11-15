@echo off
IF EXIST PerformanceCheck.csv del PerformanceCheck.csv
IF EXIST PerformanceCheck.PIXRun del PerformanceCheck.PIXRun
IF EXIST PerformanceCheck.etl del PerformanceCheck.etl
IF EXIST ..\RS3Benchmark.exe.index del ..\RS3Benchmark.exe.index
..\PIXWin.exe PerformanceCheck.PIXExp -targetpath ..\RS3Benchmark.exe -targetstartfolder .\ -targetargs PerformanceCheck.config.xml -runfile Catacomb/PerformanceCheck.PIXRun -start
IF EXIST PerformanceCheck.PIXRun ..\PIXWin.exe PerformanceCheck.PIXRun -exporttocsv PerformanceCheck.csv
IF EXIST PerformanceCheck.csv ..\Ruby\bin\ruby.exe PerformanceCheck.rb