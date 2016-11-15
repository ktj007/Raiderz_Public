plugin helper PxFluid
name:"PxFluid"
category:"NVIDIA PhysX"
classID:#(0xe2757b00, 0x91b46400)
extends:dummy replaceui:true
(	
    -- Note! For some reason the last two digits of the classid are truncated to 00 by 3ds max, so in order
    --       to provide the same classid in maxscript as in the c++ part of the plugin, the last two digits
    --       should be kept as is.
    
   	local displayMesh = undefined;
   	local needRebuild = true;
--	local mNode = undefined;
	local mRebuildNr = -1;
	local mRebuildTransform = matrix3 1
	local BoundToNode = undefined;
	
	
--	local mCurrentNum = 0;
    
	parameters main_p rollout:main_r
	(
		SimulationMethod 		type:#index		ui:simulationmethod_ui			default:1
		
		CollisionMethodStatic  	type:#boolean	ui:collisionmethod_static 		default:true
		CollisionMethodDynamic 	type:#boolean	ui:collisionmethod_dynamic 		default:true
	
		FluidFlagsEnabled		type:#boolean	ui:fluidflags_enabled			default:true
		FluidFlagsHardware		type:#boolean	ui:fluidflags_hardware			default:true
		FluidFlagsTwoway		type:#boolean	ui:fluidflags_twoway			default:false
		FluidFlagsPriorityMode	type:#boolean	ui:fluidflags_prioritymode		default:false
		FluidFlagsDisableGravity type:#boolean	ui:fluidflags_disablegravity	default:false
		FluidFlagsVisualization	type:#boolean	ui:fluidflags_visualization		default:true
		
		InitialParticles		type:#node      ui:initialparticles_ui
	
		MaxParticles			type:#integer	ui:maxparticles_ui				default:32767
		
		ReservedParticles		type:#integer	ui:reservedparticles_ui			default:0
		
		RestParticlesMeter		type:#float		ui:restparticles_ui				default:10
		
		RestDensity				type:#float		ui:restdensity_ui				default:1000
		
		KernelRadiusMultiplier	type:#float		ui:kernelradius_ui				default:1.2
		MotionLimitMultiplier	type:#float		ui:motionlimit_ui				default:3.6		--is 3.0*kernelradiusmultiplier
		CollisionDistanceMultiplier	type:#float	ui:collisiondistance_ui			default:0.12	--is 0.1*kernelradiusmultiplier
		PacketSizeMultiplier	type:#float		ui:packetsize_ui				default:16
	
		Stiffness				type:#float		ui:stiffness_ui					default:20
		Viscosity				type:#float		ui:viscosity_ui					default:6
		Damping					type:#float		ui:damping_ui					default:0
		FadeInTime				type:#float		ui:fadeintime_ui				default:0
		
		ExternalAcceleration	type:#point3									default:[0,0,0]
	
		StaticRestitution		type:#float								default:0.5
		StaticAdhesion			type:#float		ui:staticadhesion_ui			default:0.05
		StaticAttraction		type:#float								default:0
		DynamicRestitution		type:#float								default:0.5
		DynamicAdhesion			type:#float		ui:dynamicadhesion_ui			default:0.5
		DynamicAttraction		type:#float								default:0

		restitutionForStaticShapes		type:#float		ui:staticrestitution_ui			default:0.5
		attractionForStaticShapes		type:#float		ui:staticattraction_ui			default:0
		restitutionForDynamicShapes		type:#float		ui:dynamicrestitution_ui		default:0.5
		attractionForDynamicShapes		type:#float		ui:dynamicattraction_ui			default:0
		
		dynamicFrictionForStaticShapes 	type:#float		ui:dynamicFricForStatic_ui			default:0.05
		staticFrictionForStaticShapes	type:#float		ui:staticFricForStatic_ui			default:0.05
		dynamicFrictionForDynamicShapes	type:#float		ui:dynamicFricForDyn_ui			default:0.5
		staticFrictionForDynamicShapes	type:#float		ui:staticFricForDyn_ui			default:0.5
	
		CollisionResponse		type:#float		ui:collisionresponse_ui			default:0.2	
	
		CollisionGroup			type:#integer	ui:collisiongroup_ui			default:0
		
		FluidCompartment		type:#integer	ui:fluidcompartment_ui			default:0
		
		-- we need a node when creating the mesh (when there are particles)
		--BoundToNode				type:#node
		--Error: not possible to have it as a node property, since there will be a dependency loop
	)
	
	rollout main_r "Fluid Params"
	(
		label			simulationmethod_lb			"Simulation Method"		align:#left
    	dropdownlist	simulationmethod_ui			""						items:#("SPH","No interaction", "Mixed mode")

		label			fluidcompartment_lb			"Fluid Compartment"		align:#left
		spinner			fluidcompartment_ui			""				range:[0,15,1]	type:#integer		pos:[105,50]	width:50

		label         	collisionmethod_lb  		"Collision Method"		align:#left
		checkbox		collisionmethod_static		"Static"											pos:[13,90]
		checkbox		collisionmethod_dynamic		"Dynamic"											pos:[85,90]

		label         	fluidflags_lb  				"Fluid Flags"										pos:[13,115]
		checkbox		fluidflags_enabled			"Enabled"             								pos:[13,130]	width:71	height:16
		checkbox		fluidflags_hardware			"Hardware"             								pos:[85,130]	width:80	height:16
		checkbox		fluidflags_prioritymode		"Prio Mode"											pos:[13,150]	width:71	height:16
		checkbox		fluidflags_disablegravity	"Dis. Gravity"										pos:[85,150]	width:80	height:16
		checkbox		fluidflags_twoway			"RB two-way interaction"
		checkbox		fluidflags_visualization	"Visualization"

		label			initialparticles_lb			"Initial particles:"	align:#left					pos:[13,220]
		pickbutton    	initialparticles_ui      	"undefined"		width:60 message:""	toolTip:"Pick a geometry whose vertices to use as initial particle positions for the fluid"	autoDisplay: true	pos:[85,217] across:2
	    button		    removeInitialParticles_ui	"x"				width:16  pos:[147, 217]				toolTip:"Remove Initial particles"

		label			general_lb					"General Settings"		align:#left		
		label			maxparticles_lb				"Max Particles: "		align:#left
		spinner			maxparticles_ui				""				range:[0,32767,1]	type:#integer	pos:[85,260]

		label			reserveparticles_lb			"Reserved Part: "		align:#left
		spinner			reservedparticles_ui		""				range:[0,32767,1]	type:#integer	pos:[85,280]

		label			stiffness_lb				"Stiffness"				align:#left
		spinner			stiffness_ui				""				range:[0,9999999,1.0]	type:#float	pos:[85,300]

		label			viscosity_lb				"Viscosity"				align:#left
		spinner			viscosity_ui				""				range:[0,9999999,1.0]	type:#float	pos:[85,322]

		label			damping_lb					"Damping"				align:#left
		spinner			damping_ui					""				range:[0,9999999,1.0]	type:#float	pos:[85,343]

		label			fadeintime_lb				"Fade-in Time"			align:#left
		spinner			fadeintime_ui				""				range:[0,9999999,1.0]	type:#float	pos:[85,364]

		label			externalacceleration_lb		"External Acceleration"	align:#left
		label			externalaccelerationx_lb	"x"													pos:[5,405]
		label			externalaccelerationy_lb	"y"													pos:[55,405]
		label			externalaccelerationz_lb	"z"													pos:[105,405]
		spinner			externalaccelerationx_ui	""				range:[-9999999,9999999,1.0]	type:#float	pos:[12,405]	width:40
		spinner			externalaccelerationy_ui	""				range:[-9999999,9999999,1.0]	type:#float	pos:[62,405]	width:40
		spinner			externalaccelerationz_ui	""				range:[-9999999,9999999,1.0]	type:#float	pos:[112,405]	width:40


 -- advanced
		label			advanced_lb					"Advanced Settings"									pos:[13,430]
		label			restparticles_lb			"Particles/m"			align:#left
		spinner			restparticles_ui			""				range:[0,9999999,10.0]	type:#float	pos:[85,448]

		label			restdensity_lb				"Rest Density"			align:#left
		spinner			restdensity_ui				""				range:[0,9999999,1.0]	type:#float	pos:[85,469]

		label			kernelradius_lb				"Radius Mult."			align:#left
		spinner			kernelradius_ui				""				range:[0,9999999,1.0]	type:#float	pos:[85,490]

		label			motionlimit_lb				"Motion Limit Mult."	align:#left
		spinner			motionlimit_ui				""				range:[0,9999999,1.0]	type:#float	pos:[100,511]	width:55

		label			collisiondistance_lb		"Coll.Dist. Mult."		align:#left
		spinner			collisiondistance_ui		""				range:[0,9999999,1.0]	type:#float	pos:[85,531]

		label			packetsize_lb				"Packet Size Mult."		align:#left
		spinner			packetsize_ui				""				range:[0,9999999,1.0]	type:#float	pos:[100,552]	width:55


 -- collision
		label			collision_lb				"Collision Settings"								pos:[13,580]
		label			staticrestitution_lb		"Rest. Static RB"		align:#left
		spinner			staticrestitution_ui		""				range:[0,1,0.1]			type:#float	pos:[95,597]	width:60 enabled:false

		label			staticadhesion_lb			"Adh. Static RB"			align:#left
		spinner			staticadhesion_ui			""				range:[0,1,0.1]			type:#float	pos:[95,618]	width:60 enabled:false

		label			staticattraction_lb			"Attr. Static RB"			align:#left
		spinner			staticattraction_ui			""				range:[0,9999999,1.0]	type:#float	pos:[95,639]	width:60 enabled:false

		label			dynamicrestitution_lb		"Rest. Dyn. RB"			align:#left
		spinner			dynamicrestitution_ui		""				range:[0,1,0.1]			type:#float	pos:[95,660]	width:60 enabled:false

		label			dynamicadhesion_lb			"Adh. Dyn. RB"				align:#left
		spinner			dynamicadhesion_ui			""				range:[0,1,0.1]			type:#float	pos:[95,681]	width:60 enabled:false

		label			dynamicattraction_lb		"Attr. Dyn. RB"			align:#left
		spinner			dynamicattraction_ui		""				range:[0,1,0.1]			type:#float	pos:[95,702]	width:60 enabled:false

		label			collisionresponse_lb		"Coll Resp. Coeff"	align:#left
		spinner			collisionresponse_ui		""				range:[0,9999999,1.0]	type:#float	pos:[95,723]	width:60 enabled:false

		label			collisiongroup_lb			"Collision Group"			align:#left
		spinner			collisiongroup_ui			""				range:[0,9999999,1]		type:#integer	pos:[85,744]

		label			dynamicFricForStatic_lb		"Dyn. Fric. Static RB"			align:#left across:2
		spinner			dynamicFricForStatic_ui		""			range:[0, 1, 0.05]	type:#float	width:45 enabled:true

		label			staticFricForStatic_lb		"Static Fric. Static RB"			align:#left across:2
		spinner			staticFricForStatic_ui		""				range:[0, 1, 0.05]	type:#float	    width:45 enabled:true

		label			dynamicFricForDyn_lb		"Dyn. Fric. Dyn. RB"			align:#left across:2
		spinner			dynamicFricForDyn_ui		""			range:[0, 1, 0.5]	type:#float	width:45 enabled:true

		label			staticFricForDyn_lb			"Static Fric. Dyn. RB"			align:#left across:2
		spinner			staticFricForDyn_ui			""				range:[0, 1, 0.5]	type:#float	    width:45 enabled:true
		
		fn updateStates =
		(
			collisionresponse_ui.enabled = fluidflags_twoway.enabled and fluidflags_twoway.checked
		
			dynamicrestitution_ui.enabled = collisionmethod_dynamic.enabled and collisionmethod_dynamic.checked
			dynamicadhesion_ui.enabled = dynamicrestitution_ui.enabled
			dynamicattraction_ui.enabled = dynamicrestitution_ui.enabled

			staticrestitution_ui.enabled = collisionmethod_static.enabled and collisionmethod_static.checked
			staticadhesion_ui.enabled = staticrestitution_ui.enabled
			staticattraction_ui.enabled = staticrestitution_ui.enabled
		)
		
		on staticrestitution_ui  changed val do (restitutionForStaticShapes = val)
		on staticattraction_ui   changed val do (attractionForStaticShapes = val)
		on dynamicrestitution_ui changed val do (restitutionForDynamicShapes = val)
		on dynamicattraction_ui  changed val do (attractionForDynamicShapes = val)
		
		on fluidflags_twoway changed state do updateStates()
		on collisionmethod_dynamic changed state do updateStates()
		on collisionmethod_static changed state do updateStates()
		
		on removeInitialParticles_ui pressed do
		(
			initialparticles_ui.object = undefined
		)
		
		on main_r open do 
		(
			externalaccelerationx_ui.value = ExternalAcceleration.x;
			externalaccelerationy_ui.value = ExternalAcceleration.y;
			externalaccelerationz_ui.value = ExternalAcceleration.z;
			fluidflags_hardware.enabled = px.hwAvailable();
			fluidflags_hardware.checked = px.hwAvailable();
			
			updateStates()
		)
		
		on externalaccelerationx_ui changed val do
		(
			ExternalAcceleration.x = val
		)
	
		on externalaccelerationy_ui changed val do
		(
			ExternalAcceleration.y = val
		)
	
		on externalaccelerationz_ui changed val do
		(
			ExternalAcceleration.z = val
		)
	)
	
	parameters drain_p rollout:drain_r
	(
		Drains				type:#nodeTab	tabSizeVariable:true
	)

	function filterNode n =
	(
		if (superclassof n) != GeometryClass then return false
		return true;
	)

	function GetPxActor n = 
	(
		if (isGroupMember n) then 
		(
			local pn = n;
			while (pn != undefined and (isGroupMember pn)) do
			(
				n = pn;
				pn = pn.parent;
			)
			if (isGroupHead pn) then (
				n = pn;
			)
		)
		if (n != undefined) then
		(
			local type = try(getuserprop n "PhysicsType" as integer) catch(PX_PHYSTYPE_UNDEFINED);
			if (type < PX_PHYSTYPE_RB_OVER) then return n;
		)
		return undefined;
	)
	
	function GetActorProxy actor =
	(
		local proxyname = try(getuserprop actor "Proxy_Geometry" as string) catch("");
		if (proxyname=="" or proxyname==undefined) then return undefined;
		return findobject(proxyname);
	)

	rollout drain_r "Drains"
	(
		label 			drains_lb 					"Fluid Drains (scene global)"
		listbox			drains_ui					caption:"Drains"	height:5		items:#()
		
		pickbutton 		addNode 					"Add Drain" 		pos:[3,100]		width:60	filter:filterNode
		button 			removeNode 					"Remove Drain" 	pos:[67,100]	width:90
		
		-- button			updateListBtn				"Update"
		
		fn updateDrainList full =
		(
			if (full) then
			(
				Drains = #()
				local tempVal = false;
				for obj in objects do 
				(
				  if (superclassof obj) == GeometryClass then 
				  (
				    local drain = px_getshapeflag obj "NX_SF_FLUID_DRAIN";
					if (drain) then
					(
						append Drains obj;
					)
				  )
				)
			)
			--drains_ui.items = for o in Drains collect o.name
			drains_ui.items = for o in Drains collect (
				local tempActor = GetPxActor(o);
				if (tempActor != undefined) then
				(
					tempActor.name + ":" + o.name;
				) else (
					--probably a proxy geometry for something else, how do we find the actor?
					"Proxy?:" + o.name;
				)
			)
		)
		
		-- on updateListBtn pressed do
		-- (
			-- updateDrainList true
		-- )
		
		on addNode picked obj do --if isValidNode obj do
		(
			local actorNode = GetPxActor(obj)
			if (actorNode == undefined) then 
			(
				local answer = queryBox "The object you selected, is it a proxy geometry for another actor?"	title:"Proxy geometry?"	beep:false
				if answer then 
				(
					if (findItem Drains obj) == 0 then
					(
						--setUserProp obj "FluidDrain" true
						px_setshapeflag obj "NX_SF_FLUID_DRAIN" true
						append Drains obj
					)
					updateDrainList false
					drains_ui.selection = Drains.count
				)
				return false;
			)
			local proxyNode = GetActorProxy(actorNode);
			if (proxyNode != undefined) then
			(
				local proxyShapes = #();
				local stackedNodes = #(proxyNode);
				while (stackedNodes.count > 0) do
				(
					local tempNode = stackedNodes[1];
					deleteitem stackedNodes 1;
					
					if (superclassof tempNode) == GeometryClass then 
					(
						append proxyShapes tempNode
					)

					local temp = undefined;
					for temp in tempNode.children do
					(
						append stackedNodes temp;
					)
				)
				if (proxyShapes.count > 1) then
				(
					local answer = queryBox "The object you selected has a proxy containing several shapes, select all of the proxy shapes as drains?" title:"Question" beep:false
					if (not answer) then
					(
						while (proxyShapes.count > 0) do
						(
							deleteitem proxyShapes 1;
						)
					)
				)
				while (proxyShapes.count > 0) do
				(
					local temp = proxyShapes[1];
					deleteItem proxyShapes 1;
					if (findItem Drains temp) == 0 then
					(
						px_setshapeflag temp "NX_SF_FLUID_DRAIN" true
						--setUserProp temp "FluidDrain" true
						append Drains temp;
					)
				)
				updateDrainList false
				drains_ui.selection = Drains.count
			) 
			else (
				if (findItem Drains obj) == 0 then
				(
					--setUserProp obj "FluidDrain" true
					px_setshapeflag obj "NX_SF_FLUID_DRAIN" true
					append Drains obj
				)
				updateDrainList false
				drains_ui.selection = Drains.count
			)
			updateDrainList true
		)
		on removeNode pressed do
		(
			if drains_ui.selection > 0 do
			(
				try (
				local obj = Drains[drains_ui.selection];
				--eraseProp obj "FluidDrain"
				--setUserProp obj "FluidDrain" false
				px_setshapeflag obj "NX_SF_FLUID_DRAIN" false
				deleteItem Drains drains_ui.selection
				) catch()
				updateDrainList false
				if drains_ui.selection == 0 do
					drains_ui.selection = Drains.count
			)
			updateDrainList true
		)
		on drain_r open do 
		(
			updateDrainList true
		)
	)

	
	rollout about_r "About"
	(
		label about_lb "PhysX Fluid Object"
	)
	
	tool create
	(
		on mousepoint click do
		case click of
		(
			1: 
			(
				nodeTM.translation = worldpoint;
				delegate.boxsize = [10,10,10];
				#stop
			)
		)
	)

	on getDisplayMesh do
	(
		if (displayMesh == undefined) then
		(
			displayMesh = TriMesh();
		)
		--tried to have the c++ part rebuild the mesh, but that didn't work out
		local rebuild = px.getFluidUpdateNr BoundToNode;
		local rebuildOverride = false;
		if (BoundToNode != undefined) then
		(
			if (BoundToNode.transform.position != mRebuildTransform.position) then rebuildOverride = true;
			if (BoundToNode.transform.rotation != mRebuildTransform.rotation) then rebuildOverride = true;
		)
		local nrParticles = px.getFluidParticleCount BoundToNode;
		if (((rebuild != mRebuildNr) or rebuildOverride) and (nrParticles > 0)) then
		(
			local tempMesh = px.pxCreateFluidMesh BoundToNode displayMesh;
			if (tempMesh != undefined) then
			(
				if (displayMesh != tempMesh) then
				(
					displayMesh = tempMesh;
				)
				needRebuild = false;
			)
		)		
		else if ((rebuild != mRebuildNr) or rebuildOverride) then
		(
			mRebuildNr = rebuild;
			if (BoundToNode != undefined) then
			(
				mRebuildTransform = BoundToNode.transform;
			)
			local size = delegate.boxsize;
			local verts = #();
			local faces = #();
			
			append verts [-1, -1, -1]
			append verts [-1, -1, 1]
			append verts [-1, 1, -1]
			append verts [-1, 1, 1]
			append verts [1, -1, -1]
			append verts [1, -1, 1]
			append verts [1, 1, -1]
			append verts [1, 1, 1]
			
			append faces [2, 3, 1]
			append faces [2, 4, 3]
			
			append faces [5, 7, 6]
			append faces [7, 8, 6]
			
			append faces [1, 5, 2]
			append faces [5, 6, 2]
			
			append faces [8, 2, 6]
			append faces [8, 4, 2]
			
			append faces [5, 1, 3]
			append faces [7, 5, 3]

			append faces [3, 8, 7]
			append faces [3, 4, 8]

			setMesh displayMesh vertices: verts faces: faces
			needRebuild = false;
		)
		
		return displayMesh.mesh;
	)

	on attachedToNode n do
	(
		--print ("Attached to: " + (n as string))
		BoundToNode = n;
	)
	
	on detachedFromNode n do
	(
		BoundToNode = undefined;
	)
)