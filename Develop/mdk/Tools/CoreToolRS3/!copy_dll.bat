IF "%1" == "" goto :END
copy %1.dll ..\..\Game\Runtime\Dev\CoreToolRS3
copy %1.pdb ..\..\Game\Runtime\Dev\CoreToolRS3
:END