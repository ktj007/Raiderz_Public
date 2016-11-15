macroScript AlignU category:"MAIET"
(
	on Execute do
	(	
		local uv = modpanel.getcurrentobject()
		uvsel = uv.getselectedvertices() as array
		mid = 0.0
		for i in uvsel do
		(
			mid += (uv.getvertexposition currenttime i).x
		)
		mid /= uvsel.count
		
		undo "Align U" on
		(
			uv.forceupdate false
			uv.movex mid
			uv.forceupdate true
			uv.updateview()
		)	
	)
)

macroScript AlignV category:"MAIET"
(
	on Execute do
	(	
		local uv = modpanel.getcurrentobject()
		uvsel = uv.getselectedvertices() as array
		mid = 0.0
		for i in uvsel do
		(
			mid += (uv.getvertexposition currenttime i).y
		)
		mid /= uvsel.count
		
		undo "Align V" on
		(
			uv.forceupdate false
			uv.movey mid
			uv.forceupdate true
			uv.updateview()
		)
	)
)