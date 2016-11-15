-- 
-- This file contains the rollout code for editing cloth properties
--
	rollout clothproperties_roll "Cloth Properties"
	(
		
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

		function setClothProp name val =
		(
			for n in $selection do
			(
				setUserProp n name val;
			)
		)

		label     cloth_flags               "Cloth Flags:"			pos:[4,5]
		checkbox  clf_pressure              "Pressure"				pos:[6,22]
		checkbox  clf_static                "Static"				pos:[80,22]
		checkbox  clf_disable_collision     "Disable col."			pos:[6,42]
		checkbox  clf_selfcollision         "Self collision"		pos:[80,42]
		checkbox  clf_gravity               "Gravity"				pos:[6,62]
		checkbox  clf_bending               "Bending"				pos:[80,62]
		checkbox  clf_bending_ortho         "Ortho bend"			pos:[6,82]
		checkbox  clf_damping               "Damping"				pos:[80,82]
		checkbox  clf_collision_twoway      "2-way coll."			pos:[6,102]
		checkbox  clf_comdamping            "Comdamping"			pos:[80,102]
		checkbox  clf_tearable              "Tearable"				pos:[6,122]
		checkbox  clf_triangle_collision    "Triangle coll."		pos:[80,122]
		checkbox  clf_fluid_collision       "Fluid Coll."			pos:[6,142]
		checkbox  clf_hardware              "Hardware"				pos:[80,142]
		  
		label     cloth_props               "Cloth Properties:"		pos:[4,165]
		checkbox  cll_autoattach            "Auto-attach to shapes" pos:[6,181] --automatically attach to colliding shapes
		spinner   clp_thickness             "Thickness"             range:[0, 9999999,  0.01]
		spinner   clp_density               "Density"               range:[0.001, 9999999,  1.0]
		spinner   clp_bendingStiffness      "Bending Stiff."               range:[0, 1,    1.0]
		spinner   clp_stretchingStiffness   "Stretching Stiff."            range:[0.001, 1,    1.0]
		spinner   clp_dampingCoefficient    "Damping Coef."               range:[0, 1,    0.5]
		spinner   clp_friction              "Friction"              range:[0, 1,    0.5]
		spinner   clp_pressure              "Pressure"              range:[0, 9999999,  1.0]
		spinner   clp_tearFactor            "Tear factor"           range:[1.001, 9999999,  1.5] enabled:false
		spinner   clp_collisionResponseCoef "Collision Resp"        range:[0, 9999999,  0.2]
		spinner   clp_attachResponseCoef    "Attachment Resp"       range:[0, 1,  0.2]
		spinner   clp_solverIterations      "Solver Iterations"     range:[1, 100,  5]    type:#integer scale:1
		
		function updateSelection &props = 
		(
			-- can't update GUI objects in here, it can be called before the gui has been opened
			local differences = false;
			for n in $selection do
			(
				if (checkBoolProperty n &props "px_clf_pressure" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_static" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_disable_collision" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_selfcollision" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_gravity" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_bending" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_bending_ortho" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_damping" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_comdamping" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_collision_twoway" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_triangle_collision" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_tearable" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_hardware" false) then differences = true;
				if (checkBoolProperty n &props "px_clf_fluid_collision" false) then differences = true;
				
				if (checkBoolProperty n &props "px_cll_autoattach" false) then differences = true;
				
				if (checkFloatProperty n &props "px_clp_thickness" 0.01) then differences = true;
				if (checkFloatProperty n &props "px_clp_density" 1.0) then differences = true;
				if (checkFloatProperty n &props "px_clp_bendingStiffness" 1.0) then differences = true;
				if (checkFloatProperty n &props "px_clp_stretchingStiffness" 1.0) then differences = true;
				if (checkFloatProperty n &props "px_clp_dampingCoefficient" 0.5) then differences = true;
				if (checkFloatProperty n &props "px_clp_friction" 0.5) then differences = true;
				if (checkFloatProperty n &props "px_clp_pressure" 1.0) then differences = true;
				if (checkFloatProperty n &props "px_clp_tearFactor" 1.5) then differences = true;
				if (checkFloatProperty n &props "px_clp_collisionResponseCoef" 1.0) then differences = true;
				if (checkFloatProperty n &props "px_clp_attachResponseCoef" 1.0) then differences = true;
				if (checkIntegerProperty n &props "px_clp_solverIterations" 5) then differences = true;
			)
			
			return differences;
		)
		
		function updateUIWithReadValues &props =
		(
			clf_pressure.checked = getValueAsBool (pxMap_getValue &props "px_clf_pressure" false) false;
			clf_static.checked = getValueAsBool (pxMap_getValue &props "px_clf_static" false) false;
			clf_disable_collision.checked = getValueAsBool (pxMap_getValue &props "px_clf_disable_collision" false) false;
			clf_selfcollision.checked = getValueAsBool (pxMap_getValue &props "px_clf_selfcollision" false) false;
			clf_gravity.checked = getValueAsBool (pxMap_getValue &props "px_clf_gravity" false) false;
			clf_bending.checked = getValueAsBool (pxMap_getValue &props "px_clf_bending" false) false;
			clf_bending_ortho.checked = getValueAsBool (pxMap_getValue &props "px_clf_bending_ortho" false) false;
			clf_damping.checked = getValueAsBool (pxMap_getValue &props "px_clf_damping" false) false;
			clf_comdamping.checked = getValueAsBool (pxMap_getValue &props "px_clf_comdamping" false) false;
			clf_collision_twoway.checked = getValueAsBool (pxMap_getValue &props "px_clf_collision_twoway" false) false;
			clf_triangle_collision.checked = getValueAsBool (pxMap_getValue &props "px_clf_triangle_collision" false) false;
			clf_tearable.checked = getValueAsBool (pxMap_getValue &props "px_clf_tearable" false) false;
			clf_hardware.checked = getValueAsBool (pxMap_getValue &props "px_clf_hardware" false) false;
			clf_fluid_collision.checked = getValueAsBool (pxMap_getValue &props "px_clf_fluid_collision" false) false;

			clp_thickness.value = getValueAsFloat (pxMap_getValue &props "px_clp_thickness" 0.01) 0.01;
			clp_density.value = getValueAsFloat (pxMap_getValue &props "px_clp_density" 1.0) 1.0;
			clp_bendingStiffness.value = getValueAsFloat (pxMap_getValue &props "px_clp_bendingStiffness" 1.0) 1.0;
			clp_stretchingStiffness.value = getValueAsFloat (pxMap_getValue &props "px_clp_stretchingStiffness" 1.0) 1.0;
			clp_dampingCoefficient.value = getValueAsFloat (pxMap_getValue &props "px_clp_dampingCoefficient" 0.5) 0.5;
			clp_friction.value = getValueAsFloat (pxMap_getValue &props "px_clp_friction" 0.5) 0.5;
			clp_pressure.value = getValueAsFloat (pxMap_getValue &props "px_clp_pressure" 1.0) 1.0;
			clp_tearFactor.value = getValueAsFloat (pxMap_getValue &props "px_clp_tearFactor" 1.5) 1.5;
			clp_collisionResponseCoef.value = getValueAsFloat (pxMap_getValue &props "px_clp_collisionResponseCoef" 0.2) 0.2;
			clp_attachResponseCoef.value = getValueAsFloat (pxMap_getValue &props "px_clp_attachResponseCoef" 1.0) 1.0;
			clp_solverIterations.value = getValueAsInt (pxMap_getValue &props "px_clp_solverIterations" 5) 5;

			cll_autoattach.checked = getValueAsBool (pxMap_getValue &props "px_cll_autoattach" false) false;
		)

		function updateEditProperties =
		(
			setClothProp "px_clf_pressure"				clf_pressure.checked;
			setClothProp "px_clf_static"				clf_static.checked;
			setClothProp "px_clf_disable_collision"		clf_disable_collision.checked;
			setClothProp "px_clf_selfcollision"			clf_selfcollision.checked;
			setClothProp "px_clf_gravity"				clf_gravity.checked;
			setClothProp "px_clf_bending"				clf_bending.checked;
			setClothProp "px_clf_bending_ortho"			clf_bending_ortho.checked;
			setClothProp "px_clf_damping"				clf_damping.checked;
			setClothProp "px_clf_comdamping"			clf_comdamping.checked;
			setClothProp "px_clf_collision_twoway"		clf_collision_twoway.checked;
			setClothProp "px_clf_triangle_collision"	clf_triangle_collision.checked;
			setClothProp "px_clf_tearable"				clf_tearable.checked;
			setClothProp "px_clf_hardware"				clf_hardware.checked;
			setClothProp "px_clf_fluid_collision"		clf_fluid_collision.checked;
			setClothProp "px_clp_thickness"				clp_thickness.value;
			setClothProp "px_clp_density"				clp_density.value;
			setClothProp "px_clp_bendingStiffness"		clp_bendingStiffness.value;
			setClothProp "px_clp_stretchingStiffness"	clp_stretchingStiffness.value;
			setClothProp "px_clp_dampingCoefficient"	clp_dampingCoefficient.value;
			setClothProp "px_clp_friction"				clp_friction.value;
			setClothProp "px_clp_pressure"				clp_pressure.value;
			setClothProp "px_clp_tearFactor"			clp_tearFactor.value;
			setClothProp "px_clp_collisionResponseCoef"	clp_collisionResponseCoef.value;
			setClothProp "px_clp_attachResponseCoef"	clp_attachResponseCoef.value;
			setClothProp "px_clp_solverIterations"		clp_solverIterations.value;
			setClothProp "px_cll_autoattach"			cll_autoattach.checked;
		)
		
		function enableControls editState =
		(
			clf_pressure.enabled = editState;
			clf_static.enabled = editState;
			clf_disable_collision.enabled = editState;
			clf_selfcollision.enabled = editState;
			clf_gravity.enabled = editState;
			clf_bending.enabled = editState;
			clf_bending_ortho.enabled = clf_bending.checked;
			clf_damping.enabled = editState;
			clf_comdamping.enabled = clf_damping.enabled and clf_damping.checked;
			clf_collision_twoway.enabled = editState;
			clf_triangle_collision.enabled = editState;
			clf_tearable.enabled = editState;
			clf_hardware.enabled = editState and px.hwAvailable();
			clf_fluid_collision.enabled = editState;
			clp_thickness.enabled = editState;
			clp_density.enabled = editState;
			clp_bendingStiffness.enabled = editState;
			clp_stretchingStiffness.enabled = editState;
			clp_dampingCoefficient.enabled = clf_comdamping.enabled and clf_comdamping.checked;
			clp_friction.enabled = editState;
			clp_pressure.enabled = editState;
			clp_tearFactor.enabled = clf_tearable.checked;
			clp_collisionResponseCoef.enabled = clf_collision_twoway.checked;
			clp_attachResponseCoef.enabled = editState;
			clp_solverIterations.enabled = editState;
			cll_autoattach.enabled = editState;
		)

		fn updateRelativeWidgets = 
		(
			clf_comdamping.enabled = clf_damping.enabled and clf_damping.checked
			clp_dampingCoefficient.enabled = clf_comdamping.enabled and clf_comdamping.checked;
			clf_collision_twoway.enabled = not clf_disable_collision.checked
			clp_collisionResponseCoef.enabled = clf_collision_twoway.enabled and clf_collision_twoway.checked;
			clf_bending_ortho.enabled = clf_bending.checked
		)

		on clothproperties_roll open do
		(
			updateUIWithReadValues px_control.mainui_roll.mPhysXPropertiesOfFirst;
			enableControls true;
			updateRelativeWidgets()
		)
		
		on clf_pressure               changed val do      ( setClothProp "px_clf_pressure"              val; )
		on clf_static                 changed val do      ( setClothProp "px_clf_static"                val; )
		on clf_disable_collision      changed val do      
		(
			updateRelativeWidgets()
			setClothProp "px_clf_disable_collision"     val; 
		)
		on clf_selfcollision          changed val do      ( setClothProp "px_clf_selfcollision"         val; )
		on clf_gravity                changed val do      ( setClothProp "px_clf_gravity"               val; )
		on clf_bending                changed val do      
		( 
			updateRelativeWidgets()
			setClothProp "px_clf_bending"               val; 
		)
		on clf_bending_ortho          changed val do      ( setClothProp "px_clf_bending_ortho"         val; )
		on clf_damping                changed val do      
		( 
			updateRelativeWidgets()
			setClothProp "px_clf_damping"               val; 
		)
		on clf_comdamping                changed val do      
		( 
			updateRelativeWidgets()
			setClothProp "px_clf_comdamping"               val; 
		)
		on clf_collision_twoway       changed val do      
		(
			updateRelativeWidgets()
			setClothProp "px_clf_collision_twoway"      val; 
		)
		on clf_triangle_collision     changed val do      ( setClothProp "px_clf_triangle_collision"    val; )
		on clf_tearable               changed val do      
		( 
			clp_tearFactor.enabled = clf_tearable.checked
			setClothProp "px_clf_tearable"              val;
		)
		on clf_hardware               changed val do      ( setClothProp "px_clf_hardware"              val; )
		on clf_fluid_collision        changed val do      ( setClothProp "px_clf_fluid_collision"       val; )
		  
		on clp_thickness              changed val do      ( setClothProp "px_clp_thickness"             val; )
		on clp_density                changed val do      ( setClothProp "px_clp_density"               val; )
		on clp_bendingStiffness       changed val do      ( setClothProp "px_clp_bendingStiffness"      val; )
		on clp_stretchingStiffness    changed val do      ( setClothProp "px_clp_stretchingStiffness"   val; )
		on clp_dampingCoefficient     changed val do      ( setClothProp "px_clp_dampingCoefficient"    val; )
		on clp_friction               changed val do      ( setClothProp "px_clp_friction"              val; )
		on clp_pressure               changed val do      ( setClothProp "px_clp_pressure"              val; )
		on clp_tearFactor             changed val do      ( setClothProp "px_clp_tearFactor"            val; )
		on clp_collisionResponseCoef  changed val do      ( setClothProp "px_clp_collisionResponseCoef" val; )
		on clp_attachResponseCoef     changed val do      ( setClothProp "px_clp_attachResponseCoef"    val; )
		on clp_solverIterations       changed val do      ( setClothProp "px_clp_solverIterations"      val; )
		  
		on cll_autoattach             changed val do      ( setClothProp "px_cll_autoattach"            val; )
		

	--end of rollout	
	)

	rollout metalcloth_property_roll "MetalCloth Properties"
	(
		
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
		

		label         lb_core_type        "Core Type"       align:#left  across: 2 offset:[-8, 0]
		dropdownlist  ui_core_type        ""	                  width: 80 offset:[-10, 0] items:#("Sphere", "Capsule", "Box", "Sphere Compound") selection: 1
		spinner       ui_num_sphere       "Sphere Number"         type:#integer range:[1,100000, 1] align:#right toolTip:"number of Spheres"
		
		spinner       ui_core_mass            "Core Mass"         type:#float range:[0.01,10000, 1] align:#right

		spinner       ui_impulse_shreshold   "Impulse Thresh."        range:[-100,100000, 1] align:#right toolTip:"Impulse Threshold"
		spinner       ui_penetration_depth   "Penetra. Depth"         range:[0,100000, 0] align:#right    toolTip:"Penetration Depth"
		spinner       ui_max_deform_distance "Max Deform."       range:[0,100000, 0] align:#right    toolTip:"Max Deformation Distance"
		
		function updateSelection &props = 
		(
			-- can't update GUI objects in here, it can be called before the gui has been opened
			local differences = false;
			for n in $selection do
			(
				if (checkIntegerProperty n &props "px_mc_core_shape" 1)  then differences = true;
				if (checkIntegerProperty n &props "px_mc_num_sphere" 1)  then differences = true;
				if (checkFloatProperty n &props "px_mc_core_mass" 1) then differences = true;
				if (checkFloatProperty n &props "px_mc_impulse_threshold" 1) then differences = true;
				if (checkFloatProperty n &props "px_mc_penetration_depth" 0) then differences = true;
				if (checkFloatProperty n &props "px_mc_max_deform_distance" 0) then differences = true;
			)
			return differences;
		)
		
		function updateUIWithReadValues &props =
		(
			ui_core_type.selection = getValueAsInt (pxMap_getValue &props "px_mc_core_shape" 1) 1;
			ui_num_sphere.value    = getValueAsInt (pxMap_getValue &props "px_mc_num_sphere" 0.0) 0.0;
			ui_core_mass.value     = getValueAsFloat (pxMap_getValue &props "px_mc_core_mass" 1.0) 1.0;
			ui_impulse_shreshold.value     = getValueAsFloat (pxMap_getValue &props "px_mc_impulse_threshold" 1.0) 1.0;
			ui_penetration_depth.value    = getValueAsFloat (pxMap_getValue &props "px_mc_penetration_depth" 0.0) 0.0;
			ui_max_deform_distance.value  = getValueAsFloat (pxMap_getValue &props "px_mc_max_deform_distance" 0.0) 0.0;
		)

		function updateEditProperties =
		(
			for n in $selection do
			(
				setUserProp n "px_mc_core_shape"                ui_core_type.selection;
				setUserProp n "px_mc_num_sphere"                ui_num_sphere.value;
				setUserProp n "px_mc_core_mass"                 ui_core_mass.value;
				setUserProp n "px_mc_impulse_threshold"                ui_impulse_shreshold.value;
				setUserProp n "px_mc_penetration_depth"                ui_penetration_depth.value;
				setUserProp n "px_mc_max_deform_distance"              ui_max_deform_distance.value;
			)
		)
		
		function enableControls editState =
		(
			ui_core_type.enabled = editState;
			ui_num_sphere.enabled = (editState and (ui_core_type.selection==4));
			ui_core_mass.enabled = editState;
			ui_impulse_shreshold.enabled = editState;
			ui_penetration_depth.enabled = editState;
			ui_max_deform_distance.enabled = editState;
		)		
		
		on geometrytype_ui selected selIndex do
		(
			updateEditProperties();
			enableControls true;
		)
		
		on ui_core_type selected selIndex do 
		(
			updateEditProperties();
			enableControls true;
		)
		
		on ui_num_sphere changed val do updateEditProperties();
		on ui_core_mass changed val do updateEditProperties();
		on ui_impulse_shreshold changed val do updateEditProperties();
		on ui_penetration_depth changed val do updateEditProperties();
		on ui_max_deform_distance changed val do updateEditProperties();

		on metalcloth_property_roll open do
		(
			updateUIWithReadValues px_control.mainui_roll.mPhysXPropertiesOfFirst;
			enableControls true;
		)
		
	--end of rollout	
	)

