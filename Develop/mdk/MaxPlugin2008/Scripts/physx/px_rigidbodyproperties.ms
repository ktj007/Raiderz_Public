-- 
-- This file contains the rollout code for editing rigid body properties
--
	--can't get these values to stay in scope when they are local, they become empty lists all of a sudden, making them global for now
	global rbprops_mRigidBodies = #();
	global rbprops_mShapes = #();

	rollout rigidbodyproperties_roll "Rigid Body Properties"
	(
		label		lbl_a	"Specify physics representation:"	pos:[4,5]
	    -- dropdownlist	geometrytype_ui			""						items:#("Automatic", "Sphere", "Box", "Capsule", "Convex", "Triangle Mesh (Static only)")	pos:[5,21] width:150
	    dropdownlist	geometrytype_ui			""						items:#("Automatic")	pos:[5,21] width:150
		-- label		lbl_b	"Object(s) simulated as: "		pos:[4,43]
		-- button		simulatedas_ui	"..."	pos:[115,43]	height:15	width:40
	
		spinner       	mass_ui            "Mass"                                       range:[0,1000000,0]  pos:[15,70] width:60
		spinner       	density_ui            "Density"                                       range:[0,10000000,0] pos:[95,70] width:60
		
		label		lbl_c	"Material settings:"	align:#left
	    dropdownlist	material_ui			""						items:#("Use 3ds max material", "Use object material settings", "Use PhysX default material")	pos:[5,108] width:150

		spinner       statfric        "Stat. Friction:      "                                       range:[0,5,0.3]  align:#right width:120
		spinner       fric            "Dyn. Friction:      "                                       range:[0,5,0.3]  align:#right width:120
		spinner       elas            "Restitution:    "                                       range:[0,5,0.3]  align:#right width:120
		label         initVel_lb       "Initial Velocity:"                                     align:#left height:15
		spinner       initVelX        "x"														range:[-100000,100000,0] align:#right   width:46 across:3
		spinner       initVelY        "y"														range:[-100000,100000,0] align:#right   width:46
		spinner       initVelZ        "z"														range:[-100000,100000,0] align:#right   width:46
		label         initSpin_lb      "Initial Spin: (Deg)"									align:#left height:15
		spinner       initSpinX        "x"														range:[-100000,100000,0] align:#right   width:46 across:3
		spinner       initSpinY        "y"														range:[-100000,100000,0] align:#right   width:46
		spinner       initSpinZ        "z"														range:[-100000,100000,0] align:#right   width:46
		
		-- label         lproxy          "Proxy:"                                                align:#center height:15 across:2
		group "Proxy"
		(
			-- width:80 height:15
			checkbutton   ui_proxy        "<proxy>"                                              aligh:#left  tooltip:"select the node that will be used as proxy" across:2
			button        ui_proxyalign   "Align"          align:#right  tooltip:"Set the proxy to the position of the selected node."
			checkbox      proxyvisible    "Visible"                                              align:#right enable:false
		)
		
		group "Others"
		(
			checkbox      puttosleep      "Make it sleep"                                           align:#left enable:false
			checkbox      useCCD          "Enable CCD"                                  align:#left enable:false
		)

		local massFoundFirst = 0;
		local mSameGeometryType = true;
		
		on ui_proxyalign pressed do
		(
			for i in $selection do
			(
				local pgNode = getproxy i;
				if (pgNode != undefined) then
				(
					pxTools.alignPosNodeAndProxy i pgNode
				)
			)
		)
		
		function updateRigidBodyLists = 
		(
			rbprops_mRigidBodies = #();
			rbprops_mShapes = #();
			px_findRigidBodies $selection &rbprops_mRigidBodies false;
			px_findShapes rbprops_mRigidBodies &rbprops_mShapes;
		)

		function updateSelection &props = 
		(
			updateRigidBodyLists();

			-- can't update GUI objects in here, it can be called before the gui has been opened
			local differences = false;
			mSameGeometryType = true;
			for n in rbprops_mRigidBodies do
			(
				if (checkFloatProperty n &props "InitialVelocityX" 0.0) then differences = true;
				if (checkFloatProperty n &props "InitialVelocityY" 0.0) then differences = true;
				if (checkFloatProperty n &props "InitialVelocityZ" 0.0) then differences = true;
				if (checkFloatProperty n &props "InitialSpinX" 0.0) then differences = true;
				if (checkFloatProperty n &props "InitialSpinY" 0.0) then differences = true;
				if (checkFloatProperty n &props "InitialSpinZ" 0.0) then differences = true;
				if (checkStringProperty n &props "Proxy_Geometry" "") then differences = true;
				if (checkFloatProperty n &props "Mass" 0.0) then differences = true;
				if (checkFloatProperty n &props "Density" 0.0) then differences = true;
				if (checkBoolProperty n &props "PutToSleep" false) then differences = true;
				if (checkBoolProperty n &props "EnableCCD" false) then differences = true;
				if (massFoundFirst == 0) then
				(
					local hasMass = (pxMap_hasKey &props "Mass" != undefined);
					if hasMass then massFoundFirst = 1;
					if ( ((pxMap_hasKey &props "Density") != undefined) and not hasMass ) then massFoundFirst = 2;
				)
			)
			for n in rbprops_mShapes do
			(
				if (checkFloatProperty n &props "Friction" 0.0) then differences = true;
				if (checkFloatProperty n &props "StaticFriction" 0.0) then differences = true;
				if (checkFloatProperty n &props "Restitution" 0.0) then differences = true;
				if (checkFloatProperty n &props "Ellasticity" 0.0) then differences = true;
				if (checkIntegerProperty n &props "MaterialType" 2) then differences = true;
				if (checkIntegerProperty n &props "GeometryType" 1) then (
					differences = true;
					mSameGeometryType = false;
				)
			)
			
			return differences;
		)
		
		function getSimulationTypeString type = (
			return case type of 
			(
				1: "Automatic";
				2: "Sphere";
				3: "Box";
				4: "Capsule";
				5: "Convex";
				6: "Triangle Mesh";
				default: "Unknown";
			)
		)
		
		function updateUIGeometryType geomType =
		(
			if (mSameGeometryType) then
			(
				if (geomType == 1) then
				(
					-- TODO: need to go through all shapes to see that they are the same type
					local geomTypeStr = "Auto...";
					if (rbprops_mShapes.count == 1) then
					(
						local sc = classof rbprops_mShapes[1];
						if (sc == Box) or (sc == Capsule) or (sc == Sphere) then 
						( 
							geomTypeStr = sc as string;
						)
					) else
					(
						mSameGeometryType = false;
					)
					-- simulatedas_ui.caption = geomTypeStr;
				) else
				(
					local geomTypeStr = getSimulationTypeString geomType;
					-- simulatedas_ui.caption = geomTypeStr;
				)
			) else
			(
				mSameGeometryType = false;
				-- simulatedas_ui.caption = "...";
			)		
		)
		
		function updateUIWithReadValues &props =
		(
			fric.value = pxMap_getValue &props "Friction" 0.3;
			statfric.value = pxMap_getValue &props "StaticFriction" 0.3;
			local tmpElas = pxMap_getValue &props "Ellasticity" 0.0;
			elas.value = pxMap_getValue &props "Restitution" tmpElas;
			initVelX.value = pxMap_getValue &props "InitialVelocityX" 0.0;
			initVelY.value = pxMap_getValue &props "InitialVelocityY" 0.0;
			initVelZ.value = pxMap_getValue &props "InitialVelocityZ" 0.0;
			initSpinX.value = (pxMap_getValue &props "InitialSpinX" 0.0) * RATE_RAD_TO_DEGREE;
			initSpinY.value = (pxMap_getValue &props "InitialSpinY" 0.0) * RATE_RAD_TO_DEGREE;
			initSpinZ.value = (pxMap_getValue &props "InitialSpinZ" 0.0) * RATE_RAD_TO_DEGREE;
			if (massFoundFirst == 0 or massFoundFirst == 1) then
			(
				mass_ui.value = pxMap_getValue &props "Mass" 0.0;
			)
			if (mass_ui.value == 0.0 or massFoundFirst == 2) then
			(
				density_ui.value = pxMap_getValue &props "Density" 0.0;
			)
			if (mass_ui.value == 0.0 and density_ui.value == 0.0) then
			(
				mass_ui.value = 0.0; --1.0;
				density_ui.value = 1.0;
			)
			material_ui.selection = pxMap_getValue &props "MaterialType" 2;
			geometrytype_ui.selection = pxMap_getValue &props "GeometryType" 1;
			
			proxyvisible.enabled = false;
			puttosleep.checked = getValueAsBool (pxMap_getValue &props "PutToSleep" false) false;
			-- print (format "updateUIWithReadValues - puttosleep = %/n" puttosleep.checked);
			useCCD.checked = getValueAsBool (pxMap_getValue &props "EnableCCD" false) false;
			
			ui_proxy.enabled = false;
			ui_proxy.text = "<proxy>";
			-- only enable proxy button when selected one rigidbody!
			if (rbprops_mRigidBodies.count == 1) then
			(
				ui_proxy.enabled = true;
				local pg = pxMap_getValue &props "Proxy_Geometry" undefined
				if (pg != undefined and pg != "") then
				(
					pobj = findobject pg;
					proxyvisible.enabled = (pobj != undefined);
					if (pobj != undefined) then
					(
						ui_proxy.text = pg
						proxyvisible.state = (not pobj.ishidden);
					)
				)
			)

			local geomType = pxMap_getValue &props "GeometryType" 1
			updateUIGeometryType geomType;
		)

  
	
		function updateEditProperties =
		(
			if ((rbprops_mRigidBodies == undefined) or (rbprops_mRigidBodies.count == 0)) then
			(
				updateRigidBodyLists();
			)
			local density = density_ui.value;
			local mass = mass_ui.value;
			local trimesh = (geometrytype_ui.selection == 6);
			local n = undefined;
			for n in rbprops_mRigidBodies do
			(
				if (mass > 0) then
				(
					setuserprop n "Mass" mass
					eraseprop n "Density"
				) else 
				(
					setuserprop n "Density" density
					eraseprop n "Mass"
				)
			)
			for n in rbprops_mShapes do
			(
				if trimesh then (setuserprop n "IsConcave" 1)
				else eraseprop n "IsConcave";
				
				setUserProp n "GeometryType" geometrytype_ui.selection
			)

			local current_shapes = rbprops_mShapes;

			--material
			for n in current_shapes do setuserprop n "MaterialType" material_ui.selection
			for n in current_shapes do setuserprop n "Friction" fric.value
			for n in current_shapes do setuserprop n "StaticFriction" statfric.value
			for n in current_shapes do setuserprop n "Restitution" elas.value

			--initial velocity
			for n in rbprops_mRigidBodies do setuserprop n "InitialVelocityX" initVelX.value
			for n in rbprops_mRigidBodies do setuserprop n "InitialVelocityY" initVelY.value
			for n in rbprops_mRigidBodies do setuserprop n "InitialVelocityZ" initVelZ.value

			--initial spin
			for n in rbprops_mRigidBodies do setuserprop n "InitialSpinX" (initSpinX.value*RATE_DEGREE_TO_RAD)
			for n in rbprops_mRigidBodies do setuserprop n "InitialSpinY" (initSpinY.value*RATE_DEGREE_TO_RAD)
			for n in rbprops_mRigidBodies do setuserprop n "InitialSpinZ" (initSpinZ.value*RATE_DEGREE_TO_RAD)
			
			for n in rbprops_mRigidBodies do
			(
				setUserProp n "PutToSleep"  puttosleep.checked;
				--print (format "updateEditProperties - puttosleep = % /n" puttosleep.checked);
				setUserProp n "EnableCCD"  useCCD.checked;
			)
		)

		on ui_proxy changed state do
		(
			if state then
			(
				ui_proxy.text = "Pick Proxy Object"
				local p = pickobject count:1 select:false message:"pick object to use as proxy";

				if(p == #escape) then
				(
				  local pg = getUserProp rbprops_mRigidBodies[1] "Proxy_Geometry";
				  if(pg != undefined and pg != "") then
				  (
				    ui_proxy.text = pg;
					proxyvisible.enabled = true;
				  )
				  else
				  (
				    ui_proxy.text = "<proxy>";
					proxyvisible.enabled = false;
				  )
		
				  ui_proxy.state = false;
				  updateRigidBodyLists(); --need to call this, to update the list of RBs and shapes
				  return true;
				)
				if not (px_isshape p) then
				(
				  for n in rbprops_mRigidBodies do setUserProp n "Proxy_Geometry" "";
				  ui_proxy.text = "<proxy>";
				  ui_proxy.state = false;
				  proxyvisible.enabled = false;
				  updateRigidBodyLists();
				  updateEditProperties(); -- need to call update properties, so that the current properties are written to the original shape(s)
				  return true;
				)
				ppt = try(getuserprop p "PhysicsType" as integer) catch(PX_PHYSTYPE_UNDEFINED)
				
				if(ppt < PX_PHYSTYPE_RB_OVER) then 
				(
					px_warning ("Chosen Proxy '"+p.name+"' is itself physical in nature\nPlease use a geometry that is not itself a physics object")
					p = undefined
				) 
				
				local pn=""
				
				if(p != undefined) then
				(
		            -- handle groups by finding the top-most group head for the picked geometry (the groups are in the list of picked geometries, but we can't know the order)
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
					pn = p.name
					proxyvisible.checked = not p.ishidden;
					proxyvisible.enabled = true;
					setuserprop p "PhysicsType"  PX_PHYSTYPE_RB_OVER;  -- disabled  PX_PHYSTYPE_RB_PROXY
					    
					--for n in selection do setuserprop n "Proxy_Geometry" pn;
					--if not (pxTools.isBoneOrBip p) then pxTools.CenterPivot p
					lastNode = undefined
					for n in rbprops_mRigidBodies do 
					(
						setUserProp n "Proxy_Geometry" pn;
						--if not (pxTools.isBoneOrBip n) then pxTools.CenterPivot n
						lastNode = n
					)
					--if lastNode != undefined then p.pos = lastNode.pos
					
					ui_proxy.text = pn;
					ui_proxy.state = false;
					
					updateRigidBodyLists();
					updateEditProperties(); -- need to call update properties, so that the current properties are written to the proxy shape(s)
				)
				else
				(
					ui_proxy.text = "<proxy>";
					ui_proxy.state = false;
					proxyvisible.enabled = false;
					updateRigidBodyLists();
				)
		    )
		    else
		    (
				toolmode.commandmode = #move;
				local pg = getUserProp rbprops_mRigidBodies[1] "Proxy_Geometry";
				if(pg != undefined and pg != "") then
				  ui_proxy.text = pg;
				else
				  ui_proxy.text = "<proxy>";
		    )
		)
		
		on proxyvisible changed state do
		(
			if ui_proxy.text != "<proxy>" then
			(
				obj = findobject ui_proxy.text
				if obj != undefined then
				(
					obj.isHidden = not state
				)
			)
		)

		
		function enableControls editState =
		(
			rigidbodyproperties_roll.statfric.enabled = editState;
			rigidbodyproperties_roll.fric.enabled = editState;
			rigidbodyproperties_roll.elas.enabled = editState;
			rigidbodyproperties_roll.initVelX.enabled = editState;
			rigidbodyproperties_roll.initVelY.enabled = editState;
			rigidbodyproperties_roll.initVelZ.enabled = editState;
			rigidbodyproperties_roll.initSpinX.enabled = editState;
			rigidbodyproperties_roll.initSpinY.enabled = editState;
			rigidbodyproperties_roll.initSpinZ.enabled = editState;
			rigidbodyproperties_roll.ui_proxy.enabled = editState;
			rigidbodyproperties_roll.proxyvisible.enabled = editState;
			rigidbodyproperties_roll.puttosleep.enabled = editState;
			rigidbodyproperties_roll.useCCD.enabled = editState;
			rigidbodyproperties_roll.mass_ui.enabled = editState;
			rigidbodyproperties_roll.density_ui.enabled = editState;
			rigidbodyproperties_roll.material_ui.enabled = editState;
			if editState then
			(
				local matType = rigidbodyproperties_roll.material_ui.selection == 2;
				rigidbodyproperties_roll.statfric.enabled = matType;
				rigidbodyproperties_roll.fric.enabled = matType;
				rigidbodyproperties_roll.elas.enabled = matType;
			)
			-- todo: enable this control when there are multiple objects (of different types) selected
			-- rigidbodyproperties_roll.simulatedas_ui.enabled = false;
		)

		on mass_ui changed val do
		(
			if (mass_ui.value != 0.0) then density_ui.value = 0.0;
			updateEditProperties();
		)

		on density_ui changed val do
		(
			if (density_ui.value != 0.0) then mass_ui.value = 0.0;
			updateEditProperties();
		)
		
		on material_ui selected selIndex do
		(
			enableControls true;
			updateEditProperties();
		)

		on statfric changed val do
		(
			updateEditProperties();
		)
		on fric changed val do
		(
			updateEditProperties();
		)
		on elas changed val do
		(
			updateEditProperties();
		)
		on initVelX changed val do
		(
			updateEditProperties();
		)
		on initVelY changed val do
		(
			updateEditProperties();
		)
		on initVelZ changed val do
		(
			updateEditProperties();
		)
		on initSpinX changed val do
		(
			updateEditProperties();
		)
		on initSpinY changed val do
		(
			updateEditProperties();
		)
		on initSpinZ changed val do
		(
			updateEditProperties();
		)

		on geometrytype_ui selected selIndex do
		(
			local tmpType = px_control.mainUI_roll.simulationmethod_ui.selection;
			format "-> tmpType = %\n" tmpType
			if (tmpType == 1) or (tmpType == 2) then
			(
				-- for dynamic RB, it is not ok to set with triangleMesh
				if (selIndex == 6) then
				(
					messagebox "For Dynamic/Kinematic Rigid Body, they can not be set as Triangle Mesh!"
					geometrytype_ui.selection = 1
					selIndex = 1
				)
			)

			updateUIGeometryType selIndex;
			updateEditProperties();
		)
		
		on rigidbodyproperties_roll open do
		(
			updateRigidBodyLists();
			updateUIWithReadValues px_control.mainui_roll.mPhysXPropertiesOfFirst;
			enableControls true;
		)
		
		on puttosleep changed val do updateEditProperties();
		on useCCD changed val do updateEditProperties();

	--end of rollout	
	)
