IF "%1" == "" goto :END
copy %1.dll ..\..\RealSpace3\Runtime\Tools\EffectTool_NET
copy %1.pdb ..\..\RealSpace3\Runtime\Tools\EffectTool_NET
:END