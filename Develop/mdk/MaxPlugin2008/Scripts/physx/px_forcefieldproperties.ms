-- 
-- This file contains the rollout code for editing Force Field properties
--
	--can't get these values to stay in scope when they are local, they become empty lists all of a sudden, making them global for now
	--global this_ff_object = #();
	

		function isRigidBody n = 
		(
			local type = try(getuserprop n "PhysicsType" as integer) catch(PX_PHYSTYPE_UNDEFINED);
			-- print (format "object type = %\n" type);
			if(type < PX_PHYSTYPE_RB_OVER) then
			(
				return true;
			)
			return false;
		)

	rollout ForceFieldProperties_Roll "ForceField Properties"
	(
		group	              "ForceField Flags:"	     -- align:#left offset:[-8, 0]
		(
			checkbox      ui_flag_vol_rigidbody   "Vol. Scaling RB "                  align:#left enable:false
			checkbox      ui_flag_vol_fluid       "Vol. Scaling Fluid "               align:#left enable:false
			checkbox      ui_flag_vol_cloth       "Vol. Scaling Cloth "               align:#left enable:false
			checkbox      ui_flag_vol_softbody    "Vol. Scaling SoftBody "            align:#left enable:false
		)
		
		label         ui_lbl_g                "Frame Actor"       align:#left  across: 2 offset:[-8, 0]
        pickbutton    ui_frameactor           "undefined"         offset:[-15, 0] width: 60   filter:isRigidBody message:"" toolTip:"Specify Actor that the ForceField moves with." autoDisplay: true
        button        ui_removeframeactor     "x"                 width: 15   pos:[135,112]   toolTip:"Remove Frame Actor"

		label         ui_lbl_h                "Coord. Space"      align:#left  width: 50 across: 2 offset:[-8, 0]
		dropdownlist  ui_coordinate_space     ""	              width: 80 offset:[-10, 0] items:#("NX_FFC_CARTESIAN", "NX_FFC_SPHERICAL", "NX_FFC_CYLINDRICAL", "NX_FFC_TOROIDAL") selection: 1

		group	              "Interaction with others:"  -- align:#left offset:[-8, 0]
		(
			label         ui_lbl_c                "RigidBody"       align:#left  width: 50 across: 2 
			dropdownlist  ui_interaction_rigidbody       ""	        width: 80 offset:[-10, 0] items:#("Dummy_0", "Dummy 1", "Gravitational", "Other", "No Interaction") selection: 4
			label         ui_lbl_d                "Cloth"           align:#left  width: 50 across: 2 
			dropdownlist  ui_interaction_cloth           ""	        width: 80 offset:[-10, 0] items:#("Dummy_0", "Dummy 1", "Gravitational", "Other", "No Interaction") selection: 4
			label         ui_lbl_e                "SoftBody"        align:#left  width: 50 across: 2 
			dropdownlist  ui_interaction_softbody        ""	        width: 80 offset:[-10, 0] items:#("Dummy_0", "Dummy 1", "Gravitational", "Other", "No Interaction") selection: 4
			label         ui_lbl_f                "Fluids"          align:#left  width: 50 across: 2 
			dropdownlist  ui_interaction_fluid           ""	        width: 80 offset:[-10, 0] items:#("Dummy_0", "Dummy 1", "Gravitational", "Other", "No Interaction") selection: 4
		)
		
		group "Linear Kernel Properties"
		(
			label      lk_contant_force_ui       "Constant Force"    align:#left offset:[-5, 0]
			spinner    lk_contant_force_x        "x"           range:[-100000,100000,0] align:#right   width:46 across:3
			spinner    lk_contant_force_y        "y"           range:[-100000,100000,0] align:#right   width:46
			spinner    lk_contant_force_z        "z"           range:[-100000,100000,0] align:#right   width:46
			
			label      lk_position_target_ui     "Position Target"    align:#left offset:[-5, 0]
			spinner    lk_position_target_x      "x"           range:[-100000,100000,0] align:#right   width:46 across:3
			spinner    lk_position_target_y      "y"           range:[-100000,100000,0] align:#right   width:46
			spinner    lk_position_target_z      "z"           range:[-100000,100000,0] align:#right   width:46
			
			label      lk_velocity_target_ui     "Velocity Target"    align:#left offset:[-5, 0]
			spinner    lk_velocity_target_x      "x"           range:[-100000,100000,0] align:#right   width:46 across:3
			spinner    lk_velocity_target_y      "y"           range:[-100000,100000,0] align:#right   width:46
			spinner    lk_velocity_target_z      "z"           range:[-100000,100000,0] align:#right   width:46
			
			label      lk_falloff_linear_ui      "Falloff Linear"    align:#left offset:[-5, 0]
			spinner    lk_falloff_linear_x       "x"           range:[0,100000,0] align:#right   width:46 across:3
			spinner    lk_falloff_linear_y       "y"           range:[0,100000,0] align:#right   width:46
			spinner    lk_falloff_linear_z       "z"           range:[0,100000,0] align:#right   width:46
			
			label      lk_falloff_quadratic_ui      "Falloff Quadratic"    align:#left offset:[-5, 0]
			spinner    lk_falloff_quadratic_x       "x"           range:[0,100000,0] align:#right   width:46 across:3
			spinner    lk_falloff_quadratic_y       "y"           range:[0,100000,0] align:#right   width:46
			spinner    lk_falloff_quadratic_z       "z"           range:[0,100000,0] align:#right   width:46
			
			label      lk_noise_ui      "Noise"    align:#left offset:[-5, 0]
			spinner    lk_noise_x       "x"           range:[-100000,100000,0] align:#right   width:46 across:3
			spinner    lk_noise_y       "y"           range:[-100000,100000,0] align:#right   width:46
			spinner    lk_noise_z       "z"           range:[-100000,100000,0] align:#right   width:46
		)
		

		---- ui layout over ----
		
		on ui_removeframeactor pressed do
		(
			ui_frameactor.object = undefined;
			setuserprop $ "px_ff_frame_actor" "undefined";
		)
		
		function updateEditProperties = 
		(
			--print "updateEditProperties /n";
			for n in $selection do
			(
				setUserProp n "px_fff_volumetric_scaling_rigidbody"  ui_flag_vol_rigidbody.checked;
				setUserProp n "px_fff_volumetric_scaling_fluid"      ui_flag_vol_fluid.checked;
				setUserProp n "px_fff_volumetric_scaling_cloth"      ui_flag_vol_cloth.checked;  
				setUserProp n "px_fff_volumetric_scaling_softbody"   ui_flag_vol_softbody.checked;
				
				setUserProp n "px_coordinate_space"                  ui_coordinate_space.selection; 
				
				setUserProp n "px_interaction_rigidbody"             ui_interaction_rigidbody.selection;
				setUserProp n "px_interaction_fluid"                 ui_interaction_fluid.selection;
				setUserProp n "px_interaction_cloth"                 ui_interaction_cloth.selection;
				setUserProp n "px_interaction_softbody"              ui_interaction_softbody.selection;
				
				setUserProp n "px_lk_contant_force_x"                lk_contant_force_x.value;
				setUserProp n "px_lk_contant_force_y"                lk_contant_force_y.value;
				setUserProp n "px_lk_contant_force_z"                lk_contant_force_z.value;

				setUserProp n "px_lk_position_target_x"                lk_position_target_x.value;
				setUserProp n "px_lk_position_target_y"                lk_position_target_y.value;
				setUserProp n "px_lk_position_target_z"                lk_position_target_z.value;

				setUserProp n "px_lk_velocity_target_x"                lk_velocity_target_x.value;
				setUserProp n "px_lk_velocity_target_y"                lk_velocity_target_y.value;
				setUserProp n "px_lk_velocity_target_z"                lk_velocity_target_z.value;

				setUserProp n "px_lk_falloff_linear_x"                 lk_falloff_linear_x.value;
				setUserProp n "px_lk_falloff_linear_y"                 lk_falloff_linear_y.value;
				setUserProp n "px_lk_falloff_linear_z"                 lk_falloff_linear_z.value;

				setUserProp n "px_lk_falloff_quadratic_x"              lk_falloff_quadratic_x.value;
				setUserProp n "px_lk_falloff_quadratic_y"              lk_falloff_quadratic_y.value;
				setUserProp n "px_lk_falloff_quadratic_z"              lk_falloff_quadratic_z.value;

				setUserProp n "px_lk_noise_x"                          lk_noise_x.value;
				setUserProp n "px_lk_noise_y"                          lk_noise_y.value;
				setUserProp n "px_lk_noise_z"                          lk_noise_z.value;
			)
		)

		function updateSelection &props = 
		( 
			--print "updateSelection /n";
			local differences = false;
			for n in $selection do
			(
				if (checkBoolProperty n &props "px_fff_volumetric_scaling_rigidbody" false) then differences = true;
				if (checkBoolProperty n &props "px_fff_volumetric_scaling_fluid" false) then differences = true;
				if (checkBoolProperty n &props "px_fff_volumetric_scaling_cloth" false) then differences = true;
				if (checkBoolProperty n &props "px_fff_volumetric_scaling_softbody" false) then differences = true;
				
				if (checkStringProperty n &props "px_ff_frame_actor" "") then differences = true;
				
				if (checkIntegerProperty n &props "px_coordinate_space" 1) then differences = true;
				
				if (checkIntegerProperty n &props "px_interaction_rigidbody" 4) then differences = true;
				if (checkIntegerProperty n &props "px_interaction_fluid" 4) then differences = true;
				if (checkIntegerProperty n &props "px_interaction_cloth" 4) then differences = true;
				if (checkIntegerProperty n &props "px_interaction_softbody" 4) then differences = true;
				
				if (checkFloatProperty n &props "px_lk_contant_force_x" 0.0) then differences = true;
				if (checkFloatProperty n &props "px_lk_contant_force_y" 0.0) then differences = true;
				if (checkFloatProperty n &props "px_lk_contant_force_z" 0.0) then differences = true;
				
				if (checkFloatProperty n &props "px_lk_position_target_x" 0.0) then differences = true;
				if (checkFloatProperty n &props "px_lk_position_target_y" 0.0) then differences = true;
				if (checkFloatProperty n &props "px_lk_position_target_z" 0.0) then differences = true;

				if (checkFloatProperty n &props "px_lk_velocity_target_x" 0.0) then differences = true;
				if (checkFloatProperty n &props "px_lk_velocity_target_y" 0.0) then differences = true;
				if (checkFloatProperty n &props "px_lk_velocity_target_z" 0.0) then differences = true;

				if (checkFloatProperty n &props "px_lk_falloff_linear_x" 0.0) then differences = true;
				if (checkFloatProperty n &props "px_lk_falloff_linear_y" 0.0) then differences = true;
				if (checkFloatProperty n &props "px_lk_falloff_linear_z" 0.0) then differences = true;

				if (checkFloatProperty n &props "px_lk_falloff_quadratic_x" 0.0) then differences = true;
				if (checkFloatProperty n &props "px_lk_falloff_quadratic_y" 0.0) then differences = true;
				if (checkFloatProperty n &props "px_lk_falloff_quadratic_z" 0.0) then differences = true;

				if (checkFloatProperty n &props "px_lk_noise_x" 0.0) then differences = true;
				if (checkFloatProperty n &props "px_lk_noise_y" 0.0) then differences = true;
				if (checkFloatProperty n &props "px_lk_noise_z" 0.0) then differences = true;
			)
			return differences;
		)
		
		function enableControls state = 
		(
			--print "enableControls /n";
			
		)

		function updateUIWithReadValues &props = 
		(
			--print "updateUIWithReadValues /n";
			ui_flag_vol_rigidbody.checked          = getValueAsBool (pxMap_getValue &props "px_fff_volumetric_scaling_rigidbody" false) false;
			ui_flag_vol_fluid.checked              = getValueAsBool (pxMap_getValue &props "px_fff_volumetric_scaling_fluid" false) false;
			ui_flag_vol_cloth.checked              = getValueAsBool (pxMap_getValue &props "px_fff_volumetric_scaling_cloth" false) false;
			ui_flag_vol_softbody.checked           = getValueAsBool (pxMap_getValue &props "px_fff_volumetric_scaling_softbody" false) false;
			
			ui_coordinate_space.selection          = getValueAsInt (pxMap_getValue &props "px_coordinate_space" 1) 1;
			
			ui_interaction_rigidbody.selection     = getValueAsInt (pxMap_getValue &props "px_interaction_rigidbody" 4) 4;
			ui_interaction_fluid.selection         = getValueAsInt (pxMap_getValue &props "px_interaction_fluid" 4) 4;
			ui_interaction_cloth.selection         = getValueAsInt (pxMap_getValue &props "px_interaction_cloth" 4) 4;
			ui_interaction_softbody.selection      = getValueAsInt (pxMap_getValue &props "px_interaction_softbody" 4) 4;
			
			lk_contant_force_x.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_contant_force_x" 0.0) 0.0;
			lk_contant_force_y.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_contant_force_y" 0.0) 0.0;
			lk_contant_force_z.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_contant_force_z" 0.0) 0.0;

			lk_position_target_x.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_position_target_x" 0.0) 0.0;
			lk_position_target_y.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_position_target_y" 0.0) 0.0;
			lk_position_target_z.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_position_target_z" 0.0) 0.0;

			lk_velocity_target_x.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_velocity_target_x" 0.0) 0.0;
			lk_velocity_target_y.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_velocity_target_y" 0.0) 0.0;
			lk_velocity_target_z.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_velocity_target_z" 0.0) 0.0;

			lk_falloff_linear_x.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_falloff_linear_x" 0.0) 0.0;
			lk_falloff_linear_y.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_falloff_linear_y" 0.0) 0.0;
			lk_falloff_linear_z.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_falloff_linear_z" 0.0) 0.0;

			lk_falloff_quadratic_x.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_falloff_quadratic_x" 0.0) 0.0;
			lk_falloff_quadratic_y.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_falloff_quadratic_y" 0.0) 0.0;
			lk_falloff_quadratic_z.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_falloff_quadratic_z" 0.0) 0.0;

			lk_noise_x.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_noise_x" 0.0) 0.0;
			lk_noise_y.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_noise_y" 0.0) 0.0;
			lk_noise_z.value                    = getValueAsFloat (pxMap_getValue &props "px_lk_noise_z" 0.0) 0.0;
			
			local pg = pxMap_getValue &props "px_ff_frame_actor" undefined;
			if (pg != undefined and pg != "") then
			(
				pobj = findobject pg;
				if (pobj != undefined) then
				(
					--ui_frameactor.text = pg
					ui_frameactor.object = pobj;
				)
			)
		)

		on ui_frameactor picked obj do
		(
			if (obj != undefined) then
			(
				local actor = px_getPxActor obj;
				if (actor != null) then
				(
					local type = try(getuserprop actor "PhysicsType" as integer) catch(PX_PHYSTYPE_UNDEFINED);
					-- print (format "object type = %\n" type);
					if(type < PX_PHYSTYPE_RB_OVER) then
					(
						ui_frameactor.object = actor;
						setuserprop $ "px_ff_frame_actor" actor.name;
						return 0;
					)
					MessageBox "You can only choose a rigid object!"
				)
			)
			ui_frameactor.object = undefined;
			setuserprop $ "px_ff_frame_actor" "";
		)

		on ForceFieldProperties_Roll open do
		(
			updateUIWithReadValues px_control.mainui_roll.mPhysXPropertiesOfFirst;
			-- enableControls true;
		)
		
		on ui_flag_vol_rigidbody changed val do updateEditProperties();
		on ui_flag_vol_fluid changed val do updateEditProperties();
		on ui_flag_vol_clothbody changed val do updateEditProperties();
		on ui_flag_vol_softbody changed val do updateEditProperties();

		on ui_coordinate_space selected selIndex do updateEditProperties();
		
		on ui_interaction_rigidbody selected selIndex do updateEditProperties();
		on ui_interaction_fluid selected selIndex do updateEditProperties();
		on ui_interaction_cloth selected selIndex do updateEditProperties();
		on ui_interaction_softbody selected selIndex do updateEditProperties();

		on lk_contant_force_x changed val do updateEditProperties();
		on lk_contant_force_y changed val do updateEditProperties();
		on lk_contant_force_z changed val do updateEditProperties();

		on lk_position_target_x changed val do updateEditProperties();
		on lk_position_target_y changed val do updateEditProperties();
		on lk_position_target_z changed val do updateEditProperties();

		on lk_velocity_target_x changed val do updateEditProperties();
		on lk_velocity_target_y changed val do updateEditProperties();
		on lk_velocity_target_z changed val do updateEditProperties();

		on lk_falloff_linear_x changed val do updateEditProperties();
		on lk_falloff_linear_y changed val do updateEditProperties();
		on lk_falloff_linear_z changed val do updateEditProperties();

		on lk_falloff_quadratic_x changed val do updateEditProperties();
		on lk_falloff_quadratic_y changed val do updateEditProperties();
		on lk_falloff_quadratic_z changed val do updateEditProperties();

		on lk_noise_x changed val do updateEditProperties();
		on lk_noise_y changed val do updateEditProperties();
		on lk_noise_z changed val do updateEditProperties();
		
	--end of rollout	
	)
