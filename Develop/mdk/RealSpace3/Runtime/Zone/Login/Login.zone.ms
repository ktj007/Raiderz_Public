global strCurrentFilePath = getFilenamePath ( getSourceFileName() )
fileIn ( strCurrentFilePath + "../../../Dev/MAIETExporters/zone.ms" )

ImportTerrain "D:/Project8/Develop/Game/Runtime/Data/ZoneTest/Login/Login_dis.dds" "D:/Project8/Develop/Game/Runtime/Data/ZoneTest/Login/_baked.bmp" 20000.000 20000.000

maxfile = getFilesRecursive pathXRefSearch "login_elu.max"
bSuccess = XRefMAXFile maxfile
if(bSuccess) then (
	newNodes = selection
	tm = (matrix3 [1.000,0.000,0.000] [0.000,1.000,0.000] [0.000,0.000,1.000] [0.000,0.000,0.000] 	)
	for t in newNodes do (
		if(t.parent == undefined) then (t.transform = t.transform * tm) 
	)
)
c = TargetDirectionallight pos:[0,0,0] target:(targetObject pos:[0.485,0.485,-0.728])
c.overShoot = true


