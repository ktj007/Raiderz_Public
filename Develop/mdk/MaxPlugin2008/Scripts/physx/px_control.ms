--
-- PhysX control panel with shortcuts for the most common actions
--

filein "px_base.ms"
filein "px_keymap.ms"

global pxLastExportedFile = undefined;
global pxLastExportedFileFormat = 1;

		function UIIndexToPhysXType index = 
		(
			return case index of
			(
				1: PX_PHYSTYPE_DYNAMIC;
				2: PX_PHYSTYPE_KINEMATIC;
				3: PX_PHYSTYPE_STATIC;
				4: PX_PHYSTYPE_CLOTH;
				5: PX_PHYSTYPE_METAL_CLOTH;
				6: PX_PHYSTYPE_SOFTBODY;
				7: PX_PHYSTYPE_FORCEFIELD;
				8: PX_PHYSTYPE_UNDEFINED;
				11: PX_PHYSTYPE_RB_PROXY;
				default: PX_PHYSTYPE_UNDEFINED;
			)
		)

		function PhysXTypeToUIIndex type = 
		(
			return case type of
			(
				PX_PHYSTYPE_DYNAMIC: 1;
				PX_PHYSTYPE_KINEMATIC: 2;
				PX_PHYSTYPE_STATIC: 3;
				PX_PHYSTYPE_CLOTH: 4;
				PX_PHYSTYPE_METAL_CLOTH: 5;
				PX_PHYSTYPE_SOFTBODY: 6;
				PX_PHYSTYPE_FORCEFIELD: 7;
				PX_PHYSTYPE_UNDEFINED: 8;
				PX_PHYSTYPE_RB_PROXY: 11;
				default: 8;
			)
		)

		-- returns true if differs from old values
		function checkIntegerProperty n &props propName defaultValue = 
		(
			local tmpType = try( (getUserProp n propName) as integer ) catch(undefined);
			if (tmpType != undefined) then
			(
				local tmpMap = pxMap_hasKey &props propName;
				if (tmpMap != undefined) then
				(
					if (tmpMap[2] != tmpType) then
					(
						return true;
					)
				) else
				(
					pxMap_addPair &props propName tmpType;
				)
			)
			return false;
		)
		
		-- also returns "differs" when there is no property
		function checkPhysicsTypeProperty n &props propName defaultValue = 
		(
			local tmpType = try( (getUserProp n propName) as integer ) catch(undefined);
			if (tmpType != undefined) then
			(
				local tmpMap = pxMap_hasKey &props propName;
				if (tmpMap != undefined) then
				(
					if (tmpMap[2] != tmpType) then
					(
						return true;
					)
					return false;
				) else
				(
					pxMap_addPair &props propName tmpType;
					return false;
				)
			)
			return true;
		)

		-- returns true if differs from old values
		function checkBoolProperty n &props propName defaultValue = 
		(
			local tmpType = try( (getUserProp n propName) as BooleanClass ) 
			catch(
			try ( ((getUserProp n propName) as integer) != 0 ) catch(undefined);
			)
			if (tmpType != undefined) then
			(
				local tmpMap = pxMap_hasKey &props propName;
				if (tmpMap != undefined) then
				(
					if (tmpMap[2] != tmpType) then
					(
						return true;
					)
				) else
				(
					pxMap_addPair &props propName tmpType;
				)
			)
			return false;
		)


		function checkFloatProperty n &props propName defaultValue = 
		(
			local tmpType = try( (getUserProp n propName) as float ) catch(undefined);
			if (tmpType != undefined) then
			(
				local tmpMap = pxMap_hasKey &props propName;
				if (tmpMap != undefined) then
				(
					if (tmpMap[2] != tmpType) then
					(
						return true;
					)
				) else
				(
					pxMap_addPair &props propName tmpType;
				)
			)
			return false;
		)

		function checkStringProperty n &props propName defaultValue = 
		(
			local tmpType = try( (getUserProp n propName) as string ) catch(undefined);
			if (tmpType != undefined) then
			(
				local tmpMap = pxMap_hasKey &props propName;
				if (tmpMap != undefined) then
				(
					if (tmpMap[2] != tmpType) then
					(
						return true;
					)
				) else
				(
					pxMap_addPair &props propName tmpType;
				)
			)
			return false;
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
		

utility px_control "PhysX Utility" 
(
	label	lbl_about								"PhysX SDK Version unknown"
	checkbutton		pxbutton_control				"PhysX Control Panel"	checked:true	width:140 height:16 tooltip:"Shows the PhysX Control Panel"
	checkbutton 	pxbutton_convexdecomposition	"Geometry Tools"	checked:false	width:140 height:16 tooltip:"Geometry tools, e.g. convert a mesh into a convex."
  	checkbutton 	pxbutton_params					"PhysX Parameters"		checked:false	width:140 height:16 tooltip:"Sets PhysX SDK Parameters"
	
	local mOpenTopRollout = undefined;
	local mSimDelta = 1.0/frameRate;
	local mStepNumber = 1;

	function addPhysicsObjects objs =
	(
		--px.setnodetime (px.getnodetime()+1);
		local n = undefined;
		local k = 0;
		local targets = #()
		
		-- copy targets
		for i in objs do
		(
			append targets i
		)
		
		-- Make sure we add rigid bodies first
		local invalidNodes = #()
		for n in targets do
		(
			--format "process node %\n" n
			if (IsValidNode n) then
			(
				if (isrootrigidbody n) then
				(
					local type = getuserprop n "PhysicsType";
					if(type == PX_PHYSTYPE_DYNAMIC or type == PX_PHYSTYPE_KINEMATIC or type == PX_PHYSTYPE_STATIC) then
					(
						px_add n;
						k+=1;
					)
				)
			)
			else
			(
				p = findItem targets n
				if p > 0 then 
				(
					deleteItem targets p
					--format " >>>> delete one invalid node\n"
				)
			)
		)
	
		--cloth
		for n in targets do
		(
			if (IsValidNode n) and (isrootrigidbody n) then
			(
				local type = getuserprop n "PhysicsType";
				if(type != undefined and type == PX_PHYSTYPE_CLOTH) then
				(
					px_add_cloth n;
					k+=1;
				)
			)
		)
	
		--metal cloth
		for n in targets do
		(
			if (IsValidNode n) and (isrootrigidbody n) then
			(
				local type = getuserprop n "PhysicsType";
				if(type != undefined and type == PX_PHYSTYPE_METAL_CLOTH) then
				(
					px_add_metal_cloth n;
					k+=1;
				)
			)
		)
	
		--fluids
		for n in targets do
		(
			if (IsValidNode n) and ((classOf(n) as string) == "PxFluid") then
			(
				px_add_fluid n;
				k+=1;
			)
		)
	
		--fluid emitters
		for n in targets do
		(
			if (IsValidNode n) and ((classOf(n) as string) == "PxEmitter") then
			(
				px_add_fluidemitter n;
				k+=1;
			)
		)
		
		--soft body
		for n in targets do
		(
			if (IsValidNode n) then
			(
				local type = getuserprop n "PhysicsType";
				if(type != undefined and type == PX_PHYSTYPE_SOFTBODY) then
				(
					px_add_softbody n;
					k+=1;
				)
			)
		)
		
		--force field
		for n in targets do
		(
			if (IsValidNode n) then
			(
				local type = getuserprop n "PhysicsType";
				if(type != undefined and type == PX_PHYSTYPE_FORCEFIELD) then
				(
					px_add_forcefield n;
					k+=1;
				)
			)
		)
	
	   for n in targets do if (IsValidNode n) and (isrootrigidbody n) then px_addjoint n;
       for n in targets do if (IsValidNode n) and (((classof n) == px6DOF) or ((classof n) == pxJoint)) then px_addjointnode n;
	
		return k;
	)
	
	function addPhysicalThings =
	(
	  local nrAdded = addPhysicsObjects $objects;
	  print(format "% objects added\n" nrAdded);
	)

	rollout mainui_roll	"PhysX Control Panel"
	(
		timer       	clock          		"clock"                         interval:1		active:false
	
		label			lbl_a				"Add/remove objects from sim:"	pos:[5,8]
		button      	addAll    		"Add All Phys."         		pos:[4,25]		width:75 	tooltip:"Add all nodes in scene that are physically driven to the PhysX scene"
		button      	addselected    		"Add Selected"                  pos:[4,50]  	width:75 	tooltip:"Add selected objects to the PhysX scene"
		button      	removeall      		"Remove All"                    pos:[82,25] 	width:75 	tooltip:"Removes all physical nodes currently in the PhysX scene"
		button      	removeselected 		"Remove Sel."                	pos:[82,50] 	width:75 	tooltip:"Remove selected objects to the PhysX scene (if the are in the scene)"
		
		label			lbl_b				"Simulation:"					pos:[5,80]
		checkbox		slidermove			"move animation slider"			checked:true
		button      	simulateframe  		"Step"							pos:[4,120] 	width:70 	tooltip:"Run/Advance the Physics Simulation for specified number of frames"
		spinner			nf					""                              pos:[77,120]    width:40    type:#integer range:[1,1000,1]
		label			lbl_nf				"Frames"						pos:[120,120]
		checkbutton 	physrun        		"Continuous"    				pos:[4,145] 	width:70 	tooltip:"Turns Physics On so its always running in background"
		button      	resetpose      		"Reset"  						pos:[77,145]	width:70	tooltip:"Resets position and orientation to initial physics simulation setting"
		
		label			lbl_c				"Export:"						pos:[5,175]
		button			export_ui			"Export to"						pos:[4,190]		width:55
		dropdownlist 	exportformat_ui		""								pos:[62,190]	width:52 	items:#("XML", "Binary", "Collada")
		button			quickexport_ui		"Redo"							pos:[117,190]
	
		label			lbl_d				"Simulate selected object(s) as:" pos:[4,220]
		dropdownlist	simulationmethod_ui	""								pos:[5,235] 	width:150	items:#("Dynamic RigidBody","Kinematic RigidBody", "Static RigidBody", "Cloth", "Metal Cloth", "SoftBody", "ForceField", "Undefined")
		  -- "Non physical"
		button			addobjects_ui		"Make Physical / Edit"			pos:[23,235]	width:115	tooltip:"Press this button to specify PhysX settings for the selected object(s). All selected objects will get the same settings."

		local mDifferentProperties = true;
		local mPhysXPropertiesOfFirst = #();
		local mOpenRollout = undefined;
		local mPotentialRigidBodies = #();
		
		local m_rbprops_roll = undefined;
		
		on nf changed sk do (mStepNumber = nf.value)
		
		function getRolloutForType type =
		(
			return case type of
			(
				PX_PHYSTYPE_DYNAMIC: px_control.rigidbodyproperties_roll;
				PX_PHYSTYPE_KINEMATIC: px_control.rigidbodyproperties_roll;
				PX_PHYSTYPE_STATIC: px_control.rigidbodyproperties_roll;
				PX_PHYSTYPE_CLOTH: px_control.clothproperties_roll;
				PX_PHYSTYPE_METAL_CLOTH: px_control.metalcloth_property_roll;
				PX_PHYSTYPE_SOFTBODY: px_control.softbodyproperties_roll;
				PX_PHYSTYPE_FORCEFIELD: px_control.forcefieldproperties_roll;
				PX_PHYSTYPE_UNDEFINED: undefined;
				--9: px_control.fluidmessage_roll;
				default: undefined;
			)
		)

		function simuateOneFrame deleta =
		(
			-- print(format "animation end is %\n" animationRange.end);
			if(slidermove.checked) then
			(
				if(slidertime >= animationRange.end) then 
					slidertime = animationRange.end;
				else
					slidertime +=1;
			)
			-- the following line command will cause the Kinematic RB move like Static RB. Then it can wake up sleeping RB actors.
			-- px.mimicStaticRB true
			px.pxsim deleta;
			if(false) then
			(
				local num = px.findSleepingActors()
				for i = 1 to num do
				(
					n = px.getSleepingActor (i-1)
					format " find %\n" n
				)
			)
			if(false) then
			(
			    -- print speed
				for n in $selection do
				(
					local vol = px.getLinearVelocity n;
					print (format "velocity = % frameRate = %\n" vol frameRate);
				)
			)
			if(false) then
			(
				local num = px.getNumContacts();
				px.setContactFilter 1;
				local index = 1;
				while(px.getNextContact()) do
				(
					local point = px.getContactPoint();
					local force = px.getContactForce() ;
					format "contact % at point % force %\n" index point force;
					-- get the two actors in the collision
					local n1 = px.getContactNode0();
					local n2 = px.getContactNode1();
					format "contact % at point % force %, node % and %\n" index point force n1 n2;
					index = index + 1;
				)
			)
		)
		
		function changeRollout type = 
		(
			local newRollout = getRolloutForType type;
			if (newRollout != mOpenRollout) then
			(
				if (mOpenRollout != undefined) then
				(
					removeRollout mOpenRollout;
				)
				mOpenRollout = newRollout;
				if (mOpenRollout != undefined) then
				(
					addRollout mOpenRollout;
					px_control.scrollpos = 1000; --try to focus the newly added rollout
					mOpenRollout.updateSelection &mPhysXPropertiesOfFirst;
				)
			)
		)
		
		function mainUI_updateUI = 
		(
			local enableAddPhysX = true
			for n in $selection do
			(
				local type = getuserprop n "PhysicsType";
				--format " PhysX Type of % is %\n" n type
				if type == PX_PHYSTYPE_RB_PROXY then enableAddPhysX = false
			)
			simulationmethod_ui.visible = not mDifferentProperties;
			addobjects_ui.visible = mDifferentProperties;
			--addobjects_ui.enabled = ($selection.count > 0)
			if enableAddPhysX then
			(
				addobjects_ui.enabled = (mPotentialRigidBodies.count > 0);
			)
			else
			(
				addobjects_ui.enabled = false
			)

			if mDifferentProperties then
			(
				changeRollout 0;
			) else
			(
				local type = UIIndexToPhysXType(simulationmethod_ui.selection);
				--print (format "index = %, physx type = %\n" simulationmethod_ui.selection type);
				changeRollout type;
			)
			
			if (addobjects_ui.visible and (mOpenRollout != undefined)) then
			(
				removeRollout mOpenRollout;
				mOpenRollout = undefined;
			)
			
			local simulating = physrun.checked;
			if simulating then
			(
				-- print (format "mainUI_updateUI: simulating = %\n" simulating);
				px.pxprep();
				undo "PhysX" on 
				(
				  if (waitOpenNewScene == 0) do
				  (
				    px.pxsnap 0
				  )
				)
			)

			px_control.mainUI_roll.clock.active = false;
			if (waitOpenNewScene == 0) then
			(
			  px_control.mainUI_roll.clock.active = simulating;
			)
			px_control.mainUI_roll.addAll.enabled = not simulating;
			px_control.mainUI_roll.addselected.enabled = not simulating;
			px_control.mainUI_roll.removeall.enabled = not simulating;
			px_control.mainUI_roll.removeselected.enabled = not simulating;
			px_control.mainUI_roll.simulateframe.enabled = not simulating;
			px_control.mainUI_roll.resetpose.enabled = not simulating;
			
			px_control.mainUI_roll.export_ui.enabled = not simulating;
			px_control.mainUI_roll.exportformat_ui.enabled = not simulating;
			px_control.mainUI_roll.quickexport_ui.enabled = not simulating;
			px_control.mainUI_roll.quickexport_ui.enabled = false;
			if (pxLastExportedFile != undefined) then
			(
				px_control.mainUI_roll.quickexport_ui.enabled = not simulating;
			)
			
			if (mOpenRollout != undefined) then
			(
				mOpenRollout.enableControls (not simulating);
			)
		)

		function updateSelection =
		(
		    nf.value = mStepNumber;
			mDifferentProperties = false;
			mPhysXPropertiesOfFirst = #();
			mPotentialRigidBodies = #();
--			px_findRigidBodies $selection &mPotentialRigidBodies true;
			px_findPotentialRigidBodies $selection &mPotentialRigidBodies;

			--for n in $selection do
			for n in mPotentialRigidBodies do
			(
				local tmp = checkPhysicsTypeProperty n &mPhysXPropertiesOfFirst "PhysicsType" 0;
				if (tmp) then mDifferentProperties = true;
			)
			
			-- now that the PhysicsType has been read, let the correct rollout read the settings
			local tmpTypePair = pxMap_hasKey &mPhysXPropertiesOfFirst "PhysicsType";
			
--messagebox (mPhysXPropertiesOfFirst as string)
			if (tmpTypePair != undefined) then
			(
				local tmpType = try (tmpTypePair[2] as integer) catch(1);
				local index = PhysXTypeToUIIndex(tmpType);
				--print (format "Phyx type = %, index = %\n" tmpType index);
				if index == 11 then index = 8  -- special treatment for rb_proxy
				if index == 8 then mDifferentProperties = false
				simulationmethod_ui.selection = index;
				local tmpRollout = getRolloutForType tmpType;
--messagebox (tmpRollout as string)				
				if (tmpRollout != undefined) then
				(
					local tmp = tmpRollout.updateSelection &mPhysXPropertiesOfFirst;
					if (tmp) then mDifferentProperties = true;
				)
				else
				(
					mDifferentProperties = true
				)
			) else -- no properties defined yet
			(
				pxMap_addPair &mPhysXPropertiesOfFirst "PhysicsType" 1;
				mDifferentProperties = true; -- in this case it means that the object has not been added
			)
			
			mainUI_updateUI();
		)
		
		function updateSelectionCallback =
		(
			updateSelection();
			if (mOpenRollout != undefined) then
			(
				try (
					mOpenRollout.updateUIWithReadValues &mPhysXPropertiesOfFirst;
				) catch()
			)
		)
		
		on mainUI_roll open do
		(
			callbacks.addScript #selectionSetChanged "px_control.mainUI_roll.updateSelectionCallback()" id:#px_control_CALLBACK
			updateSelection();
		)
  
		on mainUI_roll close do
		(
            clock.active = false;
			if (mOpenRollout != undefined) then
			(
				removeRollout mOpenRollout;
			)
			callbacks.removeScripts #selectionSetChanged id:#px_control_CALLBACK
		)

        function onSystemReset = 
        (
			-- print ("onSystemReset: try to un-press PhysRun button\n");
			physrun.checked = false
			clock.active=false
			physrun.state = false
			mainUI_updateUI();
        )

		function updateEditProperties = 
		(
			for n in mPotentialRigidBodies do
			(
				local tmpType = UIIndexToPhysXType(simulationmethod_ui.selection);
				--print (format "updateEditProperties: index = %, PhysX Type = %\n" simulationmethod_ui.selection tmpType);
				setUserProp n "PhysicsType" tmpType;
			)
			
			-- update settings for the open rollout
			if (mOpenRollout != undefined) then
			(
				mOpenRollout.updateEditProperties();
			)
		)
		
		on addobjects_ui pressed do
		(
		
			updateEditProperties(); -- TODO: not sure if this is needed, the one below is much more important
			mDifferentProperties = false;
			local tmpType = (simulationmethod_ui.selection);
			--format "type selection is %\n" tmpType;
			if (tmpType > 3) and (tmpType < 8) then
			(
				--format "it is cloth/metal/sb/ff\n"
				if pxTools.includeGroups $selection then
				(
					messageBox "You should not set Grouped object(s) as cloth/metal cloth/softbody/forcefield!"
					simulationmethod_ui.selection = 1
				)
			)
			mainUI_updateUI();
			updateEditProperties(); -- so that we also save the properties from the opened rollout
		)
		
		on simulationmethod_ui selected selIndex do
		(
			local tmpType = selIndex;
			--format "type selection is %\n" tmpType;
			if (tmpType > 3) and (tmpType < 8) then
			(
				if pxTools.includeGroups $selection then
				(
					messageBox "You should not set Grouped object(s) as cloth/metal cloth/softbody/forcefield!"
					simulationmethod_ui.selection = 8
					selIndex = 8
				)
			)
			local n = UIIndexToPhysXType(selIndex);
			changeRollout n;
--			if (mOpenRollout != undefined) then
--			(
--				mOpenRollout.updateSelection();
--			)
			updateEditProperties();
		)
		
		on clock tick do 
		(
			if animButtonState then
			(
				animButtonState = false;
			)
			with redraw off 
			(
				try
				(
					clock.active = false;
					simuateOneFrame mSimDelta;
					clock.active = true;
					-- 0.016;
				) catch (
					clock.active = false;
				)
			)
			redrawviews();
		)
		

		on physrun changed s do
		(
		    mSimDelta = 1.0/frameRate;
		    -- print (format "PhysX Notice: simulation FPS = %\n" frameRate);
			-- print (format "needRefreshObjects = %\n" needRefreshObjects);
			if (needRefreshObjects == 1) then 
			(
				-- print ("before addPhysicalThings, inside physrun: Continuous\n");
				addPhysicalThings();
				needRefreshObjects = 0;
			)
			waitOpenNewScene = 0;
			mainUI_updateUI();
		)


		on addAll pressed do
		(
			-- remove all first
			try(
				px.pxremoveall();
			) catch()
			
			-- add all
			local nrAdded = addPhysicsObjects $objects;
			if (nrAdded == 0) then px_warning "Nothing Added To Simulation.  \nNo nodes specified directly to be physical.\nUse Physical Properties Editor to \nset PhysicsType."
			needRefreshObjects = 0;
		)

		on addselected pressed do
		(
			addPhysicsObjects $selection;
		)

		on resetpose pressed do
		(
			slidertime = 0;
			px.pxrestart();
			try( px.pxremoveall(); ) catch()
			addPhysicsObjects $objects;
		)

		on export_ui pressed do
		(
			local exportformat = exportformat_ui.selection;
	
			local format_ext = "xml";
			local format_name = "NxuStream XML";
	
			if(exportformat == 3) then ( format_ext = "dae"; format_name = "COLLADA"; )
			if(exportformat == 1) then ( format_ext = "xml"; format_name = "NxuStream XML"; )
			if(exportformat == 2) then ( format_ext = "nxb"; format_name = "NxuStream BINARY"; )
	
			local filename = "";
			local searchmask = format_name + "(*." + format_ext + ")|*." + format_ext;
			filename = getsavefilename filename:filename types:searchmask;
			if(filename != undefined) then
			(
				local result = px.pxnxuexport filename format_ext;
				if(result != 0) then
				(
					pxLastExportedFile = filename;
					pxLastExportedFileFormat = exportformat;
					quickexport_ui.enabled = true;
				)
			)
		)
		
		on quickexport_ui pressed do
		(
			local format_ext = "xml";
	
			if(pxLastExportedFileFormat == 3) then ( format_ext = "dae" )
			if(pxLastExportedFileFormat == 1) then ( format_ext = "xml" )
			if(pxLastExportedFileFormat == 2) then ( format_ext = "nxb" )
	
			local result = px.pxnxuexport pxLastExportedFile format_ext;
			if (result == 0) then
			(
				print ("Export to '" + pxLastExportedFile + "' failed");
			) else
			(
				print ("Scene exported to '" + pxLastExportedFile + "'");
			)
		)	

		on removeall pressed do
		(
--			for n in $objects do
--			(
--				px.pxremove n;
--			)
			try(
				px.pxremoveall();
			) catch()
		)
		
		on removeselected pressed do
		(
			local n
			if ($selection == undefined) then return px_warning "Nothing Selected.\nSelect objects you wish to remove from PhysX engine"
			for n in $selection do px.pxremove n;
		)
		
		on simulateframe pressed do
		(
		    mSimDelta = 1.0/frameRate;
			-- print ("inside simulateframe\n");
			if (needRefreshObjects == 1) do 
			(
				addPhysicalThings();
				needRefreshObjects = 0;
			)
			
			progressstart "PhysX simulation";
			px.pxprep();
			if not animButtonState then undo "PhysX" on 
			(
				if (waitOpenNewScene == 0) do
				(
				  px.pxsnap 0; -- takes a snapshot of the inodes' TMs that might change
				)
			)
			-- nf.value
			local frames = nf.value;
			local i = 0;
			while i < frames and progressupdate (100.0*i/frames) do
			(
				with redraw off
				(
					simuateOneFrame mSimDelta;
				)
				redrawviews ();
				i += 1;
			)
			progressend();
		)

	) -- end of main rollout

	-- include the different property panels
	include "px_rigidbodyproperties.ms"
	include "px_clothproperties.ms"
	include "px_softbodyproperties.ms"
	include "px_forcefieldproperties.ms"
	
	rollout notimplemented_roll "Message"
	(
		label lbl_notimpl "Object type not yet implemented"
		function updateEditProperties = ()
		function enableControls state = ()
		function updateSelection &props = ( return false; )
		function updateUIWithReadValues &props = ()
	)

	rollout fluidmessage_roll "Message"
	(
		label lbl_fluidobj "Fluids can't be edited here,"
		label lbl_fluidobj2 "use the helper menu"
		function updateEditProperties = ()
		function enableControls state = ()
		function updateSelection &props = ( return false; )
		function updateUIWithReadValues &props = ()
	)
  
	on px_control open do
	(
		mOpenTopRollout = mainUI_roll;
		addRollout mOpenTopRollout;
		if px != undefined then
		(
			version = case px.getVersion() of
			(
				273: "2.7.3"
				280: "2.8.0"
				281: "2.8.1"
			)
			lbl_about.text = "PhysX SDK Version " + version
		)
	)
  
	on px_control close do
	(
		if (mOpenTopRollout != undefined) then
		(
			removeRollout mOpenTopRollout;
			mOpenTopRollout = undefined;
		)
	)
    
    function onSystemReset = 
    (
		-- print ("onSystemReset another 1\n");
		if (mOpenTopRollout != undefined) then
		(
			if (mOpenTopRollout.onSystemReset != undefined) then
			(
				-- print ("onSystemReset another 2\n");
				mOpenTopRollout.onSystemReset();
			)
		)
    )

	rollout convexdecomposition_roll "Geometry Tools"
	(
		label	lbl_k	"Create editable SoftBody mesh:"
		checkbox	editable_ui					"Create editable mesh"		checked:true
		checkbox	isosurface_ui				"Create iso surface"		checked:true
		checkbox	isosurfacesingle_ui			"Single iso surface"
		spinner		subdivisionlevel_ui			"Subdivision lvl"			range:[2,90,10]		type:#integer
		spinner		surfacesimplification_ui	"Surface simpl."			range:[0.0,1.0,0.5]	type:#float
		button		buildmesh_ui				"Build mesh"				tooltip:"Push this button to build a mesh suitable for SoftBody simulation using the parameters above"
		
		
		label	lbl_e	"Create convex hulls"	align:#left								pos:[4,160]
		spinner	vertcount	"Max. vertices"	range:[3, 64, 32]			type:#integer	pos:[5,180]
		spinner	inflation	"Inflate hull"	range:[0.0, 100.0, 0.025]	type:#float		pos:[5,200]
		button	skinwidth	"SW"			tooltip:"Press to set inflation to current skin width"	width:30	height:16	pos:[125,200]
		button	createhull	"Create Hulls"	tooltip:"Press to create convex representations of the selected objects"

--		label	lbl_d
--		label	lbl_f	"----- ALPHA Features -----"
--		label	lbl_h
--		label	lbl_c	"Create convex decompositions"
--		button  build   "Build Decompositions"	tooltip:"Press to create convex decompositions of the selected objects"	
--		label	lbl_i	""
--		label	lbl_j	"Replace bones with hulls"
--		spinner wmin "Minimum Weight" type:#float range:[0,0.50,0.50] scale:0.01
--		button rbwhm "Replace Bones With Hulls"
--		on rbwhm pressed do
--		(
--			bonehullit $ wmin.value
--		)
--
--		on build pressed do
--		(
--			MessageBox "Not implemented - use the 'AggregateHull' geometry object instead";
----			local n
----			local g = $selection
----			for n in g do
----			(
----				if (superclassof n) != GeometryClass then continue
----				count = px.convexDecomposition n;
----			)
--		)
		
		on buildmesh_ui pressed do
		(
			local n = undefined;
			for n in $selection do (
				local tempMesh = px.createSoftBodyMesh n editable_ui.checked subdivisionlevel_ui.value surfacesimplification_ui.value isosurface_ui.checked isosurfacesingle_ui.checked;

				if (tempMesh == undefined) then
				(
					print ("Unable to create mesh for " + n.name);
					continue;
				)
			
			
				local tempName = uniquename(n.name + "_tetra");
				local b = box width:10 height:10 length:10 name:tempName;
				convertToMesh b;
				b.transform = n.transform;
				b.mesh = tempMesh.mesh;
			)
		)	

		on createhull pressed do
		(
			local sel = #();
			local n = undefined;
			for n in $selection do append sel n;
			for n in sel do
			(
				r = px_convertToConvex n vertcount.value inflation.value;
			)
		)
		
		on skinwidth pressed do
		(
			local temp = px.getSDKParameter "NX_SKIN_WIDTH";
			if (temp >= 0.0) then
			(
				inflation.value = temp;
			) else
			(
				inflation.value = 0.025;
			)
		)
		
		on convexdecomposition_roll open do
		(
			local temp = px.getSDKParameter "NX_SKIN_WIDTH";
			if (temp >= 0.0) then
			(
				inflation.value = temp;
			)
		)
		
        function onSystemReset = 
        (
        )
	
	) -- end of convex decomposition rollout
	
	rollout physxparams_roll "PhysX SDK Parameters"
	(
		spinner    spin_skinwidth       "Skin Width"        type:#float range:[0,    100,  0.025] scale:0.01
		spinner    spin_sleeplinvel     "Sleep Lin Vel"     type:#float range:[0,    100,  0.15]  scale:0.01
		spinner    spin_sleepangvel     "Sleep Ang Vel"     type:#float range:[0,    100,  0.14]  scale:0.01
		spinner    spin_bouncethreshold "Bounce Thresh."   	type:#float range:[-100, 0,   -2]     scale:0.01
		spinner    spin_dynfrictscaling "Dyn Frict Scaling" type:#float range:[0,    100,  1]     scale:0.01
		spinner    spin_stafrictscaling "Sta Frict Scaling" type:#float range:[0,    100,  1]     scale:0.01
		spinner    spin_maxangvel       "Max Ang Vel"       type:#float range:[0,    1000, 7]     scale:0.01

		radiobuttons gravity			"Gravity Direction"	labels:#("X","Y","Z") 	default:3 align:#left
		spinner    gravityZ				"Magnitude"			type:#float range:[-100000,1000000, -9.81] scale:1 

		on spin_skinwidth changed sk do (px.setSDKParameter "NX_SKIN_WIDTH" spin_skinwidth.value)
		--on spin_sleeplinvel changed sk do (px.setSDKParameter "NX_DEFAULT_SLEEP_LIN_VEL_SQUARED" (spin_sleeplinvel.value*spin_sleeplinvel.value*px_setting_scenescale*px_setting_scenescale))
		on spin_sleeplinvel changed sk do (px.setSDKParameter "NX_DEFAULT_SLEEP_LIN_VEL_SQUARED" (spin_sleeplinvel.value*spin_sleeplinvel.value))
		on spin_sleepangvel changed sk do (px.setSDKParameter "NX_DEFAULT_SLEEP_ANG_VEL_SQUARED" (spin_sleepangvel.value*spin_sleepangvel.value))
		--on spin_bouncethreshold changed sk do (px.setSDKParameter "NX_BOUNCE_THRESHOLD" (spin_bouncethreshold.value*px_setting_scenescale))
		on spin_bouncethreshold changed sk do (px.setSDKParameter "NX_BOUNCE_THRESHOLD" (spin_bouncethreshold.value))
		on spin_dynfrictscaling changed sk do (px.setSDKParameter "NX_DYN_FRICT_SCALING" spin_dynfrictscaling.value)
		on spin_stafrictscaling changed sk do (px.setSDKParameter "NX_STA_FRICT_SCALING" spin_stafrictscaling.value)
		on spin_maxangvel changed sk do (px.setSDKParameter "NX_MAX_ANGULAR_VELOCITY" spin_maxangvel.value)
  
		function updateGravity =
		(
			if(gravity.state == 1) then
			(
				px.setGravity [gravityZ.value,0,0];
			)
			else if(gravity.state == 2) then
			(
				px.setGravity [0, gravityZ.value,0];
			)
			else if(gravity.state == 3) then
			(
				px.setGravity [0, 0, gravityZ.value];
			)
		)

		on gravityZ changed g do 
		(
			updateGravity()
		)
		on gravity changed g do
		(
			updateGravity()
		)

		function biggestIndex v =
		(
			if (abs(v[1]) > abs(v[2])) then
			(
				return if (abs(v[3]) > abs(v[1])) then 3 else 1;
			) else
			(
				return if (abs(v[3]) > abs(v[2])) then 3 else 2;
			)
		)
		
		function initParamsRoll = 
		(
			local sk = px.getSDKParameter "NX_SKIN_WIDTH"
			spin_skinwidth.value = if sk < -1e10 then 0.025 else sk;

			local slv = px.getSDKParameter "NX_DEFAULT_SLEEP_LIN_VEL_SQUARED"
			--spin_sleeplinvel.value = sqrt(slv/px_setting_scenescale/px_setting_scenescale)
			spin_sleeplinvel.value = if slv < -1e10 then 0.15 else sqrt(slv);

			local sav = px.getSDKParameter "NX_DEFAULT_SLEEP_ANG_VEL_SQUARED"
			spin_sleepangvel.value = if sav < -1e10 then 0.14 else sqrt(sav);

			local bt = px.getSDKParameter "NX_BOUNCE_THRESHOLD"
			--spin_bouncethreshold.value = bt/px_setting_scenescale
			spin_bouncethreshold.value = if bt < -1e10 then -2 else bt;

			local dfs = px.getSDKParameter "NX_DYN_FRICT_SCALING"
			spin_dynfrictscaling.value = if dfs < -1e10 then 1 else dfs;

			local sfs = px.getSDKParameter "NX_STA_FRICT_SCALING"
			spin_stafrictscaling.value = if sfs < -1e10 then 1 else sfs;

			local mav = px.getSDKParameter "NX_MAX_ANGULAR_VELOCITY"
			spin_maxangvel.value = if mav < -1e10 then 7 else mav;
			
			local grav = px.getGravity();
			local axis = biggestIndex grav;
			gravity.state = axis;
			gravityZ.value = grav[axis];  
			-- print grav;
		)
		
		--function onSystemResetBak = 
		--(
		--	initParamsRoll();
		--)
		
		on physxparams_roll open do
		(
			initParamsRoll();
		)

	) -- end of PhysX parameters rollout
	
	function changeTopRollout = 
	(
		local newRollout = mainUI_roll;
		if pxbutton_convexdecomposition.checked then newRollout = convexdecomposition_roll;
		if pxbutton_params.checked then newRollout = physxparams_roll;
		if (newRollout != mOpenTopRollout) then
		(
			if (mOpenTopRollout != undefined) then
			(
				removeRollout mOpenTopRollout;
			)
			mOpenTopRollout = newRollout;
			if (mOpenTopRollout != undefined) then
			(
				addRollout mOpenTopRollout;
				px_control.scrollpos = 1000; --try to focus the newly added rollout
			)
		)
	)
	
	
	local mInHandler = false;
	on pxbutton_control changed state do
	(
		if (not state) then pxbutton_control.checked = true;
		pxbutton_convexdecomposition.checked = false;
		pxbutton_params.checked = false;
		changeTopRollout();
	)
	
	on pxbutton_convexdecomposition changed state do
	(
		pxbutton_control.checked = false;
		if (not state) then pxbutton_control.checked = true;
		pxbutton_params.checked = false;
		changeTopRollout();
	)

  	on pxbutton_params changed state do
	(
		pxbutton_control.checked = false;
		if (not state) then pxbutton_control.checked = true;
		pxbutton_convexdecomposition.checked = false;
		changeTopRollout();
	)
)
