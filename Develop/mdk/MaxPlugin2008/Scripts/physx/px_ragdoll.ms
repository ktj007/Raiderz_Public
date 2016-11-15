
-- PhysX GUI for Ragdoll
fn Px_Get_Node_User_Prop n key default = 
(
	if n == undefined then return default
	ret = GetUserProp n key
	if undefined == ret then default else ret
)

struct Px_Bone_Tools
(
	ragdoll = undefined,   -- the node of the ragdoll mesh
	boneList=#(),          -- list of bone nodes
	boneNames = #(),
	vertex = #(),
	parentBones = #(),   -- list of parents of the bones
	selectedBones = "",
	token = ",",
	rbTag = "hull_",
	jointTag = "Constaint_",
	--hulls = #(),
	rbs = #(),
	makeHullAsRB = false,   -- if false, set bones as rb and hull as proxy, if true, set hull as rb
	
	fn makeAsRb n proxyName =
	(
		setuserprop n "PhysicsType" PX_PHYSTYPE_DYNAMIC
		if proxyName != undefined then
		(
			setuserprop n  "Proxy_Geometry" proxyName
			local p = findobject proxyName
			if p != undefined then setuserprop p "PhysicsType"  PX_PHYSTYPE_RB_OVER
		)
	),
	
	fn buildConstaint parentNode childNode =
	(
		t = Px_Get_Node_User_Prop parentNode "PhysicsType" PX_PHYSTYPE_UNDEFINED
		if t != PX_PHYSTYPE_DYNAMIC then return undefined
		t = Px_Get_Node_User_Prop childNode "PhysicsType" PX_PHYSTYPE_UNDEFINED
		if t != PX_PHYSTYPE_DYNAMIC then return undefined
		
		d6 = pxJoint()
		d6.body0 = parentNode
		d6.body1 = childNode
		--d6.linearModeX = 1
		--d6.linearModeY = 1
		--d6.linearModeZ = 1
	    d6.breakable       = false
	    d6.maxForce        = 10
	    d6.maxTorque       = 10
	    
	    d6.collision       = false
	    
	    d6.projectionMode  = 1
	    d6.projectionDist  = 0.1
	    d6.projectionAngle = 0.0872
	    
	    d6.gearing   = false
	    d6.gearRatio = 1
	
		d6.aptype	= 1
		
		d6.x_state = 1
		d6.y_state = 1
		d6.z_state = 1
		--d6.swing1Mode = 1
		--d6.swing2Mode = 1
		d6.swing1_locked = true
		d6.swing1_limited = false
		d6.swing2_locked = true
		d6.swing2_limited = false
	    d6.swing1_angle   = 10
	    d6.swing1_rest    = 0
	    d6.swing1_spring  = 0
	    d6.swing1_damp    = 0

	    d6.swing2_angle   = 10
	    d6.swing2_rest    = 0
	    d6.swing2_spring  = 0
	    d6.swing2_damp    = 0

	    d6.twist_rest     = 0
	    d6.twist_spring   = 0
	    d6.twist_damp     = 0
	    d6.twist_lmt      = true
		--d6.twistMode = 2
		--d6.twistAngleLow = -60
		--d6.twistAngleHigh = 60
		d6.twist_enbl = true
		d6.twistlow = 0
		d6.twisthigh = 60
		pos = pxTools.getNodePivot childNode
		if pos == undefined then pxTools.getNodePosition childNode; --format "have get position instead of pivot from node %\n" childNode
		if pos != undefined then d6.pos = pos
		d6.pivot = d6.center
		d6.helpersize = 1.0
	    d6.xlate_rad = 45.0
		d6.buildD6JointMesh()
		return d6
	),
	
	fn BuildDirectParents = 
	(
		for i = 1 to boneList.count do
		(
			parent = boneList[i].parent
			parentBones[i] = 0
			for j = 1 to boneList.count do
			(
				if parent == boneList[j] then 
				(
					parentBones[i] = j; 
					--format " parent '%' and child '%'\n" parent.name boneList[i].name
					exit
				)
			)
		)
	),

	fn AnalysisRagdoll n =
	(
		if n == undefined then return 0
		ragdoll = n
		boneList=#()    -- list of objects
		boneNames = #()
		skinedBones = #()
		vertex = #()
		parentBones = #()
		
		max modify mode
		--select n
		
		sk = n.modifiers[#Skin]
		if sk != undefined then
		(
			modPanel.setCurrentObject sk
			numBones = skinops.getnumberbones sk
			for i = 1 to numBones do
			(
				local t = #()
				nameBone = skinops.getbonename sk i 1
				boneNames[i] = nameBone
				boneList[i] = getnodebyname nameBone
				vertex[i] = #()
				--format "find bone % $'%'\n" i nameBone
			)
			for i = 1 to (skinOps.GetNumberVertices sk) do
			(
				w = 0
				local id = 0
				local nameBone
				for j = 1 to (skinOps.GetVertexWeightCount sk i) do
				(
					boneID = (skinOps.GetVertexWeightBoneID sk i j)
					boneName = (skinOps.GetBoneName sk boneID 0)
					VertWeight = (skinOps.GetVertexWeight sk i j)
					if VertWeight > w then 
					(
						w = VertWeight; 
						id = boneID; 
						nameBone = boneName
					)
				)
				if id > 0  then append vertex[id] i
				--format ("   Bone % name '%' weight % name(%)\n") id nameBone w boneNames[id]
			)
			
			BuildDirectParents()
		)
	),
	
	fn AutoSelectSkinedBones =
	(
		local flag = false
		selectedBones = ""
		for i = 1 to boneList.count do
		(
			--format " % bone '%' vertex(%)\n" i boneNames[i] vertex[i]
			if (vertex[i].count > 3) then 
			(
				t = i as string
				if flag then t = token + t
				selectedBones += t
				flag = true
			)
		)
		--format "selectedBones = %\n" selectedBones
	),
	
	fn GetBonesFromIndex indexString =
	(
		try(
			list_ = filterString indexString token
			local names = #()
			for i in list_ do
			(
				index = i as integer
				if index != undefined then append names boneNames[index]
			)
		)catch()
		return names
	),
	
	fn SelectBones names =
	(
		local result_ = ""
		local flag_ = false
		for n in names do
		(
			for i = 1 to boneList.count do
			(
				if n == boneNames[i] then
				(
					t_ = i as string
					if flag_ then t_ = token + t_
					result_ += t_
					flag_ = true
					exit
				)
			)
		)
		selectedBones = result_
	),
	
	fn CreateHulls nodeNames =
	(
		rbs = #()
		-- delete rigidbodies and hulls
		for i = 1 to boneNames.count do
		(
			setuserprop boneList[i] "PhysicsType" PX_PHYSTYPE_UNDEFINED
			--
			hull = rbTag + boneNames[i]
			aNode = getnodebyname hull
			if aNode != undefined then delete aNode
			rbs[i] = undefined
		)
		for n in nodeNames do
		(
			for i = 1 to boneList.count do
			(
				if n == boneNames[i] then
				(
					if vertex[i].count < 4 then exit
					local points = #()
					local bonePose = boneList[i].transform
					local relativePose = inverse bonePose
					for j in vertex[i] do
					(
						pos = ragdoll.verts[j].pos * relativePose
						append points pos.x
						append points pos.y
						append points pos.z
					)
					-- create hull node
					t = box()
					try(convertToMesh t) catch()
					t.mesh = px.createConvexFromPoints points 32 0 
					t.name = rbTag + boneNames[i]
					t.transform = bonePose
					pv = pxTools.getNodePivot boneList[i]
					if pv == undefined then pv = pxTools.getNodePosition boneList[i]
					
					if pv == undefined then 
					(
						--format "fail to get pivot from boneList[i]  i = %, boneList.count = %\n" i boneList.count
						format "fail to get pivot from boneList[i] = %\n" boneList[i]
					)
					else
					(
						t.pivot = pxTools.getNodePivot boneList[i]
					)
					
					if makeHullAsRB then 
					(
						-- set hull as rb
						makeAsRb t undefined
						rbs[i] = t
					)
					else
					(
						-- set bone as RB and hull as proxy
						makeAsRb boneList[i] t.name
						rbs[i] = boneList[i]
					)
					exit
				)
			)
		)
	),
	
	fn DeleteJoints =
	(
		local number = objects.count
		for i = 1 to number do
		(
			n = objects[number - i + 1]
			if classof(n) == pxJoint then
			(
				if (n.body0 == undefined) or (n.body1 == undefined) then 
				(
					local p = findString n.name jointTag
					if p != undefined then
					(
						p = findString n.name rbTag
						if p != undefined then format "delete d6 %\n" n.name; delete n
					)
				)
			)
		)
	),

	fn DeleateJoints2 = 
	(
		local numBones = boneList.count
		--format "numBones = %\n" numBones
		-- delete old joints
		for i = 1 to numBones do
		(
			if rbs[i] != undefined then
			(
				--format "search parent node for %\n" rbs[i]
				local findParent = undefined
				j = i
				k = 0
				while findParent == undefined do
				(
					if parentBones[j] != 0 then
					(
						if rbs[parentBones[j]] != undefined then (findParent = rbs[parentBones[j]]; exit)
						j = parentBones[j]
					)
					else
					(
						findParent = rbs[i]
					)
					k += 1
					if k > numBones then 
					(
						findParent = rbs[i]; 
						format "   ! fail to find parent for node '%'\n" rbs[i].name
						exit
					)
				)
				if findParent != rbs[i] then 
				(
					d6Name = jointTag + findParent.name + "_" + rbs[i].name
					--format "try to delete constraint %\n" d6Name
					n_ = getnodebyname d6Name
					if n_ != undefined then delete n_; --format "  delete constraint %\n" d6Name
				)
			)
		)
	),
		
	fn CreateJoints =
	(
		--format "boneList = %\n" boneList
		--format "boneList.count = %\n" boneList.count
		DeleteJoints()
		local numBones = boneList.count
		-- create joints
		for i = 1 to numBones do
		(
			if rbs[i] != undefined then
			(
				local findParent = undefined
				j = i
				k = 0
				while findParent == undefined do
				(
					if parentBones[j] != 0 then
					(
						if rbs[parentBones[j]] != undefined then (findParent = rbs[parentBones[j]]; exit)
						j = parentBones[j]
					)
					else
					(
						findParent = rbs[i]
					)
					k += 1
					if k > numBones then 
					(
						findParent = rbs[i]; 
						format "   ! fail to find parent for node '%'\n" rbs[i].name
						exit
					)
				)
				if findParent != rbs[i] then 
				(
					--format " Joint: parent '%' vs child '%'\n" findParent.name rbs[i].name
					d6 = buildConstaint findParent rbs[i]
					if d6 != undefined then d6.name = "Constaint_" + findParent.name + "_" + rbs[i].name
				)
				else
				(
					format "   fail to find parent for node '%'\n" rbs[i].name
				)
			)
		)
	)
)

Px_Bone_Tools = Px_Bone_Tools()

rollout ragdoll_creation_roll "PhysX Ragdoll"
(
	label       lbl_1                "Pick Skin"        across:2
	pickbutton  pkTarget             "undefined"	    message:""  toolTip:"Pick a ragdoll" autoDisplay: true
	group "Bones"
	(
		listbox    boneList          caption:"Bones"      height:15    items:#()
		button     btLoadAllBones    "Load bones"         align:#left  tooltip:"Load all skined bones to list" across:2
		button     btRemoveBone      "Remove"             align:#right tooltip:"Remove trivial bones for better effects"
	)
	checkbox   ckHullAsRB        "Hull as Rigidbody"      align:#left  tooltip:"true: make Hull as RB; false: make Bone as RB and Hull as proxy"
	button     btCreateHull      "Create Hulls"           align:#left  tooltip:"Create Convex Hulls for each bone and/or each bipped node"
	button     btCreateJoints    "Create Joints"          align:#left  tooltip:"Create Joints to connect bones"
	
	on ckHullAsRB changed state do
	(
		--Px_Bone_Tools.makeHullAsRB = state
	)
	
	on btRemoveBone pressed do
	(
		if (pkTarget.object != undefined) and (boneList.selection != 0) then
		(
			local temp = boneList.items
			deleteItem temp boneList.selection
			boneList.items = temp
			Px_Bone_Tools.SelectBones boneList.items
			--format "write Px_Bone_Tools.selectedBones = %\n" Px_Bone_Tools.selectedBones
			SetUserProp pkTarget.object "px_ragdoll_bones" Px_Bone_Tools.selectedBones
		)
	)
	
	on btCreateHull pressed do
	(
		Px_Bone_Tools.makeHullAsRB = ckHullAsRB.checked
		Px_Bone_Tools.CreateHulls boneList.items
	)
	
	on btCreateJoints pressed do
	(
		Px_Bone_Tools.makeHullAsRB = ckHullAsRB.checked
		Px_Bone_Tools.CreateJoints()
	)
	
	fn LoadAllSkinedBones aNode = 
	(
		Px_Bone_Tools.AnalysisRagdoll aNode
		Px_Bone_Tools.AutoSelectSkinedBones()
		--format "write Px_Bone_Tools.selectedBones = %\n" Px_Bone_Tools.selectedBones
		temp_ = Px_Get_Node_User_Prop pkTarget.object "px_ragdoll_bones" ""
		if temp_ != "" then (Px_Bone_Tools.selectedBones = temp_) else (SetUserProp aNode "px_ragdoll_bones" Px_Bone_Tools.selectedBones)
	)
	
	on btLoadAllBones pressed do
	(
		if (pkTarget.object != undefined) then
		(
			Px_Bone_Tools.AnalysisRagdoll pkTarget.object
			Px_Bone_Tools.AutoSelectSkinedBones()
			SetUserProp pkTarget.object "px_ragdoll_bones" Px_Bone_Tools.selectedBones
			--format "write Px_Bone_Tools.selectedBones = %\n" Px_Bone_Tools.selectedBones
			boneList.items = Px_Bone_Tools.GetBonesFromIndex Px_Bone_Tools.selectedBones
		)
	)
	
	fn reload =
	(
		boneList.items = #()
		if (pkTarget.object == undefined) then return 0
		for i in $selection do
		(
			pkTarget.object = i
			exit
		)
		--boneList.items = Px_Get_Node_User_Prop pkTarget.object "px_ragdoll_bones" #()
		if Px_Bone_Tools.ragdoll != pkTarget.object then 
		(
			Px_Bone_Tools.AnalysisRagdoll pkTarget.object
			Px_Bone_Tools.AutoSelectSkinedBones()
			--format "write Px_Bone_Tools.selectedBones = %\n" Px_Bone_Tools.selectedBones
			temp_ = Px_Get_Node_User_Prop pkTarget.object "px_ragdoll_bones" ""
			if temp_ != "" then (Px_Bone_Tools.selectedBones = temp_) else (SetUserProp pkTarget.object "px_ragdoll_bones" Px_Bone_Tools.selectedBones)
		)
		boneList.items = Px_Bone_Tools.GetBonesFromIndex Px_Bone_Tools.selectedBones
		--
		ckHullAsRB.checked = Px_Bone_Tools.makeHullAsRB
	)
	
	on pkTarget picked obj do
	(
		if obj != undefined then 
		(
			local sk = obj.modifiers[#Skin]
			if sk == undefined then 
			(
				format "This mesh (%) does not contain any skin information.\n" obj
				messagebox "This mesh does not contain any skin information." beep:true
				pkTarget.object = undefined
			)
			reload()
		)
		else
		(
			boneList.items = #()
		)
	)
	
	function enableControls state = 
	(
	)
	
	function updateEditProperties = 
	(
		reload()
	)
	
	function updateSelection &props =
	(
		return true
	)
	
	on px_ragdoll_panel open do
	(
		reload()
	)
)

fn PxShowRagdollDialog =
(
	if ragdoll_creation_roll != undefined then
	(
		createDialog ragdoll_creation_roll width:200 style:#(#style_titlebar, #style_border, #style_sysmenu, #style_minimizebox)
	)
)

fn PxCloseRagdollDialog =
(
	if ragdoll_creation_roll.open then DestroyDialog ragdoll_creation_roll
)

callbacks.removeScripts id:#PhysXPluginRagdoll
callbacks.addScript #filePreOpen  "PxCloseRagdollDialog()" id:#PhysXPluginRagdoll
callbacks.addScript #systemPreNew "PxCloseRagdollDialog()" id:#PhysXPluginRagdoll
callbacks.addScript #systemPreReset  "PxCloseRagdollDialog()" id:#PhysXPluginRagdoll
--
utility px_Ragdoll_Editor "Ragdoll Creator"  
(
	button btShowDialog "Ragdoll Dialog"
	on btShowDialog pressed do
	(
		PxShowRagdollDialog()
	)
)
