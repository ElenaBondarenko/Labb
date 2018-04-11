@SET HOST=127.0.0.1
@SET PORT=5557
@SET FILE_SRC=in.txt

@start cmd /C NumberClientDemo.exe -h %HOST% -p %PORT% ^< %FILE_SRC%
@start cmd /C NumberClientDemo.exe -h %HOST% -p %PORT% ^< %FILE_SRC%
@pause