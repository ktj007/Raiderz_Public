IF "%1" == "" goto :END
copy %1.dll ..\..\Runtime
copy %1.pdb ..\..\Runtime
:END