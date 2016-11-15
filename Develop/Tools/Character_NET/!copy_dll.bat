IF "%1" == "" goto :END
copy %1.dll ..\..\Game\Runtime\Dev\Character_NET
copy %1.pdb ..\..\Game\Runtime\Dev\Character_NET
:END