-- 
-- This file contains the rollout code for editing SoftBody properties
--

	rollout softbodyproperties_roll "SoftBody Properties"
	(
		label		lbl_a						"Mesh Settings"
		checkbox	meshtetras_ui				"Mesh is tetrahedra"		checked:true
		checkbox	isosurface_ui				"Create iso surface"
		checkbox	isosurfacesingle_ui			"Single iso surface"
		spinner		subdivisionlevel_ui			"Subdivision lvl"			range:[1,90,20]				type:#integer
		spinner		surfacesimplification_ui	"Surface simpl."			range:[0.0,1.0,0.5]			type:#float

		checkbox	meshtearable_ui				"Mesh Tearable" 			checked:false
		checkbox	sixteenbit_ui				"16 bit indices"			checked:false
		checkbox	vertexcolor_ui				"Vertex color -> tearable"	checked:false
		checkbox	meshhardware_ui				"Hardware"					checked:false
		
		label		lbl_b						""
		label		lbl_c						"SoftBody Properties"
		spinner		compartment_ui				"(Cloth) Compartment nr"	range:[0,15,0]				type:#integer
		spinner		volumestiff_ui				"Volume stiff."				range:[0.0,1.0,1.0]			type:#float
		
		spinner		stretchingstiff_ui			"Stretching stiff."			range:[0.01,1.0,1.0]			type:#float
		spinner		density_ui					"Density"					range:[0.01,999999.0,1.0]	type:#float
		spinner		damping_ui					"Damping"					range:[0.0,1.0,0.5]			type:#float
		spinner		friction_ui					"Friction"					range:[0.0,1.0,0.5]			type:#float
		spinner		particleradius_ui			"Particle radius"			range:[0.0,1.0,0.1]			type:#float
		spinner		solveriterations_ui			"Solver iterations"			range:[1,100,5]				type:#integer
		spinner		tearfactor_ui				"Tear factor"				range:[1.01,10.0,1.5]		type:#float  enabled:false
		spinner		collisionresponsecoeff_ui	"Collision response"		range:[0.0,999999.0,0.2]		type:#float
		spinner		attachmentresponsecoeff_ui	"Attach. response"		    range:[0.0,1.0,0.2]		type:#float
		spinner		attachmenttearfactor_ui		"Att. tear factor"			range:[1.01,10.0,1.5]		type:#float
		spinner		tofluidresponsecoeff_ui		"To fluid response"			range:[0.0,999999.0,1.0]		type:#float
		spinner		fromfluidresponsecoeff_ui	"From fluid resp."		    range:[0.0,999999.0,1.0]		type:#float
		spinner		relativegridspacing_ui		"Relative grid sp."			range:[0.01,10.0,0.25]		type:#float

		checkbox	static_ui					"Static"					checked:false
		checkbox	disable_collision_ui		"Disable collision"			checked:false
		checkbox	selfcollision_ui			"Selfcollision"				checked:false
		checkbox	visualization_ui			"Visualization"				checked:false
		checkbox	gravity_ui					"Gravity"					checked:true
		checkbox	volumeconservation_ui		"Volume conservation"		checked:true
		checkbox	dampingflag_ui				"Damping"					checked:false
		checkbox	twowaycollision_ui			"Two-way collision"			checked:false
		checkbox	tearable_ui					"Cloth Tearable"			checked:false
		checkbox	bodyhardware_ui				"Hardware"					checked:false
		checkbox	comdamping_ui				"Center of mass damp."		checked:false
		checkbox	validbounds_ui				"Valid bounds"				checked:false
		checkbox	fluidcollision_ui			"Fluid collision"			checked:false
	
		function updateEditProperties = 
		(
			twowaycollision_ui.enabled = not disable_collision_ui.checked
			collisionresponsecoeff_ui.enabled = twowaycollision_ui.enabled and twowaycollision_ui.checked
			tearfactor_ui.enabled = tearable_ui.enabled and tearable_ui.checked
			if tearable_ui.checked then meshtearable_ui.checked = true
		
			for n in $selection do
			(
				setUserProp n "px_sb_meshistetra" meshtetras_ui.checked;
				setUserProp n "px_sb_createisosurface" isosurface_ui.checked;
				setUserProp n "px_sb_createsingleisosurface" isosurfacesingle_ui.checked;
				setUserProp n "px_sb_subdivisionlevel" subdivisionlevel_ui.value;
				setUserProp n "px_sb_surfacesimplificationfactor" surfacesimplification_ui.value;
			
				setUserProp n "px_sbmf_tearable" meshtearable_ui.checked;
				setUserProp n "px_sbmf_16bit" sixteenbit_ui.checked;
				setUserProp n "px_sbmp_vertex" vertexcolor_ui.checked;
				setUserProp n "px_sbmf_hardware" meshhardware_ui.checked;
				
				setUserProp n "px_sbp_compartment" compartment_ui.value;
				setUserProp n "px_sbp_volumestiffness" volumestiff_ui.value;
				setUserProp n "px_sbp_stretchingstiffness" stretchingstiff_ui.value;
				setUserProp n "px_sbp_density" density_ui.value;
				setUserProp n "px_sbp_damping" damping_ui.value;
				setUserProp n "px_sbp_friction" friction_ui.value;
				setUserProp n "px_sbp_particleradius" particleradius_ui.value;
				setUserProp n "px_sbp_solveriterations" solveriterations_ui.value;
				setUserProp n "px_sbp_tearfactor" tearfactor_ui.value;
				setUserProp n "px_sbp_collisionresponse" collisionresponsecoeff_ui.value;
				setUserProp n "px_sbp_attachmentresponse" attachmentresponsecoeff_ui.value;
				setUserProp n "px_sbp_attachmenttearfactor" attachmenttearfactor_ui.value;
				setUserProp n "px_sbp_tofluidresponse" tofluidresponsecoeff_ui.value;
				setUserProp n "px_sbp_fromfluidresponse" fromfluidresponsecoeff_ui.value;
				setUserProp n "px_sbp_relativegridspacing" relativegridspacing_ui.value;
				
				setUserProp n "px_sbf_static" static_ui.checked;
				setUserProp n "px_sbf_disablecollision" disable_collision_ui.checked;
				setUserProp n "px_sbf_selfcollision" selfcollision_ui.checked;
				setUserProp n "px_sbf_visualization" visualization_ui.checked;
				setUserProp n "px_sbf_gravity" gravity_ui.checked;
				setUserProp n "px_sbf_volumeconservation" volumeconservation_ui.checked;
				setUserProp n "px_sbf_damping" dampingflag_ui.checked;
				setUserProp n "px_sbf_twowaycollision" twowaycollision_ui.checked;
				setUserProp n "px_sbf_tearable" tearable_ui.checked;
				setUserProp n "px_sbf_hardware" bodyhardware_ui.checked;
				setUserProp n "px_sbf_comdamping" comdamping_ui.checked;
				setUserProp n "px_sbf_validbounds" validbounds_ui.checked;
				setUserProp n "px_sbf_fluidcollision" fluidcollision_ui.checked;
			)
		)

		function enableControls state = 
		(
			local enableControls = not meshtetras_ui.checked;

			isosurface_ui.enabled = enableControls;
			isosurfacesingle_ui.enabled = enableControls;
			subdivisionlevel_ui.enabled = enableControls;
			surfacesimplification_ui.enabled = enableControls;

			-- not implemented
			vertexcolor_ui.enabled = false;
			vertexcolor_ui.checked = false;
			
			meshhardware_ui.enabled = state and px.hwAvailable();
			bodyhardware_ui.enabled = state and px.hwAvailable();
		)

		on meshtetras_ui changed val do
		(
			enableControls true; -- don't need the parameter for this class, but need to send something
		)

		function updateSelection &props = 
		( 
			local differences = false;
			for n in $selection do
			(
				if (checkBoolProperty n &props "px_sb_meshistetra" true) then differences = true;
				if (checkBoolProperty n &props "px_sb_createisosurface" true) then differences = true;
				if (checkBoolProperty n &props "px_sb_createsingleisosurface" false) then differences = true;
				if (checkIntegerProperty n &props "px_sb_subdivisionlevel" 20) then differences = true;
				if (checkFloatProperty n &props "px_sb_surfacesimplificationfactor" 0.5) then differences = true;
			
				if (checkBoolProperty n &props "px_sbmf_tearable" false) then differences = true;
				if (checkBoolProperty n &props "px_sbmf_16bit" false) then differences = true;
				if (checkBoolProperty n &props "px_sbmp_vertex" false) then differences = true;
				if (checkBoolProperty n &props "px_sbmf_hardware" false) then differences = true;

				if (checkIntegerProperty n &props "px_sbp_compartment" 0) then differences = true;
				if (checkFloatProperty n &props "px_sbp_volumestiffness" 1.0) then differences = true;
				if (checkFloatProperty n &props "px_sbp_stretchingstiffness" 1.0) then differences = true;
				if (checkFloatProperty n &props "px_sbp_density" 1.0) then differences = true;
				if (checkFloatProperty n &props "px_sbp_damping" 0.5) then differences = true;
				if (checkFloatProperty n &props "px_sbp_friction" 0.5) then differences = true;
				if (checkFloatProperty n &props "px_sbp_particleradius" 0.1) then differences = true;
				if (checkIntegerProperty n &props "px_sbp_solveriterations" 5) then differences = true;
				if (checkFloatProperty n &props "px_sbp_tearfactor" 1.5) then differences = true;
				if (checkFloatProperty n &props "px_sbp_collisionresponse" 0.2) then differences = true;
				if (checkFloatProperty n &props "px_sbp_attachmentresponse" 0.2) then differences = true;
				if (checkFloatProperty n &props "px_sbp_attachmenttearfactor" 1.5) then differences = true;
				if (checkFloatProperty n &props "px_sbp_tofluidresponse" 1.0) then differences = true;
				if (checkFloatProperty n &props "px_sbp_fromfluidresponse" 1.0) then differences = true;
				if (checkFloatProperty n &props "px_sbp_relativegridspacing" 0.25) then differences = true;

				if (checkBoolProperty n &props "px_sbf_static" false) then differences = true;
				if (checkBoolProperty n &props "px_sbf_disablecollision" false) then differences = true;
				if (checkBoolProperty n &props "px_sbf_selfcollision" false) then differences = true;
				if (checkBoolProperty n &props "px_sbf_visualization" false) then differences = true;
				if (checkBoolProperty n &props "px_sbf_gravity" true) then differences = true;
				if (checkBoolProperty n &props "px_sbf_volumeconservation" true) then differences = true;
				if (checkBoolProperty n &props "px_sbf_damping" false) then differences = true;
				if (checkBoolProperty n &props "px_sbf_twowaycollision" false) then differences = true;
				if (checkBoolProperty n &props "px_sbf_tearable" false) then differences = true;
				if (checkBoolProperty n &props "px_sbf_hardware" false) then differences = true;
				if (checkBoolProperty n &props "px_sbf_comdamping" false) then differences = true;
				if (checkBoolProperty n &props "px_sbf_validbounds" false) then differences = true;
				if (checkBoolProperty n &props "px_sbf_fluidcollision" false) then differences = true;
			)
			return differences; 
		)

		function getValueAsBool val defaultValue = 
		(
			return try (
				val as BooleanClass
			) catch
			(
				try
				(
					local tmp = val as integer;
					tmp != 0;
				) catch(defaultValue)
			);
		)
		
		function getValueAsFloat val defaultValue =
		(
			return try (
				val as float;
			) catch(defaultValue);
		)

		function getValueAsInt val defaultValue =
		(
			return try (
				val as integer;
			) catch(defaultValue);
		)


		function updateUIWithReadValues &props = 
		(
			meshtetras_ui.checked				= getValueAsBool (pxMap_getValue &props "px_sb_meshistetra" true) true;
			isosurface_ui.checked				= getValueAsBool (pxMap_getValue &props "px_sb_createisosurface" true) true;
			isosurfacesingle_ui.checked			= getValueAsBool (pxMap_getValue &props "px_sb_createsingleisosurface" false) false;
			subdivisionlevel_ui.value			= getValueAsInt (pxMap_getValue &props "px_sb_subdivisionlevel" 20) 20;
			surfacesimplification_ui.value		= getValueAsFloat (pxMap_getValue &props "px_sb_surfacesimplificationfactor" 0.5) 0.5;

			meshtearable_ui.checked				= getValueAsBool (pxMap_getValue &props "px_sbmf_tearable" false) false;
			sixteenbit_ui.checked				= getValueAsBool (pxMap_getValue &props "px_sbmf_16bit" false) false;
			vertexcolor_ui.checked				= getValueAsBool (pxMap_getValue &props "px_sbmp_vertex" false) false;
			meshhardware_ui.checked					= getValueAsBool (pxMap_getValue &props "px_sbmf_hardware" false) false;

			compartment_ui.value				= getValueAsInt (pxMap_getValue &props "px_sbp_compartment" 0) 0;
			volumestiff_ui.value				= getValueAsFloat (pxMap_getValue &props "px_sbp_volumestiffness" 1.0) 1.0;
			stretchingstiff_ui.value			= getValueAsFloat (pxMap_getValue &props "px_sbp_stretchingstiffness" 1.0) 1.0;
			density_ui.value					= getValueAsFloat (pxMap_getValue &props "px_sbp_density" 1.0) 1.0;
			damping_ui.value					= getValueAsFloat (pxMap_getValue &props "px_sbp_damping" 0.5) 0.5;
			friction_ui.value					= getValueAsFloat (pxMap_getValue &props "px_sbp_friction" 0.5) 0.5;
			particleradius_ui.value				= getValueAsFloat (pxMap_getValue &props "px_sbp_particleradius" 0.1) 0.1;
			solveriterations_ui.value			= getValueAsInt (pxMap_getValue &props "px_sbp_solveriterations" 5) 5;
			tearfactor_ui.value					= getValueAsFloat (pxMap_getValue &props "px_sbp_tearfactor" 1.5) 1.5;
			collisionresponsecoeff_ui.value		= getValueAsFloat (pxMap_getValue &props "px_sbp_collisionresponse" 0.2) 0.2;
			attachmentresponsecoeff_ui.value	= getValueAsFloat (pxMap_getValue &props "px_sbp_attachmentresponse" 0.2) 0.2;
			attachmenttearfactor_ui.value		= getValueAsFloat (pxMap_getValue &props "px_sbp_attachmenttearfactor" 1.5) 1.5;
			tofluidresponsecoeff_ui.value		= getValueAsFloat (pxMap_getValue &props "px_sbp_tofluidresponse" 1.0) 1.0;
			fromfluidresponsecoeff_ui.value		= getValueAsFloat (pxMap_getValue &props "px_sbp_fromfluidresponse" 1.0) 1.0;
			relativegridspacing_ui.value		= getValueAsFloat (pxMap_getValue &props "px_sbp_relativegridspacing" 0.25) 0.25;

			static_ui.checked					= getValueAsBool (pxMap_getValue &props "px_sbf_static" false) false;
			disable_collision_ui.checked		= getValueAsBool (pxMap_getValue &props "px_sbf_disablecollision" false) false;
			selfcollision_ui.checked			= getValueAsBool (pxMap_getValue &props "px_sbf_selfcollision" false) false;
			visualization_ui.checked			= getValueAsBool (pxMap_getValue &props "px_sbf_visualization" false) false;
			gravity_ui.checked					= getValueAsBool (pxMap_getValue &props "px_sbf_gravity" true) true;
			volumeconservation_ui.checked		= getValueAsBool (pxMap_getValue &props "px_sbf_volumeconservation" true) true;
			dampingflag_ui.checked					= getValueAsBool (pxMap_getValue &props "px_sbf_damping" false) false;
			twowaycollision_ui.checked			= getValueAsBool (pxMap_getValue &props "px_sbf_twowaycollision" false) false;
			tearable_ui.checked					= getValueAsBool (pxMap_getValue &props "px_sbf_tearable" false) false;
			bodyhardware_ui.checked				= getValueAsBool (pxMap_getValue &props "px_sbf_hardware" false) false;
			comdamping_ui.checked				= getValueAsBool (pxMap_getValue &props "px_sbf_comdamping" false) false;
			validbounds_ui.checked				= getValueAsBool (pxMap_getValue &props "px_sbf_validbounds" false) false;
			fluidcollision_ui.checked			= getValueAsBool (pxMap_getValue &props "px_sbf_fluidcollision" false) false;
		)
		
		on softbodyproperties_roll open do
		(
			updateUIWithReadValues px_control.mainui_roll.mPhysXPropertiesOfFirst;
			enableControls true;
		)
		
		on meshtetras_ui changed val do updateEditProperties();
		on isosurface_ui changed val do updateEditProperties();
		on isosurfacesingle_ui changed val do updateEditProperties();
		on subdivisionlevel_ui changed val do updateEditProperties();
		on surfacesimplification_ui changed val do updateEditProperties();
		
		on meshtearable_ui changed val do updateEditProperties();
		on sixteenbit_ui changed val do updateEditProperties();
		on vertexcolor_ui changed val do updateEditProperties();
		on meshhardware_ui changed val do updateEditProperties();
		
		on compartment_ui changed val do updateEditProperties();
		on volumestiff_ui changed val do updateEditProperties();
		on stretchingstiff_ui changed val do updateEditProperties();
		on density_ui changed val do updateEditProperties();
		on damping_ui changed val do updateEditProperties();
		on friction_ui changed val do updateEditProperties();
		on particleradius_ui changed val do updateEditProperties();
		on solveriterations_ui changed val do updateEditProperties();
		on tearfactor_ui changed val do updateEditProperties();
		on collisionresponsecoeff_ui changed val do updateEditProperties();
		on attachmentresponsecoeff_ui changed val do updateEditProperties();
		on attachmenttearfactor_ui changed val do updateEditProperties();
		on tofluidresponsecoeff_ui changed val do updateEditProperties();
		on fromfluidresponsecoeff_ui changed val do updateEditProperties();
		on relativegridspacing_ui changed val do updateEditProperties();
		
		on static_ui changed val do updateEditProperties();
		on disable_collision_ui changed val do updateEditProperties();
		
		on selfcollision_ui changed val do updateEditProperties();
		on visualization_ui changed val do updateEditProperties();
		on gravity_ui changed val do updateEditProperties();
		on volumeconservation_ui changed val do updateEditProperties();
		on dampingflag_ui changed val do updateEditProperties();
		on twowaycollision_ui changed val do updateEditProperties();
		on tearable_ui changed val do updateEditProperties();
		on bodyhardware_ui changed val do updateEditProperties();
		on comdamping_ui changed val do updateEditProperties();
		on validbounds_ui changed val do updateEditProperties();
		on fluidcollision_ui changed val do updateEditProperties();
	)

