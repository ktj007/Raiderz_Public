macroScript GunZ2SoundGeometry category:"GunZ2"
(
	-- 선택된게 1개라도 있어야한다.
	if selection.count > 0 then
	(
		-- 파일 이름을 입력받는다.
		out_name = getSaveFileName caption:"GunZ2 Sound Geometry FIle" types:"GunZ2 Sound Geometry (*sge)|*.sge|All Files (*.*)|*.*|"
		
		if (out_name != undefined ) then
		(
			-- 파일을 열고
			out_file = fopen out_name "wb"
			
			-- 모든 폴리곤(face) 갯수를 세고 저장한다.
			num_faces = 0
			for i = 1 to selection.count do
			(
				tmesh = snapShotAsMesh selection[i]
				num_faces += tmesh.numfaces
			)		
			writeLong out_file num_faces
					
			-- 선택된 모든 메시의 폴리곤 정보를 저장한다.
			for i = 1 to selection.count do
			(			
				tmesh = snapShotAsMesh selection[i]
				num_verts = tmesh.numverts
				num_faces = tmesh.numfaces
				
				for f = 1 to num_faces do
				(				
					face = getFace tmesh f
					
					v0 = getVert tmesh ((int)face.x)
					v1 = getVert tmesh ((int)face.y)
					v2 = getVert tmesh ((int)face.z)
					
					-- Max 좌표계 --> OpenGL 좌표계
					writeFloat out_file -v0.x
					writeFloat out_file v0.z
					writeFloat out_file v0.y
					writeFloat out_file -v1.x
					writeFloat out_file v1.z
					writeFloat out_file v1.y
					writeFloat out_file -v2.x
					writeFloat out_file v2.z
					writeFloat out_file v2.y
				)
							
				delete tmesh
			)
			
			-- 저장하고 끝
			fclose out_file	
		)
	)
	else
	(
		messagebox "선택된 메시가 없습니다"
	)
)