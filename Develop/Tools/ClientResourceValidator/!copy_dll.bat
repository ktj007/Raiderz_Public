IF "%1" == "" goto :END
copy %1.dll ..\..\Game\Runtime\Dev\ResourceValidator\ClientResourceValidator
copy %1.pdb ..\..\Game\Runtime\Dev\ResourceValidator\ClientResourceValidator
:END