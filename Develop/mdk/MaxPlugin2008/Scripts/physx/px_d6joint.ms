-- 
--  PhysX
--
--  D6 Joint editor
--  6-DOF joint editor.
--

plugin helper pxJoint
name:"PxJoint"
category:"NVIDIA PhysX"
classID:#(0x5fa2e45d, 0x17e7a9e)
extends:dummy replaceui:true
(
  local mDisplayMesh = undefined;
  local mNeedRebuild = true;

  function addToScene n =
  (
  	if (n == undefined) then
	(
		return 0;
	)

	-- before creating the joint descriptor, make sure we are referring to the correct bodies
	local pbody = n.body0
	local cbody = n.body1
	local tempPBody = px_getPxActor pbody;
	if (tempPBody != undefined and pbody != tempPBody) then
	(
		pbody = tempPBody;
		n.body0 = pbody;
	)
	local tempCBody = px_getPxActor cbody;
	if (tempCBody != undefined and cbody != tempCBody) then
	(
		cbody = tempCBody;
		n.body1 = cbody;
	)

	-- create a joint descriptor to fill in
    local jointDesc = px.pxcreateD6JointDesc n;
    if (jointDesc != undefined) then
    (
		
		if( pbody == undefined and cbody == undefined ) then
		(
			messagebox ("Warning: D6 Joint " + n.name + " not added because its rigid bodies are not defined.");
			px.pxremove n; -- don't forget to remove the descriptor for the failed object
			return 0;
		)

		-- figure out which type of attachment point we should calculate
		local attachmentPointType = 1;
		if (n.aptype != undefined) then
		(
			attachmentPointType = n.aptype
		)
		
		-- figure out the transform of the actors
		local jf0 = matrix3 1;
		local jf1 = matrix3 1;
		if (pbody != undefined) then
		(
			jf0 = pbody.transform;
			if ((px.isActor pbody) != 0) then
			(
				--get the transform of the actor from the plugin
				jf0 = px.getGlobalPose pbody;
			) else (
                jf0 = pbody.transform;
			)
		)

		if (cbody != undefined) then
		(
			jf1 = cbody.transform;
			if ((px.isActor cbody) != 0) then
			(
				jf1 = px.getGlobalPose cbody;
			) else (
                jf1 = cbody.transform;
			)			
		)
		
		-- calculate the local axes for the joint
		local jf0_n = matrix3 1;
		local jf1_n = matrix3 1;
		local tempTransform = copy jf0;
		tempTransform.pos = [0, 0, 0];
		tempTransform = inverse tempTransform;
		jf0_n[1] = normalize (n.transform[1] * tempTransform)
		jf0_n[2] = normalize (n.transform[2] * tempTransform)
		jf0_n[3] = normalize (n.transform[3] * tempTransform)
		tempTransform = copy jf1;
		tempTransform.pos = [0, 0, 0];
		tempTransform = inverse tempTransform;
		jf1_n[1] = normalize (n.transform[1] * tempTransform)
		jf1_n[2] = normalize (n.transform[2] * tempTransform)
		jf1_n[3] = normalize (n.transform[3] * tempTransform)
		
		--calculate the attachment point depending on the chosen attachment point
		--default is 1 (joint center)
		local attachmentPoint0 = n.transform.position * inverse jf0;
		local attachmentPoint1 = n.transform.position * inverse jf1;
		
		if (attachmentPointType == 2 and pbody != undefined) then --body0 center
		(
			attachmentPoint0 = [0, 0, 0];
			attachmentPoint1 = pbody.transform.position * inverse jf1;
		)
		if (attachmentPointType == 3 and cbody != undefined) then --body1 center
		(
			attachmentPoint0 = cbody.transform.position * inverse jf0;
			attachmentPoint1 = [0, 0, 0];		
		)
		if (attachmentPointType == 4) then --custom
		(
			local hadGlobal = false;
			if ( (getUserProp n "GlobalAttachmentPoint") != undefined) then
			(
				local apGlobal = getPoint3Prop n "GlobalAttachmentPoint";
				if (apGlobal != undefined and (classOf apGlobal) == Point3) then
				(
					hadGlobal = true;
					attachmentPoint0 = apGlobal * inverse jf0;
					attachmentPoint1 = apGlobal * inverse jf1;
				)
			)
			if (not hadGlobal) then
			(
				if ((getUserProp n "LocalAttachmentPoint0") != undefined) then
				(
					local temp = getPoint3Prop n "LocalAttachmentPoint0";
					if ((classOf temp) == Point3) then
					(
						attachmentPoint0 = temp;
					)
				)
				if ((getUserProp n "LocalAttachmentPoint1") != undefined) then
				(
					local temp = getPoint3Prop n "LocalAttachmentPoint1";
					if ((classOf temp) == Point3) then
					(
						attachmentPoint1 = temp;
					)
				)
			)
		)
		
		--set various joint properties
		px.pxsetD6JointLocalAxis jointDesc 1 jf0_n[1] jf0_n[2] attachmentPoint0
		px.pxsetD6JointLocalAxis jointDesc 2 jf1_n[1] jf1_n[2] attachmentPoint1

		local values1 = [n.swing1_angle, n.swing1_rest, n.swing1_spring, n.swing1_damp];
		local values2 = [n.swing2_angle, n.swing2_rest, n.swing2_spring, n.swing2_damp];
		local values3 = [n.twist_rest, n.twist_spring, n.twist_damp];
		
		px.pxsetD6JointSwing jointDesc 1 n.swing1_limited n.swing1_locked values1;
		px.pxsetD6JointSwing jointDesc 2 n.swing2_limited n.swing2_locked values2;
		px.pxsetD6JointTwist  jointDesc n.twist_enbl n.twistlow n.twisthigh values3;
	    
	    px.pxsetD6JointLinear jointDesc n.x_state n.y_state n.z_state n.xlate_rad;
	    
	    if(n.projectionMode < 1 or  n.projectionMode > 3) then n.projectionMode = 1
		format " projectionMode=%\n" n.projectionMode
		px.pxsetD6JointProjection jointDesc n.projectionMode n.projectionDist n.projectionAngle;
		
		px.pxsetD6JointCollision jointDesc n.collision
	    px.pxsetD6JointGear jointDesc n.gearing n.gearRatio
	    
		local joint = px.pxaddD6Joint jointDesc;
		px.pxsetD6JointBreakable joint n.breakable n.maxForce n.maxTorque;
	)
    
  )
  
  parameters bodyspec rollout: constraintparams
  (
    body0           type:#node      ui:body0_ui
    body1           type:#node      ui:body1_ui
    
    breakable       type:#boolean   ui:breakable_ui   default: false
    maxForce        type:#float     ui:maxForce_ui    default: 10
    maxTorque       type:#float     ui:maxTorque_ui   default: 10
    
    collision       type:#boolean   ui:collision_ui   default: false
    
    projectionMode  type:#integer   default:1
    projectionDist  type:#float     ui:projdist_ui    default: 0.1
    projectionAngle type:#float     ui:projangle_ui	  default: 0.0872
    
    gearing         type:#boolean   ui:gearing_ui     default: false
    gearRatio       type:#float     ui:gearratio_ui   default: 1
	
	aptype			type:#index		ui:ap_ui		  default: 1
  )

  rollout constraintparams "Constraint" width:162 height:99
  (
    pickbutton    body0_ui      "undefined"         pos:[45,  8]  width: 100  height: 18 message:"" toolTip:"Specify Body0" autoDisplay: true
    label         lbl3_ui       "Body0: "           pos:[ 8, 11]  width: 37   height: 15
    pickbutton    body1_ui      "undefined"         pos:[45, 32]  width: 100  height: 18 message:"" toolTip:"Specify Body1" autoDisplay: true
    label         lbl4_ui       "Body1: "           pos:[ 8, 35]  width: 37   height: 15
	button		  rembody0_ui	"x"					pos:[150,10]  width: 10	  height: 14 toolTip:"Remove Body0"
    button		  rembody1_ui	"x"					pos:[150,34]  width: 10	  height: 14 toolTip:"Remove Body1"
	
    checkbox      breakable_ui  "Breakable"         pos:[ 8, 55]  width:120   height:21 checked:false
    label         maxForce_lb   "Max Force"         pos:[ 8, 77]  width:80    height:14
    spinner       maxForce_ui   ""                  range:[0,10000,1] pos:[90, 75]  width:60    height:16 type:#float
    label         maxTorque_lb  "Max Torque"        pos:[ 8, 97]  width:80    height:14
    spinner       maxTorque_ui  ""                  range:[0,10000,1] pos:[90, 95]  width:60    height:16 type:#float
    
    checkbox      collision_ui  "Collision Enabled" pos:[ 8,115]  width:120   height:21 checked:false
    
    label         projmode_lb   "Projection Mode"   pos:[ 8,139]  width:60    height:14
    dropdownlist  projmode_ui   ""                  pos:[60,135]  width:95    height:21 items:#("none","point_mindist", "linear_mindist")
    label         projdist_lb   "Projection Dist"   pos:[ 8,167]  width:80    height:14
    spinner       projdist_ui   ""                  pos:[90,165]  width:60    height:16 type:#float
    label         projangle_lb  "Projection Angle"  pos:[ 8,187]  width:80    height:14
    spinner       projangle_ui  ""                  pos:[90,185]  width:60    height:16 type:#float
    
    checkbox      gearing_ui    "Gearing"           pos:[ 8,205]  width:60    height:20 checked:false
    label         gearratio_lb  "Gear Ratio"        pos:[ 8,227]  width:80    height:14
    spinner       gearratio_ui  ""                  pos:[90,225]  width:60    height:16 type:#float
	
	label		  attachlbl		"Attachment point:"	align:#left
	dropdownlist  ap_ui		""					pos:[20,262]	items:#("Joint center", "Body0 center", "Body1 center")
	--dropdownlist  ap_ui		""					pos:[20,262]	width:100	items:#("Joint center", "Body0 center", "Body1 center") --, "Custom")
	--button		  apcust_ui	"pick"				pos:[122,262]	tooltip:"Pick an object, from which the transform position will be used as attachment point for the bodies"
	
	fn update_projmode_ui =
	(
		local state = false
		if (projmode_ui.selection != 1) then state = true
		projdist_ui.enabled = state
		projangle_ui.enabled = state
	)
	
	on projmode_ui selected sel do
	(
		projectionMode = projmode_ui.selection
		update_projmode_ui();
	)
	
	on rembody0_ui pressed do
	(
		body0_ui.object = undefined;
	)

	on body0_ui picked obj do
	(
		if (obj != undefined) then
		(
			local actor = px_getPxActor obj;
			if (actor != null) then
			(
				local type = try(getuserprop actor "PhysicsType" as integer) catch(PX_PHYSTYPE_UNDEFINED);
				-- print (format "object type = %\n" type);
				if((type == PX_PHYSTYPE_KINEMATIC) or (type == PX_PHYSTYPE_DYNAMIC))then
				(
					if(body1_ui.object != body0_ui.object) then
					(
						body0_ui.object = actor;
						return 0;
					) else 
					(
						MessageBox "You can not choose a same object!"
					)
				)
			)
		)
		MessageBox "Please choose a Kinematic/Dynamic Rigid Body!";
		body0_ui.object = undefined;
	)

	on rembody1_ui pressed do
	(
		body1_ui.object = undefined;
	)
	
	on body1_ui picked obj do
	(
		if (obj != undefined) then
		(
			local actor = px_getPxActor obj;
			if (actor != null) then
			(
				local type = try(getuserprop actor "PhysicsType" as integer) catch(PX_PHYSTYPE_UNDEFINED);
				-- print (format "object type = %\n" type);
				if(type == PX_PHYSTYPE_DYNAMIC) then
				(
					if(body1_ui.object != body0_ui.object) then
					(
						body1_ui.object = actor;
						return 0;
					) else 
					(
						MessageBox "You can not choose a same object!"
					)
				)
			)
		)
		MessageBox "Please choose a Dynamic Rigid Body!";
		body1_ui.object = undefined;
	)
	
	/*
	on apcust_ui pressed do
	(
		local p = pickobject count:1 select:false message:"Pick object whos center will be used as attachment point for the bodies";
		if (p != undefined) then
		(
			if (p.transform != undefined) then
			(
				ap_ui.selection = 5;
				setUserProp $ "LocalAttachmentPoint0" ""
				setUserProp $ "LocalAttachmentPoint1" ""
				local pGlobal = p.transform.position;
				setUserProp $ "GlobalAttachmentPoint" (pGlobal as string)
				--local body0Transform = body0.transform
				--local pLocal = pGlobal * inverse body0Transform
				--setUserProp $ "attachmentPoint0" (pLocal as string)
			)
		)
	)
	*/

    on constraintparams open do
    (
        mNeedRebuild = true;
		if breakable_ui.state == false then
		(
		  maxForce_ui.enabled   = false;
          maxForce_lb.enabled   = false;
          maxTorque_lb.enabled  = false;
          maxTorque_ui.enabled  = false;
		)
		
		if gearing_ui.state == false then
		(
		  gearratio_lb.enabled   = false;
          gearratio_ui.enabled   = false;
		)

		projmode_ui.selection = projectionMode
		update_projmode_ui()
	
    )
    
    on breakable_ui changed state do
    (
      mNeedRebuild = true;
      case state of
      (
        false:
        (
          maxForce_ui.enabled   = false;
          maxForce_lb.enabled   = false;
          maxTorque_lb.enabled  = false;
          maxTorque_ui.enabled  = false;
        )
        true:
        (
          maxForce_ui.enabled   = true;
          maxForce_lb.enabled   = true;
          maxTorque_lb.enabled  = true;
          maxTorque_ui.enabled  = true;
        )
      )
    )
    
    on gearing_ui changed state do
    (
      mNeedRebuild = true;
      case state of
      (
        false:
        (
          gearratio_lb.enabled   = false;
          gearratio_ui.enabled   = false;
        )
        true:
        (
          gearratio_lb.enabled   = true;
          gearratio_ui.enabled   = true;
        )
      )
    )
  )


  parameters swingtwist rollout: swingtwistparams
  (
    swing1_limited  type: #boolean  ui: swing1_ui_limited default: true
    swing1_locked   type: #boolean  ui: swing1_ui_locked  default: false
    swing1_angle    type: #float    ui: swing1_ui_angle   default: 10
    swing1_rest     type: #float    ui: swing1_ui_rest    default: 0
    swing1_spring   type: #float    ui: swing1_ui_spring  default: 0
    swing1_damp     type: #float    ui: swing1_ui_damp    default: 0

    swing2_limited  type: #boolean  ui: swing2_ui_limited default: true
    swing2_locked   type: #boolean  ui: swing2_ui_locked  default: false
    swing2_angle    type: #float    ui: swing2_ui_angle   default: 10
    swing2_rest     type: #float    ui: swing2_ui_rest    default: 0
    swing2_spring   type: #float    ui: swing2_ui_spring  default: 0
    swing2_damp     type: #float    ui: swing2_ui_damp    default: 0

    twist_enbl      type: #boolean  ui: twist_enbl_chk  default: false
    twistlow        type: #float    ui: twistlow        default: 10
    twisthigh       type: #float    ui: twisthigh       default: 10
    twist_rest      type: #float    ui: twist_rest      default: 0
    twist_spring    type: #float    ui: twist_spring    default: 0
    twist_damp      type: #float    ui: twist_damp      default: 0
    twist_lmt       type: #boolean  ui: twist_lmt_chk   default: true
  )

  parameters displayprops rollout: displaypropsparams
  (
    helpersize  type: #float  ui: helpersize  default: 0.001
  )


  function buildD6JointMesh = 
  (
    if ( (swing1_limited and not swing1_locked) and (swing2_limited and not swing2_locked) ) then
    (
      -- explicitly defined variables
      num_rings             = 6
      sections_per_quadrant = 4
      case of
      (
        ((swing1_angle>150) or (swing2_angle>150)): (num_rings = 12;  sections_per_quadrant = 20)
        ((swing1_angle>130) or (swing2_angle>130)): (num_rings = 11;  sections_per_quadrant = 18)
        ((swing1_angle>110) or (swing2_angle>110)): (num_rings = 10;  sections_per_quadrant = 16)
        ((swing1_angle>90)  or (swing2_angle>90)):  (num_rings = 9;   sections_per_quadrant = 14)
        ((swing1_angle>70)  or (swing2_angle>70)):  (num_rings = 8;   sections_per_quadrant = 12)
        ((swing1_angle>50)  or (swing2_angle>50)):  (num_rings = 7;   sections_per_quadrant = 8)
        ((swing1_angle>30)  or (swing2_angle>30)):  (num_rings = 6;   sections_per_quadrant = 6)
        ((swing1_angle>0)   or (swing2_angle>0)):   (num_rings = 5;   sections_per_quadrant = 4)
      )

      -- derived variables
      swing1step  = swing1_angle/num_rings
      swing2step  = swing2_angle/num_rings
      ring1_verts = #()
      ring2_verts = #()
      face_array  = #()
      vert_count  = 0
      vert_array  = #()
      sections  = (sections_per_quadrant * 4)
      mystep    = 360/sections as float
      ring1_vert1 = 3
      rings   = #()
      -- populate rings array
      for i = 1 to num_rings do
      (
        local a = #()
        append rings a
      )

      ring_build_loop_count = rings.count - 1

      -- base vertex
      append vert_array [0,0,0]; vert_count +=1
      -- top of volume vertex
      append vert_array [helpersize,0,0]; vert_count +=1

      -- generate the rest of the verts
      for b = 1 to num_rings do
      (
        for a = 0 to (360 - mystep) by mystep do
        (
          local s = sin a
          local c = cos a
          local maxAng = (swing1step * b)*(swing2step * b)/((swing1step * b)*s*s+(swing2step * b)*c*c)
          --          local p = [c*(sin maxAng),s*(sin maxAng),(cos maxAng)]*helpersize
          local p = [(cos maxAng),-s*(sin maxAng),c*(sin maxAng)]*helpersize
          append vert_array p;
          vert_count +=1;
          append rings[b] vert_count;
        )
      )

      -- face array for initial dome triangles
      for i = 1 to sections do
      (
        a = 2
        b = i+2
        c = (mod (i) (sections)) + 3
        p = [a,b,c]
        append face_array p
        startstop=b
      )

      -- face array for remaining dome triangles
      for rbl = 1 to ring_build_loop_count do
      (
        for i = 1 to sections_per_quadrant do
        (
          -- quad 1
          local new_poly = [rings[rbl][i],rings[rbl+1][i],rings[rbl+1][i+1]]
          append face_array new_poly
          new_poly = [rings[rbl][i],rings[rbl+1][i+1],rings[rbl][i+1]]
          append face_array new_poly
          -- quad 2
          new_poly = [rings[rbl][i+(sections_per_quadrant)],rings[rbl+1][i+(sections_per_quadrant)],rings[rbl][i+(sections_per_quadrant+1)]]
          append face_array new_poly
          new_poly = [rings[rbl][i+(sections_per_quadrant+1)],rings[rbl+1][i+(sections_per_quadrant)],rings[rbl+1][i+(sections_per_quadrant+1)]]
          append face_array new_poly
          -- quad 3
          new_poly = [rings[rbl][i+(2*sections_per_quadrant)],rings[rbl+1][i+(2*sections_per_quadrant)],rings[rbl+1][i+(2*sections_per_quadrant+1)]]
          append face_array new_poly
          new_poly = [rings[rbl][i+(2*sections_per_quadrant)],rings[rbl+1][i+(2*sections_per_quadrant+1)],rings[rbl][i+(2*sections_per_quadrant+1)]]
          append face_array new_poly
          -- quad 4
          if i<sections_per_quadrant then
          (
            new_poly = [rings[rbl][i+(3*sections_per_quadrant)],rings[rbl+1][i+(3*sections_per_quadrant)],rings[rbl][i+(3*sections_per_quadrant+1)]]
            append face_array new_poly
            new_poly = [rings[rbl][i+(3*sections_per_quadrant+1)],rings[rbl+1][i+(3*sections_per_quadrant)],rings[rbl+1][i+(3*sections_per_quadrant+1)]]
            append face_array new_poly
          )
          else
          (
            new_poly = [rings[rbl][sections],rings[rbl+1][sections],rings[rbl][1]]
            append face_array new_poly
            new_poly = [rings[rbl][1],rings[rbl+1][sections],rings[rbl+1][1]]
            append face_array new_poly
          )
        ) -- end i loop
      ) -- end rbl loop

      -- cone triangles
      for i = 1 to sections do
      (
        if i<(sections) then
        (
          new_poly = [1,rings[num_rings][i+1],rings[num_rings][i]]
          append face_array new_poly
        )
        else
        (
          new_poly = [1,rings[num_rings][1],rings[num_rings][i]]
          append face_array new_poly
        )
      ) -- end i loop
      swing_faces_count = face_array.count
      swing_verts_count = vert_array.count
      if twist_enbl then
      (
        x = 0
        p1 = [x,0,0]
        append vert_array p1
        p2 = [x,0,0]
        append vert_array p2
        for i = (-10) to (10) do
        (
          if i > 0 then
          (
            local a = twistlow * i * 0.1
            y = helpersize * (cos a) * 1.1
            z = helpersize * (sin a) * 1.1
            p1 = [x,y,z]
            append vert_array p1
            p2 = [x,y,z]
            append vert_array p2
          )
          else
          (
            local a = twisthigh * i * 0.1
            y = helpersize * (cos a) * 1.1
            z = helpersize * (sin a) * 1.1
            p1 = [x,y,z]
            append vert_array p1
            p2 = [x,y,z]
            append vert_array p2
          )
          )
          for i = (3 + swing_verts_count) to (41 + swing_verts_count) by 2 do
          (
            append face_array [(1 + swing_verts_count),i,(i+2)]
            append face_array [(2 + swing_verts_count),(i+3),(i+1)]
          )

        )
        setMesh mDisplayMesh vertices: vert_array faces: face_array

        -- set smoothing groups
        s1 = swing_faces_count - sections
        for i = 1 to s1 do
        (
          setfacesmoothgroup mDisplayMesh i 1
        )
        for i = (s1+1) to swing_faces_count do
        (
          setfacesmoothgroup mDisplayMesh i 2
        )

        -- set vertex colors
        defaultVCFaces mDisplayMesh
        if not twist_enbl then
        (
          for i = 1 to face_array.count do
          (
            a = getVCFace mDisplayMesh i
            setVertColor mDisplayMesh a.x (color 128 128 128) -- swing color
            setVertColor mDisplayMesh a.y (color 128 128 128) -- swing color
            setVertColor mDisplayMesh a.z (color 128 128 128) -- swing color
          )
        )
        else
        (
          for i = 1 to face_array.count do
          (
            a = getVCFace mDisplayMesh i
            setVertColor mDisplayMesh a.x (color 128 128 128) -- swing color
            setVertColor mDisplayMesh a.y (color 128 128 128) -- swing color
            setVertColor mDisplayMesh a.z (color 128 128 128) -- swing color
          )
          for i = (swing_faces_count + 1) to face_array.count do
          (
            a = getVCFace mDisplayMesh i
            setVertColor mDisplayMesh a.x (color 200 0 0) -- twist color
            setVertColor mDisplayMesh a.y (color 200 0 0) -- twist color
            setVertColor mDisplayMesh a.z (color 200 0 0) -- twist color
          )
        )
        -- set node display & render properties
        (refs.dependents this)[1].showVertexColors = true
        (refs.dependents this)[1].vertexColorType = #color
        (refs.dependents this)[1].vertexColorsShaded = true
        (refs.dependents this)[1].renderable = false
        (refs.dependents this)[1].xray = true
        (refs.dependents this)[1].wirecolor = (color 96 96 96)

      ) -- end if
      else if ( (swing1_limited == false or swing1_locked == true) and (swing2_limited == true and not swing2_locked == true) ) then
      (
        z = 0
        vert_array = #()
        face_array = #()
        p1 = [0,0,z]
        append vert_array p1
        p2 = [0,0,z]
        append vert_array p2
        for i = (-10) to (10) do
        (
          local a = swing2_angle * i * 0.1
          x = helpersize * (cos a)
          y = helpersize * (sin a)
          p1 = [x,y,z]
          append vert_array p1
          p2 = [x,y,z]
          append vert_array p2
        )
        for i = 3 to 41 by 2 do
        (
          append face_array [1,i,(i+2)]
          append face_array [2,(i+3),(i+1)]
        )
        swing_faces_count = face_array.count
        swing_verts_count = vert_array.count
        if twist_enbl then
        (
          x = 0
          p1 = [x,0,0]
          append vert_array p1
          p2 = [x,0,0]
          append vert_array p2
          for i = (-10) to (10) do
          (
          if i > 0 then
          (
            local a = twistlow * i * 0.1
            y = helpersize * (cos a) * 1.1
            z = helpersize * (sin a) * 1.1
            p1 = [x,y,z]
            append vert_array p1
            p2 = [x,y,z]
            append vert_array p2
          )
          else
          (
            local a = twisthigh * i * 0.1
            y = helpersize * (cos a) * 1.1
            z = helpersize * (sin a) * 1.1
            p1 = [x,y,z]
            append vert_array p1
            p2 = [x,y,z]
            append vert_array p2
          )
        )
        for i = (3 + swing_verts_count) to (41 + swing_verts_count) by 2 do
        (
          append face_array [(1 + swing_verts_count),i,(i+2)]
          append face_array [(2 + swing_verts_count),(i+3),(i+1)]
        )
      )
      setMesh mDisplayMesh vertices: vert_array faces: face_array

      -- set vertex colors
      defaultVCFaces mDisplayMesh
      if not twist_enbl then
      (
        for i = 1 to face_array.count do
        (
          a = getVCFace mDisplayMesh i
          setVertColor mDisplayMesh a.x (color 88 111 225)  -- swing2 color
          setVertColor mDisplayMesh a.y (color 88 111 225)  -- swing2 color
          setVertColor mDisplayMesh a.z (color 88 111 225)  -- swing2 color
        )
      )
      else
      (
        for i = 1 to face_array.count do
        (
          a = getVCFace mDisplayMesh i
          setVertColor mDisplayMesh a.x (color 88 111 225)  -- swing2 color
          setVertColor mDisplayMesh a.y (color 88 111 225)  -- swing2 color
          setVertColor mDisplayMesh a.z (color 88 111 225)  -- swing2 color
        )
        for i = (swing_faces_count + 1) to face_array.count do
        (
          a = getVCFace mDisplayMesh i
          setVertColor mDisplayMesh a.x (color 200 0 0) -- twist color
          setVertColor mDisplayMesh a.y (color 200 0 0) -- twist color
          setVertColor mDisplayMesh a.z (color 200 0 0) -- twist color
        )
      )

      -- set node display and render properties
      (refs.dependents this)[1].showVertexColors = true
      (refs.dependents this)[1].vertexColorType = #color
      (refs.dependents this)[1].vertexColorsShaded = true
      (refs.dependents this)[1].renderable = false
      (refs.dependents this)[1].xray = true
      (refs.dependents this)[1].wirecolor = (color 96 96 96)
    )
    else if ( (swing2_limited == false or swing2_locked == true) and (swing1_limited == true and not swing1_locked == true) ) then
    (
      y = 0
      vert_array = #()
      face_array = #()
      p1 = [0,y,0]
      append vert_array p1
      p2 = [0,y,0]
      append vert_array p2
      for i = (-10) to (10) do
      (
        local a = swing1_angle * i * 0.1
        x = helpersize * (cos a)
        z = helpersize * (sin a)
        p1 = [x,y,z]
        append vert_array p1
        p2 = [x,y,z]
        append vert_array p2
      )
      for i = 3 to 41 by 2 do
      (
        append face_array [1,i,(i+2)]
        append face_array [2,(i+3),(i+1)]
      )
      swing_faces_count = face_array.count
      swing_verts_count = vert_array.count
      if twist_enbl then
      (
        x = 0
        p1 = [x,0,0]
        append vert_array p1
        p2 = [x,0,0]
        append vert_array p2
        for i = (-10) to (10) do
        (
          if i > 0 then
          (
            local a = twistlow * i * 0.1
            y = helpersize * (cos a) * 1.1
            z = helpersize * (sin a) * 1.1
            p1 = [x,y,z]
            append vert_array p1
            p2 = [x,y,z]
            append vert_array p2
          )
          else
          (
            local a = twisthigh * i * 0.1
            y = helpersize * (cos a) * 1.1
            z = helpersize * (sin a) * 1.1
            p1 = [x,y,z]
            append vert_array p1
            p2 = [x,y,z]
            append vert_array p2
          )
        )
        for i = (3 + swing_verts_count) to (41 + swing_verts_count) by 2 do
        (
          append face_array [(1 + swing_verts_count),i,(i+2)]
          append face_array [(2 + swing_verts_count),(i+3),(i+1)]
        )
      )
      setMesh mDisplayMesh vertices: vert_array faces: face_array

      -- set vertex colors
      defaultVCFaces mDisplayMesh
      if not twist_enbl then
      (
        for i = 1 to face_array.count do
        (
          a = getVCFace mDisplayMesh i
          setVertColor mDisplayMesh a.x (color 26 177 26) -- swing1 color
          setVertColor mDisplayMesh a.y (color 26 177 26) -- swing1 color
          setVertColor mDisplayMesh a.z (color 26 177 26) -- swing1 color
        )
      )
      else
      (
        for i = 1 to face_array.count do
        (
          a = getVCFace mDisplayMesh i
          setVertColor mDisplayMesh a.x (color 26 177 26) -- swing1 color
          setVertColor mDisplayMesh a.y (color 26 177 26) -- swing1 color
          setVertColor mDisplayMesh a.z (color 26 177 26) -- swing1 color
        )
        for i = (swing_faces_count + 1) to face_array.count do
        (
          a = getVCFace mDisplayMesh i
          setVertColor mDisplayMesh a.x (color 200 0 0) -- twist color
          setVertColor mDisplayMesh a.y (color 200 0 0) -- twist color
          setVertColor mDisplayMesh a.z (color 200 0 0) -- twist color
        )
      )

      -- set node display and render properties
      (refs.dependents this)[1].showVertexColors = true
      (refs.dependents this)[1].vertexColorType = #color
      (refs.dependents this)[1].vertexColorsShaded = true
      (refs.dependents this)[1].renderable = false
      (refs.dependents this)[1].xray = true
      (refs.dependents this)[1].wirecolor = (color 96 96 96)
    )
    else if ( (swing1_limited == false or swing1_locked == true) and (swing2_limited == false or swing2_locked == true) and twist_enbl == true) then
    (
      face_array = #()
      vert_array = #()
      x = 0
      p1 = [x,0,0]
      append vert_array p1
      p2 = [x,0,0]
      append vert_array p2
      for i = (-10) to (10) do
      (
        if i > 0 then
        (
          local a = twistlow * i * 0.1
          y = helpersize * (cos a) * 1.1
          z = helpersize * (sin a) * 1.1
          p1 = [x,y,z]
          append vert_array p1
          p2 = [x,y,z]
          append vert_array p2
        )
        else
        (
          local a = twisthigh * i * 0.1
          y = helpersize * (cos a) * 1.1
          z = helpersize * (sin a) * 1.1
          p1 = [x,y,z]
          append vert_array p1
          p2 = [x,y,z]
          append vert_array p2
        )
      )
      for i = (3) to (41) by 2 do
      (
        append face_array [(1),i,(i+2)]
        append face_array [(2),(i+3),(i+1)]
      )
      setMesh mDisplayMesh vertices: vert_array faces: face_array

      -- set vertex colors
      defaultVCFaces mDisplayMesh
      for i = (1) to face_array.count do
      (
        a = getVCFace mDisplayMesh i
        setVertColor mDisplayMesh a.x (color 200 0 0) -- twist color
        setVertColor mDisplayMesh a.y (color 200 0 0) -- twist color
        setVertColor mDisplayMesh a.z (color 200 0 0) -- twist color
      )

      -- set node display and render properties
      (refs.dependents this)[1].showVertexColors = true
      (refs.dependents this)[1].vertexColorType = #color
      (refs.dependents this)[1].vertexColorsShaded = true
      (refs.dependents this)[1].renderable = false
      (refs.dependents this)[1].xray = true
      (refs.dependents this)[1].wirecolor = (color 96 96 96)
    )
    else
    (
		--display a box, just to display something
		local size = helpersize;
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
		
			
		setMesh mDisplayMesh vertices: verts faces: faces
    )
  )

  rollout swingtwistparams "Swing & Twist" width:162 height:526
  (
    GroupBox  swing1_ui_group     "Swing1"              pos:[7, 6]    width:147 height:140
    checkbox  swing1_ui_limited   "Limited"             pos:[13,44]   width:120 height:21 checked:true
    checkbox  swing1_ui_locked    "Locked"              pos:[13,24]   width:120 height:21 checked:false
 
    spinner   swing1_ui_angle     "Angle (deg)"         pos:[13,64]   width:120 height:16 range:[0,180,0] type:#float
    spinner   swing1_ui_rest      "Restitution"         pos:[13,84]   width:120 height:16 type:#float
    spinner   swing1_ui_spring    "Spring"              pos:[13,104]  width:120 height:16 range:[0,9999999999,0] type:#float
    spinner   swing1_ui_damp      "Damping"             pos:[13,124]  width:120 height:16 range:[0,1000,0] type:#float
    
    GroupBox  swing2_ui_group     "Swing2"              pos:[7, 156]  width:147 height:140
    checkbox  swing2_ui_limited   "Limited"             pos:[13,194]  width:120 height:21 checked:true
    checkbox  swing2_ui_locked    "Locked"              pos:[13,174]  width:120 height:21 checked:false
    spinner   swing2_ui_angle     "Angle (deg)"         pos:[13,214]  width:120 height:16 range:[0,180,0] type:#float
    spinner   swing2_ui_rest      "Restitution"         pos:[13,234]  width:120 height:16 type:#float
    spinner   swing2_ui_spring    "Spring"              pos:[13,254]  width:120 height:16 range:[0,9999999999,0] type:#float
    spinner   swing2_ui_damp      "Damping"             pos:[13,274]  width:120 height:16 range:[0,1000,0] type:#float

    GroupBox  twist_grp           "Twist"               pos:[7,307]   width:147 height:180
    label     twist_enbl_lbl      "Enabled"             pos:[13,329]  width:42  height:14
    checkbox  twist_enbl_chk      ""                    pos:[59,325]  width:16  height:21 checked:false
    spinner   twist_rest          "Restitution"         pos:[31,348]  width:117 height:16 range:[0,100,0] type:#float enabled:false
    spinner   twist_spring        "Spring"              pos:[51,368]  width:97  height:16 range:[0,9999999999,0] type:#float enabled:false
    spinner   twist_damp          "Damping"             pos:[39,388]  width:109 height:16 range:[0,1000,0] type:#float enabled:false
    GroupBox  twist_lmt_grp       "                  "  pos:[13,414]  width:135 height:66
    label     twist_lmt_lbl       "Limited"             pos:[24,413]  width:34  height:14 enabled:false
    checkbox  twist_lmt_chk       ""                    pos:[63,409]  width:16  height:21 checked:true enabled:false
    spinner   twistlow            "Low (deg)"           pos:[34,438]  width:109 height:16 range:[0,180,10] type:#float enabled:false
    spinner   twisthigh           "High (deg)"          pos:[32,458]  width:111 height:16 range:[0,180,10] type:#float enabled:false

	function updateSwing1UI = 
	(
		local state = (not swing1_locked) and swing1_limited;
		swing1_ui_angle.enabled   = state;
		swing1_ui_rest.enabled    = state;
		swing1_ui_spring.enabled  = state;
		swing1_ui_damp.enabled    = state;
		swing1_ui_locked.enabled  = true;
		swing1_ui_limited.enabled = not swing1_locked;
	)

	function updateSwing2UI = 
	(
		local state = (not swing2_locked) and swing2_limited;
		swing2_ui_angle.enabled   = state;
		swing2_ui_rest.enabled    = state;
		swing2_ui_spring.enabled  = state;
		swing2_ui_damp.enabled    = state;
		swing2_ui_locked.enabled  = true;
		swing2_ui_limited.enabled = not swing2_locked;
	)

	function updateTwistUI = 
	(
		local enabled = twist_enbl_chk.checked;
		local limited = twist_lmt_chk.checked;
		local state = enabled;
		local limitState = enabled and limited;
		twist_lmt_lbl.enabled = state;
		twist_lmt_chk.enabled  = state;
		twistlow.enabled = limitState;
		twisthigh.enabled  = limitState;
		twist_rest.enabled = state;
		twist_spring.enabled = state;
		twist_damp.enabled = state;
	)

    on swingtwistparams open do
    (
        mNeedRebuild = true;
		updateSwing1UI();
		updateSwing2UI();
		updateTwistUI();
    )
	
    on swing1_ui_limited changed state do
    (
		mNeedRebuild = true;
		updateSwing1UI();
    )
  
    on swing1_ui_locked changed state do
    (
		mNeedRebuild = true;	
		updateSwing1UI();
    )
  
    on swing2_ui_limited changed state do
    (
		mNeedRebuild = true;	
		updateSwing2UI();
	)
  
    on swing2_ui_locked changed state do
    (
		mNeedRebuild = true;	
		updateSwing2UI();
    )	

    on twist_enbl_chk changed state do
    (
        mNeedRebuild = true;
		updateTwistUI();
    )
  
    on twist_lmt_chk changed state do
    (
        mNeedRebuild = true;
		updateTwistUI();
    )
    
	on swing1_ui_angle changed val do (mNeedRebuild = true; buildD6JointMesh();)
	on swing1_ui_rest changed val do (mNeedRebuild = true; )
	on swing2_ui_angle changed val do (mNeedRebuild = true; buildD6JointMesh(););
	on swing2_ui_rest changed val do (mNeedRebuild = true; )
	on twistlow changed val do (mNeedRebuild = true; )
	on twisthigh changed val do (mNeedRebuild = true; )
  )
  
  parameters xlate rollout: xlateparams
  (
    x_state   type: #integer  ui: x_range       default: 1
    y_state   type: #integer  ui: y_range       default: 1
    z_state   type: #integer  ui: z_range       default: 1
    xlate_rad type: #float    ui: xlate_radius  default: 45.0
  )

  rollout xlateparams "Translation" width:162 height:130
  (
    label         xlate_locked_lbl  "Locked"                      pos:[18,23]   width: 49   height: 17
    label         xlate_limited_lbl "Limited"                     pos:[18,38]   width: 49   height: 17
    label         xlate_free_lbl    "Free"                        pos:[18,53]   width: 49   height: 17
    GroupBox      xlate_limit_grp   "If Limited, specify Radius"  pos:[7,75]    width: 147  height: 46
    spinner       xlate_radius      "Radius "                     pos:[47,99]   width: 102  height: 16 enabled: false range: [10,179,45] type: #float
    radiobuttons  x_range           ""                            pos:[72,22]   width: 23   height: 48 labels:#("", "", "") columns: 1
    radiobuttons  y_range           ""                            pos:[102,22]  width: 23   height: 48 labels:#("", "", "") columns: 1
    radiobuttons  z_range           ""                            pos:[132,22]  width: 23   height: 48 labels:#("", "", "") columns: 1
    label         lbl16             "X"                           pos:[75,8]    width: 13   height: 14
    label         lbl17             "Y"                           pos:[105,8]   width: 13   height: 14
    label         lbl18             "Z"                           pos:[135,8]   width: 13   height: 14
    
    on x_range changed state do
    (
        mNeedRebuild = true;
		if x_range.state == 2 then
		  xlate_radius.enabled = true;
		else if y_range.state != 2 and z_range.state != 2 then
		  xlate_radius.enabled = false;
    )
    on y_range changed state do
    (
        mNeedRebuild = true;
		if y_range.state == 2 then
		  xlate_radius.enabled = true;
		else if x_range.state != 2 and z_range.state != 2 then
		  xlate_radius.enabled = false;
    )
    on z_range changed state do
    (
        mNeedRebuild = true;
		if z_range.state == 2 then
		  xlate_radius.enabled = true;
		else if x_range.state != 2 and y_range.state != 2 then
		  xlate_radius.enabled = false;
    )
  )   

  rollout displaypropsparams "Display Properties" width: 162 height: 36
  (
    spinner helpersize "Radius " pos:[52,12] width: 102 height: 16 range: [0.001,1000,0.001] type: #float

    on helpersize changed val do
	(
		mNeedRebuild = true;
		delegate.boxsize = [val*10.2,val*10.2,val*10.2];
	)
  )

  on getDisplayMesh do
  (
	if (mDisplayMesh == undefined) then
	(
      mDisplayMesh = TriMesh();
	)

    if (mNeedRebuild) then
    (
      buildD6JointMesh();
      mNeedRebuild = false;
    )

    return mDisplayMesh.mesh;
  )

  -- mouse interface for creation
  tool create
  (
  
    on mousePoint click do
    (
      case click of
      (
        1:  coordsys grid (nodeTM.translation = gridpoint)
      )
    ) -- end mousepoint
    
    on mouseMove click do
    (
	  mNeedRebuild = true;
      case click of
      (
        2:  (helpersize   = abs(gridDist.y);    if helpersize>1000  then helpersize=1000; if helpersize < 0.01 then helpersize = 10;)
        3:  (swing1_angle = abs(gridDist.y*10); if swing1_angle>179 then swing1_angle=179)
        4:  (swing2_angle = abs(gridDist.y*10); if swing2_angle>179 then swing2_angle=179)
        5:  (#stop)
      )
    ) -- end mouseMove
    
  ) -- end create

)
