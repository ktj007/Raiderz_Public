macroScript ClothViewer category:"MAIET"
(

on Execute do
(
	Local strPathPhysXSimpleCloth = "C:/WORK/SoulHunt/Game/Runtime/Samples/PhysXSimpleCloth.exe" 
	
	Local strExportFileName = (GetDir #export + "/" + getFilenameFile maxFileName + ".elu" )
	
	exportFile strExportFileName
	
	-- 응용 프로그램 실행
	shellLaunch strPathPhysXSimpleCloth strExportFileName
	
)

)