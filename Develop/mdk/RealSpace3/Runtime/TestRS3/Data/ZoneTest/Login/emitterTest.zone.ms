global strCurrentFilePath = getFilenamePath ( getSourceFileName() )
fileIn ( strCurrentFilePath + "../../../Dev/MAIETExporters/zone.ms" )

/*
*/

-- 지형 임포트 -----------------------------------------------------------
ImportTerrain "E:/work/SoulHunt/Game/RunTime/TestRS3/Data/ZoneTest/Login/Login_dis.dds" "E:/work/SoulHunt/Game/RunTime/TestRS3/Data/ZoneTest/Login/_baked.bmp" 20000.000 20000.000

-- Actor 임포트 ----------------------------------------------------------

-- SpeedTree 임포트 ------------------------------------------------------
(
	local tm = matrix3 [26.705,0.000,0.000] [0.000,26.705,0.000] [0.000,0.000,26.705] [16840.451,3467.894,471.901] 
	importSpeedTree "k_teress_AmericanBoxwood_RT.spt" "k_teress_AmericanBoxwood_RT_025" tm
)
(
	local tm = matrix3 [26.705,0.000,0.000] [0.000,26.705,0.000] [0.000,0.000,26.705] [17380.916,6191.933,12.634] 
	importSpeedTree "k_teress_AmericanBoxwood_RT.spt" "k_teress_AmericanBoxwood_RT_026" tm
)
(
	local tm = matrix3 [33.000,0.000,0.000] [0.000,33.000,0.000] [0.000,0.000,33.000] [9055.093,8383.367,1771.612] 
	importSpeedTree "k_teress_AmericanBoxwood_RT.spt" "k_teress_AmericanBoxwood_RT_027" tm
)
(
	local tm = matrix3 [-14.107,18.721,0.000] [-18.721,-14.107,0.000] [0.000,0.000,23.441] [18640.061,4422.800,-14.093] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_016" tm
)
(
	local tm = matrix3 [-23.313,-2.450,0.000] [2.450,-23.313,0.000] [0.000,0.000,23.441] [17131.299,3051.119,444.299] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_017" tm
)
(
	local tm = matrix3 [-23.313,-2.450,0.000] [2.450,-23.313,0.000] [0.000,0.000,23.441] [15865.584,5148.816,487.464] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_018" tm
)
(
	local tm = matrix3 [-23.313,-2.450,0.000] [2.450,-23.313,0.000] [0.000,0.000,23.441] [14984.311,2558.990,438.795] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_019" tm
)
(
	local tm = matrix3 [14.107,-18.721,0.000] [18.721,14.107,0.000] [0.000,0.000,23.441] [10002.659,6432.967,472.250] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_020" tm
)
(
	local tm = matrix3 [33.000,0.000,0.000] [0.000,33.000,0.000] [0.000,0.000,33.000] [8991.612,8911.180,1785.350] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_023" tm
)
(
	local tm = matrix3 [14.107,-18.721,0.000] [18.721,14.107,0.000] [0.000,0.000,23.441] [17191.506,4002.144,449.450] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_024" tm
)
(
	local tm = matrix3 [13.416,-17.803,0.000] [17.803,13.416,0.000] [0.000,0.000,22.292] [18551.361,4046.980,-67.732] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_025" tm
)
(
	local tm = matrix3 [22.220,-9.432,0.000] [9.432,22.220,0.000] [0.000,0.000,24.139] [19736.365,6499.572,29.441] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_026" tm
)
(
	local tm = matrix3 [7.058,23.085,0.000] [-23.085,7.058,0.000] [0.000,0.000,24.139] [18284.076,4861.191,26.915] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_027" tm
)
(
	local tm = matrix3 [7.058,23.085,0.000] [-23.085,7.058,0.000] [0.000,0.000,24.139] [17590.732,5693.058,-4.807] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_028" tm
)
(
	local tm = matrix3 [-16.122,-30.320,0.000] [30.320,-16.122,0.000] [0.000,0.000,34.340] [8191.550,10487.938,1785.349] 
	importSpeedTree "k_teress_CommonOlive_RT.spt" "k_teress_CommonOlive_RT_009" tm
)
(
	local tm = matrix3 [-2.395,-34.256,0.000] [34.256,-2.395,0.000] [0.000,0.000,34.340] [14410.472,13323.115,854.416] 
	importSpeedTree "k_teress_CommonOlive_RT.spt" "k_teress_CommonOlive_RT_010" tm
)

-- Directional light 임포트 ----------------------------------------------
c = TargetDirectionallight pos:[0,0,0] target:(targetObject pos:[0.485,0.485,-0.728])
c.overShoot = true

-- 에러 메시지 출력 ; 주석 처리 하지 마세요-------------------------------
displayNotExistSpeedFiles()


