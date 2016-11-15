global strCurrentFilePath = getFilenamePath ( getSourceFileName() )
fileIn ( strCurrentFilePath + "../../../Dev/MAIETExporters/zone.ms" )

/*
*/

-- 지형 임포트 -----------------------------------------------------------
ImportTerrain "C:/Works/SoulHunt/Game/Runtime/TestRS3/Data/ZoneTest/Login/Login_dis.dds" "C:/Works/SoulHunt/Game/Runtime/TestRS3/Data/ZoneTest/Login/_baked.bmp" 20000.000 20000.000

-- Actor 임포트 ----------------------------------------------------------

-- SpeedTree 임포트 ------------------------------------------------------
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_018" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [26.705,0.000,0.000] [0.000,26.705,0.000] [0.000,0.000,26.705] [15309.440,6192.732,128.160] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_025" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [26.705,0.000,0.000] [0.000,26.705,0.000] [0.000,0.000,26.705] [12827.507,8258.992,471.901] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_026" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [26.705,0.000,0.000] [0.000,26.705,0.000] [0.000,0.000,26.705] [13367.972,10983.031,12.634] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_027" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [33.000,0.000,0.000] [0.000,33.000,0.000] [0.000,0.000,33.000] [9055.093,8383.367,1771.612] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_028" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [25.380,0.000,0.000] [0.000,25.380,0.000] [0.000,0.000,25.380] [15892.903,9298.266,26.036] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_007" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [23.872,8.689,0.000] [-8.689,23.872,0.000] [0.000,0.000,25.404] [15303.417,6558.408,96.747] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_014" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [-14.107,18.721,0.000] [-18.721,-14.107,0.000] [0.000,0.000,23.441] [16789.703,8737.680,43.030] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_015" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [-14.107,18.721,0.000] [-18.721,-14.107,0.000] [0.000,0.000,23.441] [16847.781,9585.297,53.164] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_016" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [-14.107,18.721,0.000] [-18.721,-14.107,0.000] [0.000,0.000,23.441] [14627.116,9213.898,-14.093] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_017" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [-23.313,-2.450,0.000] [2.450,-23.313,0.000] [0.000,0.000,23.441] [13118.354,7842.217,444.299] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_018" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [-23.313,-2.450,0.000] [2.450,-23.313,0.000] [0.000,0.000,23.441] [11852.640,9939.914,487.464] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_019" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [-23.313,-2.450,0.000] [2.450,-23.313,0.000] [0.000,0.000,23.441] [10971.366,7350.088,438.795] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_020" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [14.107,-18.721,0.000] [18.721,14.107,0.000] [0.000,0.000,23.441] [10002.659,6432.967,472.250] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_021" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [14.107,-18.721,0.000] [18.721,14.107,0.000] [0.000,0.000,23.441] [12993.265,6849.354,23.556] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_022" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [23.427,0.818,0.000] [-0.818,23.427,0.000] [0.000,0.000,23.441] [13745.647,7160.713,-43.122] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_023" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [33.000,0.000,0.000] [0.000,33.000,0.000] [0.000,0.000,33.000] [8991.612,8911.180,1785.350] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_024" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [14.107,-18.721,0.000] [18.721,14.107,0.000] [0.000,0.000,23.441] [13178.562,8793.242,449.450] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_025" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [13.416,-17.803,0.000] [17.803,13.416,0.000] [0.000,0.000,22.292] [14538.417,8838.078,-67.732] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_026" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [22.220,-9.432,0.000] [9.432,22.220,0.000] [0.000,0.000,24.139] [15723.421,11290.670,29.441] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_027" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [7.058,23.085,0.000] [-23.085,7.058,0.000] [0.000,0.000,24.139] [14271.132,9652.289,26.915] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_AmericanBoxwood_RT_2.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_AmericanBoxwood_RT_2.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_AmericanBoxwood_RT_2_028" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [7.058,23.085,0.000] [-23.085,7.058,0.000] [0.000,0.000,24.139] [13577.788,10484.156,-4.807] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_CommonOlive_RT.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_CommonOlive_RT.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_CommonOlive_RT_009" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [-16.122,-30.320,0.000] [30.320,-16.122,0.000] [0.000,0.000,34.340] [8191.550,10487.938,1785.349] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)
(
	local sptfile = getFilesRecursiveFirst pathSpeedTreeSearch "k_teress_CommonOlive_RT.spt"
	if sptfile == undefined or sptfile.count == 0 then (
		appendIfUnique arrNotExistSpeedFiles "k_teress_CommonOlive_RT.spt"
	) else (
		local treeInstance = SpeedTree_4 name:"k_teress_CommonOlive_RT_010" seed:0
		treeInstance.SetSptFile sptfile
		treeInstance.Show_wind_animation = false
		local tm = matrix3 [-2.395,-34.256,0.000] [34.256,-2.395,0.000] [0.000,0.000,34.340] [14410.472,13323.115,854.416] 		
		treeInstance.transform = tm
		SetUserProp treeInstance "set_export_ignore" true
		local mtlCount = GetNumSubMtls treeInstance.material
		for i = 1 to mtlCount do(
			treeInstance.material[i].twoSided = true
		)
	)
)

-- Directional light 임포트 ----------------------------------------------
c = TargetDirectionallight pos:[0,0,0] target:(targetObject pos:[0.485,0.485,-0.728])
c.overShoot = true

-- 에러 메시지 출력 ; 주석 처리 하지 마세요-------------------------------
displayNotExistSpeedFiles()


