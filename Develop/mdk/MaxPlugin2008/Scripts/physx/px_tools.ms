/*
	this is a tool set
*/

struct pxTools
(
	fn isBoneOrBip n =
	(
		if n.boneEnable then return true
		if classof(n) == Biped_Object then return true
		return false
	),
	
	fn isBipedNode n =
	(
		local pos = undefined
		try(pos = biped.getTransform n #pos) catch()
		if (pos != undefine) then return true else return false
	),
	
	/**********************************************
	Checks the node is scaled or not
	**********************************************/
	fn isScaled n =
	(
		-- assume it is biped node
		local scale = undefined
		try(
			--if isBipedNode n then (scale = biped.getTransform n #scale) else scale = n.scale
			scale = n.transform.scale
			local escala = scale - [1,1,1]
			delta = dot escala escala
			if delta > 0.001 then (return true) else (return false)
		)catch()
		return false
	),
	
	fn setNodePosition n value =
	(
		--if isBipedNode n then(biped.setTransform n #pos value) else	(try(n.pos = value) catch()	)
		t = n.transform
		t.row4 = value
		n.transform = t
	),
	
	fn setNodePivot n value =
	(
		if isBipedNode n then
		(
			--biped.setTransform n #pos value
			setNodePosition n value
		)
		else
		(
			try(n.pivot = value) catch()
		)
	),
	
	fn getNodePosition n =
	(
		--local value = undefined 
		if isBipedNode n then (	value = biped.getTransform n #pos	)	else	(	try(value = n.pos) catch()	)
		return n.transform.row4
		--return value
	),
	
	fn getNodePivot n =
	(
		--local value = undefined 
		--if isBipedNode n then (	value = biped.getTransform n #pos	)	else (try(value = n.pivot) catch()	)
		--return value
		local value = undefined 
		try(value = n.pivot) catch( value = getNodePosition n)
		return value
	),
	
	fn alignPivotNodeAndProxy n proxy =
	(
		local pv = pxTools.getNodePivot n
		local center = undefined
		--format "n = % proxy = %\n" n.name proxy.name
		try(center = n.center) catch(format "failed in pxTools.alignPivotNodeAndProxy node = %, proxy = %." n.name proxy.name)
		--format "n pivot = % and center = %\n" pv center
		local t = center - pv
		local proxyCenter = proxy.center
		if (pv != undefined) then pxTools.setNodePivot proxy (proxyCenter - t)
	),
	
	fn alignPosNodeAndProxy n proxy =
	(
		alignPivotNodeAndProxy n proxy
		local pos = getNodePosition n
		pxTools.setNodePosition  proxy pos
	),

	fn GetGroupRoot p =
	(
		if (isGroupMember p) then 
		(
			local pg = p;
			while (pg != undefined and (isGroupMember pg)) do
			(
				p = pg;
				pg = pg.parent;
			)
			if (isGroupHead pg) then (
				p = pg;
			)
		)
		return p
	),

	/**********************************************
	find the root parent node
	**********************************************/
	fn GetRootParent n =
	(
		if n.parent == undefined then 
		(
			return n
		)
		else
		(
			return GetRootParent n.parent
		)
	),
	
	/**********************************************
	find all the children nodes in a group 
	**********************************************/
	fn GetAllGroupChildren n result=
	(
		for i in n.children do
		(
			--format "check child node %\n" i;
			if(isGroupHead i) then
			(
				--format "it is head node\n";
				GetAllGroupChildren i result;
			)
			else if(isGroupMember i) then
			(
				--format "it is group member\n";
				append result i;
				GetAllGroupChildren i result;
			)
		)
	),
	
	fn ScaleBone n = 
	(
		parent = n.parent
		children = #()
		for i in n.children do
		(
			append children i
			i.parent = undefined
		)
		n.parent = undefined
		ResetXForm n
		for i in children do
		(
			i.parent = n
		)
		n.parent = parent
	),

	fn CenterPivot n = 
	(
		try(
		  if n.pivot != n.center then n.pivot = n.center
		) catch()
	),
	
	-- for grouped nodes, they will be merged as one
	fn ScaleGeometryObject n =
	(
		/*
		if n.children.count > 0 then
		(
			for i in n.children do
			(
				if (superclassof(i) == GeometryClass) then i.parent = parent
			)
		)
		if n.parent != undefined then
		(
			if (superclassof(n.parent) == GeometryClass) then n.parent = undefined
		)
		*/
		ResetXForm n
	),
	
	fn ScaleGroupedObject n =
	(
		local children = #()
		local result = #()
		local missed = #()
		GetAllGroupChildren n children
		--format " there are % children\n" children.count
		for i in children do
		(
			if (superclassof(i) == GeometryClass) and (isScaled i) then 
			(
				if i.children.count > 0 then
				(
					for k in i.children do
					(
						if (superclassof(k) == GeometryClass) then k.parent = n
					)
				)
				if i.parent != undefined then
				(
					if (superclassof(i.parent) == GeometryClass) then i.parent = n
				)
				ResetXForm i
			)
		)
	),

	fn RemoveScales n =
	(
		if (superclassof(n) == GeometryClass) then
		(
			if (isScaled n) then 
			(
				if n.boneEnable then
				(
					format "skip scale bone % \n" n
					--ScaleBone n
				)
				else
				(
					ScaleGeometryObject n
				)
			)
		)
		else if(isGroupHead n) then
		(
			if(not isGroupMember n) then
			(
				ScaleGroupedObject n
			)
		)

		return n
	),
	
	fn includeGroups g =
	(
		if g == undefined then return false;
		if (classof(g)==ObjectSet) then
		(
			for n in g do
			(
				if(isGroupHead n) then
				(
					return true;
				)
			)
		)
		else
		(
			return (isGroupHead g);
		)
		return false;
	),
	
	fn filterObjects g groups singles =
	(
		if g == undefined then return false
		if (classof(g)==ObjectSet) then
		(
			for n in g do
			(
				if(not isGroupMember n) then
				(
					if(isGroupHead n) then 
					(
						append groups n
					)
					else
					(
						append singles n
					)
				)
			)
		)
		else
		(
			if(not isGroupMember g) then
			(
				if(isGroupHead g) then 
				(
					append groups g
				)
				else
				(
					append singles g
				)
			)
		)
	)
)

pxTools = pxTools();
