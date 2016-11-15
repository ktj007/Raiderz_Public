IF "%1" == "" goto :END
copy %1.dll ..\Runtime\Dev\CoreToolRS3
copy %1.pdb ..\Runtime\Dev\CoreToolRS3
:END