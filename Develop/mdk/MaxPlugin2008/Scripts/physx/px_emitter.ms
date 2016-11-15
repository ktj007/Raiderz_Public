plugin helper PxEmitter
name:"PxEmitter"
category:"NVIDIA PhysX"
classID:#(0xe2757b00, 0x92b46400)
extends:dummy replaceui:true
(	
    -- Note! For some reason the last two digits of the classid are truncated to 00 by 3ds max, so in order
    --       to provide the same classid in maxscript as in the c++ part of the plugin, the last two digits
    --       should be kept as is.
	local displayMesh = undefined;
	local meshBuildRadius = 0.0;
	local meshBuildType = 0;

	parameters main_p rollout:main_r
	(
		EmitterType				type:#index		ui:emittertype_ui			default:2
		EmitterShape			type:#index		ui:emittershape_ui			default:1

		EmitterFlagsEnabled			type:#boolean	ui:emitterflags_enabled			default:true
		EmitterFlagsForceOnBody		type:#boolean	ui:emitterflags_forceonbody		default:false
		EmitterFlagsAddBodyVelocity	type:#boolean	ui:emitterflags_addbodyvelocity	default:true
		EmitterFlagsVisualization	type:#boolean	ui:emitterflags_visualization	default:true

		FrameShape				type:#node      ui:frameshape_ui
		Fluid					type:#node		ui:fluid_ui

		MaxParticles			type:#integer	ui:maxparticles_ui				default:1000

		DimensionX				type:#float		ui:dimensionx_ui				default:0.25
		DimensionY				type:#float		ui:dimensiony_ui				default:0.25

		RandomPos				type:#point3									default:[0,0,0]
		RandomAngle				type:#float		ui:randomangle_ui				default:0

		FluidVelocityMagnitude	type:#float		ui:velocitymagnitude_ui			default:1

		Rate					type:#float		ui:rate_ui						default:100

		ParticleLifetime		type:#float		ui:particlelifetime_ui			default:0

		RepulsionCoefficient	type:#float		ui:repulsioncoefficient_ui		default:1
	)
	
	--checks so that the user is picking a fluid object
	function checkFluid n =
	(
		return (classOf(n) as string) == "PxFluid";
	)
	
	--checks so that the user is picking an actor and/or shape
	function checkActorShape n =
	(
		--return true;
		--return px_isshape(n);
		if (superclassof n) != GeometryClass then return false
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
			return type < PX_PHYSTYPE_RB_OVER;
		)
		return false
	)
	
	rollout main_r "Emitter Params"
	(
		label			fluid_lb						"Fluid:"		width: 60					align:#left
		pickbutton    	fluid_ui      					"undefined"		pos:[80, 2] width: 60		filter:checkFluid		message:""	toolTip:"Pick a fluid for which this emitter should emit particles"	autoDisplay: true
	    button		    removefluid_ui					"x"				pos:[145, 2]				toolTip:"Remove Fluid"

		label			frameshape_lb					"Frame ref:"	width: 60	align:#left
		pickbutton    	frameshape_ui      				"undefined"		pos:[80, 25] width: 60		filter:checkActorShape	message:""	toolTip:"Pick a shape (in a PhysX actor) to use as reference for the direction of the fluid"	autoDisplay: true	
	    button		    removeframe_ui					"x"				pos:[145, 25]				toolTip:"Remove Frame Ref."

		label			emittertype_lb					"Emitter Type"		align:#left
    	dropdownlist	emittertype_ui					""					items:#("Constant Pressure","Constant Flow Rate")

		label			emittershape_lb					"Shape"				align:#left
    	dropdownlist	emittershape_ui					""					items:#("Rectangular","Elliptical")		pos:[55,92]		width:95

		label			dimension_lb					"Dimension"			align:#left
		label			dimensionx_lb					"X"															pos:[13,140]
		spinner			dimensionx_ui					""					range:[0,9999999,1]		type:#float		pos:[22,139]	width:60
		label			dimensiony_lb					"Y"															pos:[90,140]
		spinner			dimensiony_ui					""					range:[0,9999999,1]		type:#float		pos:[99,139]	width:60

		label			emitterflags_lb					"Emitter Flags"												pos:[13,170]
		checkbox		emitterflags_enabled			"Enabled"
		checkbox		emitterflags_forceonbody		"Force On Body"
		checkbox		emitterflags_addbodyvelocity	"Add Body Velocity"
		checkbox		emitterflags_visualization		"Visualization"

		label			maxparticles_lb					"Max Particles: "											pos:[13,280]
		spinner			maxparticles_ui					""					range:[0,32767,1]		type:#integer	pos:[85,280]

		label			velocitymagnitude_lb			"Fluid Velocity"	align:#left
		spinner			velocitymagnitude_ui			""					range:[0,9999999,1]		type:#float		pos:[85,301]

		label			rate_lb							"Rate"				align:#left
		spinner			rate_ui							""					range:[0,9999999,1]		type:#float		pos:[85,322] enabled:false

		label			particlelifetime_lb				"Particle Lifetime"	align:#left
		spinner			particlelifetime_ui				""					range:[0,9999999,1]		type:#float		pos:[85,343]

		label			repulsioncoefficient_lb			"Repulsion Coefficient"	align:#left
		spinner			repulsioncoefficient_ui			""					range:[0,9999999,1]		type:#float		pos:[85,382]

		label			randompos_lb					"Random Position"	align:#left
		label			randomposx_lb					"x"															pos:[5,420]
		label			randomposy_lb					"y"															pos:[55,420]
		label			randomposz_lb					"z"															pos:[105,420]
		spinner			randomposx_ui					""					range:[0,9999999,1.0]	type:#float		pos:[12,420]	width:40
		spinner			randomposy_ui					""					range:[0,9999999,1.0]	type:#float		pos:[62,420]	width:40
		spinner			randomposz_ui					""					range:[0,9999999,1.0]	type:#float		pos:[112,420]	width:40

		label			randomangle_lb					"Random Angle"												pos:[13,445]
		spinner			randomangle_ui					""					range:[0,360,1]			type:#float		pos:[85,444]

		
		fn updateStates =
		(
			rate_ui.enabled = emittertype_ui.selection == 2
		)
		
		on emittertype_ui selected selIndex do
		(
			updateStates()
		)
		
		on removefluid_ui pressed do
		(
			fluid_ui.object = undefined;
		)

		on removeframe_ui pressed do
		(
			frameshape_ui.object = undefined;
		)

		on main_r open do 
		(
			randomposx_ui.value = RandomPos.x;
			randomposy_ui.value = RandomPos.y;
			randomposz_ui.value = RandomPos.z;
			
			updateStates()
		)
		
		on randomposx_ui changed val do
		(
			RandomPos.x = val
		)

		on randomposy_ui changed val do
		(
			RandomPos.y = val
		)

		on randomposz_ui changed val do
		(
			RandomPos.z = val
		)
	)
	
	rollout cr "About"
	(
		label c1 "PhysX Fluid Emitter"
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
		local rebuild = false;
		if (EmitterShape != meshBuildType or RandomAngle != meshBuildAngle) then
		(
			--displayMesh = undefined;
			rebuild = true;
		)
		meshBuildType = EmitterShape;
		meshBuildAngle = RandomAngle;
		if (rebuild) do 
		(
			local size = delegate.boxsize;
			local verts = #();
			local faces = #();

			local ellipse = [DimensionX, DimensionY, 1]
			
			--bottom faces
			append verts [0, 0, 0]
			local i = 0;
			local numVerts = 18;
			local angleMultiplicator = 20.0;
			local angleSub = 0.0;
			if (EmitterShape == 1) then
			( --rectangle
				numVerts = 4;
				angleMultiplicator = 90.0;
				angleSub = 45;
			)
			for i = 1 to numVerts do
			(
				local angle = (i-1)*angleMultiplicator - angleSub
				append verts ([cos(angle),sin(angle),0]*ellipse)
				append faces [1, i, i+1]
			)
			append faces [1,verts.count,2]
			
			--top faces
			local sizer = 1
			if (meshBuildAngle < 89) then
			(
				sizer = sizer + tan(RandomAngle)
			)
			local zPos = 1*FluidVelocityMagnitude/size.z
			append verts [0, 0, zPos]
			local base = verts.count
			for i = 1 to numVerts do
			(
				--local angle = (i-1)*10.0
				local angle = (i-1)*angleMultiplicator - angleSub
				append verts ([cos(angle),sin(angle),zPos]*ellipse*sizer)
				append faces [base, base+i-1, base+i]
			)
			append faces [base,verts.count,base+1]
			
			--side faces
			for i = 1 to numVerts-1 do
			(
			append faces [1+i, 1+i+1, 2+i+numVerts]
			append faces [1+i+1, 2+i+numVerts+1, 2+i+numVerts]
			)
			append faces [1+numVerts, 2, 2+2*numVerts]
			append faces [2, 2+numVerts+1, 2+2*numVerts]

			--scale it to the correct size
			local scaledVerts = #();
			local i = 1;
			for i = 1 to verts.count do
			(
				append scaledVerts (verts[i]*size)
			)
			setMesh displayMesh vertices: scaledVerts faces: faces
		)
		return displayMesh.mesh;
	)
	
--	on attachedToNode n do 
--	(
		--setUserProp
		--PX_PHYSTYPE_CLOTH
--	)
)