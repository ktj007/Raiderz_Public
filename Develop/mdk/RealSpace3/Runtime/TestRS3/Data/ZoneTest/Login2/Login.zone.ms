global strCurrentFilePath = getFilenamePath ( getSourceFileName() )
fileIn ( strCurrentFilePath + "../../../Dev/MAIETExporters/zone.ms" )

/*
*/

-- 지형 임포트 -----------------------------------------------------------
ImportTerrain "D:/!!프로젝트/Soul Hunt/Runtime/Data/Zone/Login/Login_dis.dds" "D:/!!프로젝트/Soul Hunt/Runtime/Data/Zone/Login/_baked.bmp" 20000.000 20000.000

-- Actor 임포트 ----------------------------------------------------------
maxfile = getFilesRecursive pathXRefSearch "article_butterfly_01_ani.max"
bSuccess = XRefMAXFile maxfile
if(bSuccess) then (
	maxOps.CloneNodes selection cloneType:#instance newNodes:&newNodes
	tm = (matrix3 [1.000,0.000,0.000] [0.000,1.000,0.000] [0.000,0.000,1.000] [16181.057,6450.818,28.087] 	)
	for t in newNodes do (
		if(t.parent == undefined) then (t.transform = t.transform * tm) 
	)
	newNodes = selection
	tm = (matrix3 [1.000,0.000,0.000] [0.000,1.000,0.000] [0.000,0.000,1.000] [16035.459,5970.285,106.600] 	)
	for t in newNodes do (
		if(t.parent == undefined) then (t.transform = t.transform * tm) 
	)
)
maxfile = getFilesRecursive pathXRefSearch "article_butterfly_02_ani.max"
bSuccess = XRefMAXFile maxfile
if(bSuccess) then (
	maxOps.CloneNodes selection cloneType:#instance newNodes:&newNodes
	tm = (matrix3 [1.000,0.000,0.000] [0.000,1.000,0.000] [0.000,0.000,1.000] [16588.256,7664.469,28.216] 	)
	for t in newNodes do (
		if(t.parent == undefined) then (t.transform = t.transform * tm) 
	)
	newNodes = selection
	tm = (matrix3 [1.000,0.000,0.000] [0.000,1.000,0.000] [0.000,0.000,1.000] [15272.290,8535.430,29.668] 	)
	for t in newNodes do (
		if(t.parent == undefined) then (t.transform = t.transform * tm) 
	)
)
maxfile = getFilesRecursive pathXRefSearch "article_butterfly_03_ani.max"
bSuccess = XRefMAXFile maxfile
if(bSuccess) then (
	maxOps.CloneNodes selection cloneType:#instance newNodes:&newNodes
	tm = (matrix3 [1.000,0.000,0.000] [0.000,1.000,0.000] [0.000,0.000,1.000] [16384.396,8518.188,30.920] 	)
	for t in newNodes do (
		if(t.parent == undefined) then (t.transform = t.transform * tm) 
	)
	maxOps.CloneNodes selection cloneType:#instance newNodes:&newNodes
	tm = (matrix3 [1.000,0.000,0.000] [0.000,1.000,0.000] [0.000,0.000,1.000] [15345.168,7666.252,17.739] 	)
	for t in newNodes do (
		if(t.parent == undefined) then (t.transform = t.transform * tm) 
	)
	newNodes = selection
	tm = (matrix3 [1.000,0.000,0.000] [0.000,1.000,0.000] [0.000,0.000,1.000] [15714.720,6243.567,109.660] 	)
	for t in newNodes do (
		if(t.parent == undefined) then (t.transform = t.transform * tm) 
	)
)
maxfile = getFilesRecursive pathXRefSearch "ingen_waterfall.max"
bSuccess = XRefMAXFile maxfile
if(bSuccess) then (
	newNodes = selection
	tm = (matrix3 [1.000,0.000,0.000] [0.000,1.000,0.000] [0.000,0.000,1.000] [11118.076,12336.573,-322.334] 	)
	for t in newNodes do (
		if(t.parent == undefined) then (t.transform = t.transform * tm) 
	)
)

-- SpeedTree 임포트 ------------------------------------------------------
(
	local tm = matrix3 [-8.475,33.990,0.000] [-33.990,-8.475,0.000] [0.000,0.000,35.030] [15219.179,6277.084,81.757] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_017" tm
)
(
	local tm = matrix3 [29.660,14.466,0.000] [-14.466,29.660,0.000] [0.000,0.000,33.000] [15316.412,6691.217,9.081] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_018" tm
)
(
	local tm = matrix3 [-25.502,12.438,0.000] [-12.438,-25.502,0.000] [0.000,0.000,28.373] [14881.526,8184.115,-18.238] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_023" tm
)
(
	local tm = matrix3 [15.903,31.212,0.000] [-31.212,15.903,0.000] [0.000,0.000,35.030] [14648.292,8840.445,-43.055] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_024" tm
)
(
	local tm = matrix3 [29.660,14.466,0.000] [-14.466,29.660,0.000] [0.000,0.000,33.000] [14745.526,9254.578,3.320] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_025" tm
)
(
	local tm = matrix3 [20.063,20.063,0.000] [-20.063,20.063,0.000] [0.000,0.000,28.373] [13029.374,7771.803,433.060] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_026" tm
)
(
	local tm = matrix3 [24.770,24.770,0.000] [-24.770,24.770,0.000] [0.000,0.000,35.030] [12875.206,8323.646,471.948] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_027" tm
)
(
	local tm = matrix3 [27.676,-17.973,0.000] [17.973,27.676,0.000] [0.000,0.000,33.000] [13251.870,8451.367,415.991] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_028" tm
)
(
	local tm = matrix3 [-34.387,6.684,0.000] [-6.684,-34.387,0.000] [0.000,0.000,35.030] [13255.136,11034.430,1.380] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_029" tm
)
(
	local tm = matrix3 [-27.852,5.414,0.000] [-5.414,-27.852,0.000] [0.000,0.000,28.373] [13626.425,11470.826,29.392] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_030" tm
)
(
	local tm = matrix3 [-0.576,32.995,0.000] [-32.995,-0.576,0.000] [0.000,0.000,33.000] [12916.644,11419.529,7.932] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_031" tm
)
(
	local tm = matrix3 [-34.387,6.684,0.000] [-6.684,-34.387,0.000] [0.000,0.000,35.030] [9837.190,6413.678,490.824] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_032" tm
)
(
	local tm = matrix3 [-27.025,5.253,0.000] [-5.253,-27.025,0.000] [0.000,0.000,27.531] [11101.339,7312.846,411.157] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_033" tm
)
(
	local tm = matrix3 [-0.576,32.995,0.000] [-32.995,-0.576,0.000] [0.000,0.000,33.000] [9502.940,6676.814,504.747] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_034" tm
)
(
	local tm = matrix3 [-6.843,-29.640,0.000] [29.640,-6.843,0.000] [0.000,0.000,30.420] [11448.675,7443.088,397.251] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_035" tm
)
(
	local tm = matrix3 [-6.864,27.530,0.000] [-27.530,-6.864,0.000] [0.000,0.000,28.373] [16911.477,8798.750,39.987] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_039" tm
)
(
	local tm = matrix3 [29.660,14.466,0.000] [-14.466,29.660,0.000] [0.000,0.000,33.000] [16671.004,9831.469,42.323] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_040" tm
)
(
	local tm = matrix3 [10.744,31.202,0.000] [-31.202,10.744,0.000] [0.000,0.000,33.000] [16742.875,8593.359,41.001] 
	importSpeedTree "k_teress_Agarwood_RT_2.spt" "k_teress_Agarwood_RT_2_041" tm
)
(
	local tm = matrix3 [26.705,0.000,0.000] [0.000,26.705,0.000] [0.000,0.000,26.705] [15309.440,6192.732,128.160] 
	importSpeedTree "k_teress_AmericanBoxwood_RT.spt" "k_teress_AmericanBoxwood_RT_018" tm
)
(
	local tm = matrix3 [26.705,0.000,0.000] [0.000,26.705,0.000] [0.000,0.000,26.705] [12827.507,8258.992,471.901] 
	importSpeedTree "k_teress_AmericanBoxwood_RT.spt" "k_teress_AmericanBoxwood_RT_025" tm
)
(
	local tm = matrix3 [26.705,0.000,0.000] [0.000,26.705,0.000] [0.000,0.000,26.705] [13367.972,10983.031,12.634] 
	importSpeedTree "k_teress_AmericanBoxwood_RT.spt" "k_teress_AmericanBoxwood_RT_026" tm
)
(
	local tm = matrix3 [33.000,0.000,0.000] [0.000,33.000,0.000] [0.000,0.000,33.000] [9055.093,8383.367,1771.612] 
	importSpeedTree "k_teress_AmericanBoxwood_RT.spt" "k_teress_AmericanBoxwood_RT_027" tm
)
(
	local tm = matrix3 [25.380,0.000,0.000] [0.000,25.380,0.000] [0.000,0.000,25.380] [15892.903,9298.266,26.036] 
	importSpeedTree "k_teress_AmericanBoxwood_RT.spt" "k_teress_AmericanBoxwood_RT_028" tm
)
(
	local tm = matrix3 [23.872,8.689,0.000] [-8.689,23.872,0.000] [0.000,0.000,25.404] [15303.417,6558.408,96.747] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_007" tm
)
(
	local tm = matrix3 [-14.107,18.721,0.000] [-18.721,-14.107,0.000] [0.000,0.000,23.441] [16789.703,8737.680,43.030] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_014" tm
)
(
	local tm = matrix3 [-14.107,18.721,0.000] [-18.721,-14.107,0.000] [0.000,0.000,23.441] [16847.781,9585.297,53.164] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_015" tm
)
(
	local tm = matrix3 [-14.107,18.721,0.000] [-18.721,-14.107,0.000] [0.000,0.000,23.441] [14627.116,9213.898,-14.093] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_016" tm
)
(
	local tm = matrix3 [-23.313,-2.450,0.000] [2.450,-23.313,0.000] [0.000,0.000,23.441] [13118.354,7842.217,444.299] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_017" tm
)
(
	local tm = matrix3 [-23.313,-2.450,0.000] [2.450,-23.313,0.000] [0.000,0.000,23.441] [11852.640,9939.914,487.464] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_018" tm
)
(
	local tm = matrix3 [-23.313,-2.450,0.000] [2.450,-23.313,0.000] [0.000,0.000,23.441] [10971.366,7350.088,438.795] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_019" tm
)
(
	local tm = matrix3 [14.107,-18.721,0.000] [18.721,14.107,0.000] [0.000,0.000,23.441] [10002.659,6432.967,472.250] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_020" tm
)
(
	local tm = matrix3 [14.107,-18.721,0.000] [18.721,14.107,0.000] [0.000,0.000,23.441] [12993.265,6849.354,23.556] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_021" tm
)
(
	local tm = matrix3 [23.427,0.818,0.000] [-0.818,23.427,0.000] [0.000,0.000,23.441] [13745.647,7160.713,-43.122] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_022" tm
)
(
	local tm = matrix3 [33.000,0.000,0.000] [0.000,33.000,0.000] [0.000,0.000,33.000] [8991.612,8911.180,1785.350] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_023" tm
)
(
	local tm = matrix3 [14.107,-18.721,0.000] [18.721,14.107,0.000] [0.000,0.000,23.441] [13178.562,8793.242,449.450] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_024" tm
)
(
	local tm = matrix3 [13.416,-17.803,0.000] [17.803,13.416,0.000] [0.000,0.000,22.292] [14538.417,8838.078,-67.732] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_025" tm
)
(
	local tm = matrix3 [22.220,-9.432,0.000] [9.432,22.220,0.000] [0.000,0.000,24.139] [15723.421,11290.670,29.441] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_026" tm
)
(
	local tm = matrix3 [7.058,23.085,0.000] [-23.085,7.058,0.000] [0.000,0.000,24.139] [14271.132,9652.289,26.915] 
	importSpeedTree "k_teress_AmericanBoxwood_RT_2.spt" "k_teress_AmericanBoxwood_RT_2_027" tm
)
(
	local tm = matrix3 [7.058,23.085,0.000] [-23.085,7.058,0.000] [0.000,0.000,24.139] [13577.788,10484.156,-4.807] 
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
(
	local tm = matrix3 [44.830,0.000,0.000] [0.000,44.830,0.000] [0.000,0.000,44.830] [6817.958,7220.830,1785.349] 
	importSpeedTree "k_teress_ElmStocky_RT.spt" "k_teress_ElmStocky_RT_006" tm
)
(
	local tm = matrix3 [33.000,0.000,0.000] [0.000,33.000,0.000] [0.000,0.000,33.000] [15831.691,11395.803,36.523] 
	importSpeedTree "k_teress_ElmStocky_RT.spt" "k_teress_ElmStocky_RT_007" tm
)
(
	local tm = matrix3 [-30.597,-12.362,0.000] [12.362,-30.597,0.000] [0.000,0.000,33.000] [9761.237,7543.775,514.697] 
	importSpeedTree "k_teress_ElmStocky_RT.spt" "k_teress_ElmStocky_RT_008" tm
)
(
	local tm = matrix3 [33.000,0.000,0.000] [0.000,33.000,0.000] [0.000,0.000,33.000] [12383.202,5432.280,71.894] 
	importSpeedTree "k_teress_ElmStocky_RT.spt" "k_teress_ElmStocky_RT_009" tm
)
(
	local tm = matrix3 [9.268,37.170,0.000] [-37.170,9.268,0.000] [0.000,0.000,38.308] [11339.015,13522.982,2533.870] 
	importSpeedTree "k_teress_ElmStocky_RT.spt" "k_teress_ElmStocky_RT_011" tm
)
(
	local tm = matrix3 [2.005,38.256,0.000] [-38.256,2.005,0.000] [0.000,0.000,38.308] [9921.823,12472.154,2533.870] 
	importSpeedTree "k_teress_ElmStocky_RT.spt" "k_teress_ElmStocky_RT_012" tm
)
(
	local tm = matrix3 [33.000,0.000,0.000] [0.000,33.000,0.000] [0.000,0.000,33.000] [17589.227,9681.906,434.228] 
	importSpeedTree "k_teress_ElmStocky_RT_2.spt" "k_teress_ElmStocky_RT_2_006" tm
)
(
	local tm = matrix3 [33.000,0.000,0.000] [0.000,33.000,0.000] [0.000,0.000,33.000] [14643.597,6097.631,-91.009] 
	importSpeedTree "k_teress_ElmStocky_RT_2.spt" "k_teress_ElmStocky_RT_2_007" tm
)
(
	local tm = matrix3 [52.053,0.000,0.000] [0.000,52.053,0.000] [0.000,0.000,52.053] [10498.542,14008.279,2295.185] 
	importSpeedTree "k_teress_ElmStocky_RT_2.spt" "k_teress_ElmStocky_RT_2_008" tm
)
(
	local tm = matrix3 [-20.261,18.243,0.000] [-18.243,-20.261,0.000] [0.000,0.000,27.264] [8476.706,7621.729,1785.350] 
	importSpeedTree "k_teress_Linden_RT.spt" "k_teress_Linden_RT_009" tm
)
(
	local tm = matrix3 [-20.261,18.243,0.000] [-18.243,-20.261,0.000] [0.000,0.000,27.264] [12665.897,9347.367,435.025] 
	importSpeedTree "k_teress_Linden_RT.spt" "k_teress_Linden_RT_010" tm
)
(
	local tm = matrix3 [7.640,24.988,0.000] [-24.988,7.640,0.000] [0.000,0.000,26.130] [16015.114,9532.219,26.524] 
	importSpeedTree "k_teress_Ocotillo.spt" "k_teress_Ocotillo_003" tm
)
(
	local tm = matrix3 [26.130,0.000,0.000] [0.000,26.130,0.000] [0.000,0.000,26.130] [14699.187,9526.578,20.666] 
	importSpeedTree "k_teress_Ocotillo.spt" "k_teress_Ocotillo_004" tm
)
(
	local tm = matrix3 [26.130,0.000,0.000] [0.000,26.130,0.000] [0.000,0.000,26.130] [14947.089,6478.291,-19.330] 
	importSpeedTree "k_teress_Ocotillo.spt" "k_teress_Ocotillo_005" tm
)
(
	local tm = matrix3 [16.087,20.591,0.000] [-20.591,16.087,0.000] [0.000,0.000,26.130] [13662.390,6507.045,-59.743] 
	importSpeedTree "k_teress_Ocotillo.spt" "k_teress_Ocotillo_006" tm
)
(
	local tm = matrix3 [26.130,0.000,0.000] [0.000,26.130,0.000] [0.000,0.000,26.130] [13182.151,8173.014,482.793] 
	importSpeedTree "k_teress_Ocotillo.spt" "k_teress_Ocotillo_007" tm
)
(
	local tm = matrix3 [26.130,0.000,0.000] [0.000,26.130,0.000] [0.000,0.000,26.130] [13931.878,11430.865,29.208] 
	importSpeedTree "k_teress_Ocotillo.spt" "k_teress_Ocotillo_008" tm
)
(
	local tm = matrix3 [26.130,0.000,0.000] [0.000,26.130,0.000] [0.000,0.000,26.130] [10594.062,7886.154,497.371] 
	importSpeedTree "k_teress_Ocotillo.spt" "k_teress_Ocotillo_009" tm
)
(
	local tm = matrix3 [5.346,27.500,0.000] [-27.500,5.346,0.000] [0.000,0.000,28.015] [10160.917,8014.537,507.261] 
	importSpeedTree "k_teress_Ocotillo.spt" "k_teress_Ocotillo_010" tm
)
(
	local tm = matrix3 [5.346,27.500,0.000] [-27.500,5.346,0.000] [0.000,0.000,28.015] [10602.179,6519.158,341.355] 
	importSpeedTree "k_teress_Ocotillo.spt" "k_teress_Ocotillo_011" tm
)
(
	local tm = matrix3 [-22.665,16.467,0.000] [-16.467,-22.665,0.000] [0.000,0.000,28.015] [12799.671,5317.107,-51.630] 
	importSpeedTree "k_teress_Ocotillo.spt" "k_teress_Ocotillo_012" tm
)
(
	local tm = matrix3 [19.721,-17.143,0.000] [17.143,19.721,0.000] [0.000,0.000,26.130] [17525.738,9367.367,406.954] 
	importSpeedTree "k_teress_Ocotillo.spt" "k_teress_Ocotillo_014" tm
)
(
	local tm = matrix3 [21.158,0.000,0.000] [0.000,21.158,0.000] [0.000,0.000,21.158] [15021.476,7479.275,-44.898] 
	importSpeedTree "k_teress_PinOak_RT.spt" "k_teress_PinOak_RT_044" tm
)
(
	local tm = matrix3 [21.158,0.000,0.000] [0.000,21.158,0.000] [0.000,0.000,21.158] [10188.956,9882.625,506.958] 
	importSpeedTree "k_teress_PinOak_RT.spt" "k_teress_PinOak_RT_045" tm
)
(
	local tm = matrix3 [7.237,19.882,0.000] [-19.882,7.237,0.000] [0.000,0.000,21.158] [13318.804,6003.795,-100.855] 
	importSpeedTree "k_teress_PinOak_RT.spt" "k_teress_PinOak_RT_046" tm
)
(
	local tm = matrix3 [-7.926,19.618,0.000] [-19.618,-7.926,0.000] [0.000,0.000,21.158] [13437.226,10543.148,-26.886] 
	importSpeedTree "k_teress_PinOak_RT.spt" "k_teress_PinOak_RT_047" tm
)

-- Directional light 임포트 ----------------------------------------------
c = TargetDirectionallight pos:[0,0,0] target:(targetObject pos:[-0.745,0.298,-0.596])
c.overShoot = true

-- 에러 메시지 출력 ; 주석 처리 하지 마세요-------------------------------
displayNotExistSpeedFiles()


