-- 
--  PhysX
--
--  Joint editor
--  To allow for easy creation of joints using existing animation data and parent-child relationships
--
  

--filein "px_base.ms"

function calcjointlimits n quatlist =
(
  if(quatlist.count <2) then
  (
    print "not enough keys to extract any useful info"; -- need a messagebox here?  Some people dont like too many popup alerts dialogs in their face.
    return [0,-45,45]; 
  )
  axis = getpoint3prop n "pmljointaxis" 
  nrml = getpoint3prop n "pmljointnrml"
  na = nrml
  nloc = nrml * (conjugate(quatlist[1]) as Matrix3) 
  local limithigh=0
  local limitlow=0
  local theta=0;
  local q
  for q in quatlist do
  (
    nb = nloc *(q as Matrix3) 
    dp = dot na nb 
    if(dp> 1.0) then dp =  1.0 ;
    if(dp<-1.0) then dp = -1.0 ;
    delta = acos(dp)
    cp = cross na nb ;
    if((dot axis cp) <0) then
    (
      delta = -delta;
    )
    theta = theta + delta
    if(theta > limithigh) then 
    (
      limithigh = theta
    )
    if(theta < limitlow) then 
    (
      limitlow = theta
    )
    na = nb;
  )
  slidertime = 0;
  if(limithigh-limitlow > 315) then
  (
    return [0,limitlow,limithigh]
  )
  return [1,limitlow,limithigh]
)

function grabrotations n =
(
  -- we might consider adding more slidertime samples later when our algo is more intelligent
  local quatlist = #()  
  for k in n.rotation.controller.keys do
  (
    slidertime = k.time
    append quatlist n.controller.rotation
  )
  slidertime = 0
  return quatlist
)


function calcjoint n =
(
  --if(n.parent == undefined) then
  --(
    -- todo: its possible that we want to be attached to the world here
    -- return 0;
  --)
  if(n.rotation.controller.keys.count <2) then
  (
    return 0; -- not enough keys to abstract any useful info
  )
  local quatlist = grabrotations n
  local r = quatlist[2] / quatlist[1]
  setuserprop n "pmljointaxis" r.axis
  cp  = cross r.axis [0,0,1] 
  cpb = cross r.axis [0,1,0] 
  if(length(cpb)>length(cp)) then
  (
    cp = cpb
  )
  nrml = normalize ( cp )
  setuserprop n "pmljointnrml" nrml
  setuserprop n "pmljointtype" 3
  slidertime = 0;
  
  limits = calcjointlimits n quatlist
  setuserprop n "pmljointlimits" limits.x
  if(limits.x>0) then
  (
    setuserprop n "pmljointtwistlow" limits.y
    setuserprop n "pmljointtwisthigh" limits.z
    setuserprop n "pmljointswing1" 0
    setuserprop n "pmljointswing2" 0
  )
)

function calcd6jointlimits n =
(
  local twistlow  = 0
  local twisthigh = 0
  local swing1    = 0
  local swing2    = 0
  
  if (classOf n == BoneGeometry or classOf n == Bone) then 
  (
	if (classOf n.controller == IKControl) then
	(
	  px.pxcalcD6jointfromIK n;
      setuserprop n "pmljointaxis" [0,0,1]  -- pretty much a max standard to put z up
      setuserprop n "pmljointnrml" [1,0,0] -- so axis X nrml=[0,1,0]
    )

  )

)
  

function calcspherejointlimits n =
(
  -- there is no one "correct" unique method to implement this
  local twistlow  = 0
  local twisthigh = 0
  local swing1    = 0
  local swing2    = 0
  
  local quatlist = grabrotations n
  local q
  if(quatlist.count <2) then px_warning ("Trying to create sphere joint on non-animated node " + n.name) 
  else for q in quatlist do
  (
    local e = (q/quatlist[1]) as eulerangles
    e.x = abs e.x
    e.y = abs e.y
    if e.x > swing1    then swing1    = e.x -- did I get x & y mixed up?
    if e.y > swing2    then swing2    = e.y 
    if e.z < twistlow  then twistlow  = e.z
    if e.z > twisthigh then twisthigh = e.z
  )
  setuserprop n "pmljointtwistlow" twistlow
  setuserprop n "pmljointtwisthigh" twisthigh
  setuserprop n "pmljointswing1" swing1
  setuserprop n "pmljointswing2" swing2
  setuserprop n "pmljointaxis" [0,0,1]  -- pretty much a max standard to put z up
  setuserprop n "pmljointnrml" [1,0,0] -- so axis X nrml=[0,1,0]
  setuserprop n "pmljointtype" 4
  setuserprop n "pmljointlimits" 1

)

function clearjoint n =
(
  -- remove any physx user data from node
  setuserprop n "pmljointtype" 0
  setuserprop n "pmljointlimits" 0
  setuserprop n "pmljointtwistlow" 0
  setuserprop n "pmljointtwisthigh" 0
  setuserprop n "pmljointswing1" 0
  setuserprop n "pmljointswing2" 0
  local c
  return true;
)


utility px_Joint_Editor "Joint Specification"  
(
    label         sel               "Selected: (current selection)"   align:#left
    label         plabel            "Parent:            "             align:#left
    radiobuttons  jointtype         "Joint Type"                      labels:#("none (detached)","locked (fixed to parent)","revolute (hinge)","spherical (3dof)") default:0 align:#left
    edittext      axis              "Axis" enabled:false
    edittext      nrml              "Nrml" enabled:false
    checkbox      jointlimits       "Enable Joint Limits"
    -- angle         jointtwistlow     "TwistL"                          diameter:40 range:[-180,0,-45]  across:2
    -- angle         jointtwisthigh    "TwistH"                          diameter:40 range:[0,180,45] 
    -- angle         jointswing1       "Swing1"                          diameter:40 range:[0,180,45] across:2
    -- angle         jointswing2       "Swing2"                          diameter:40 range:[0,180,45] 
    spinner         jointtwistlow     "TwistL"                          diameter:40 range:[-180,0,-45]
    spinner         jointtwisthigh    "TwistH"                          diameter:40 range:[0,180,45] 
    spinner         jointswing1       "Swing1"                          diameter:40 range:[0,180,45]
    spinner         jointswing2       "Swing2"                          diameter:40 range:[0,180,45] 
  --checkbox      breakcheck        "Breakable" 
  --spinner       linearspin        "Linear"                          range:[0,100000,10.0]
  --spinner       angularspin       "Angular"                         range:[0,100000,10.0]
    button        calchj            "Calc Hinge Joint from Anim"      width:145 height:15 tooltip:"" align:#center
    button        calcsj            "Calc Sphere Joint from Anim"     width:145 height:15 enabled:false align:#center
    button        calcik            "Calc Joint Props from IK"        width:145 height:15 enabled:false align:#center
    button        clearj            "Clear All Joint Properties"      width:145 height:15 align:#center
    local         current_bodies
    local         current_body

    function grabnodejstate n =
    (
      if( n== undefined) then return 0;
      -- updates the UI with the node's user data
      if ((getuserprop n "pmljointtype") != undefined) then
      (
        --format "% jointtype %\n" n.name (getuserprop n "pmljointtype" as integer)
        jointtype.state = (getuserprop n "pmljointtype") as integer 
      )
      else jointtype.state = 0;
      if((getuserprop n "pmljointaxis") != undefined) then
      (
        axis.text = getuserprop n "pmljointaxis"
      )
      else axis.text = ""
      if((getuserprop n "pmljointnrml") != undefined) then
      (
        nrml.text = getuserprop n "pmljointnrml"
      )
      else nrml.text = ""
      if ((getuserprop n "pmljointlimits") != undefined) then
      (
        jointlimits.checked = ( (getuserprop n "pmljointlimits" as integer) > 0)
      )
      else 
		jointlimits.checked = false
		
      if((getuserprop n "pmljointtwistlow")   != undefined) then
          jointtwistlow.value =  (getuserprop n "pmljointtwistlow"  as float);
      else
			jointtwistlow.value = 0;
      if((getuserprop n "pmljointtwisthigh")  != undefined) then
          jointtwisthigh.value= (getuserprop n "pmljointtwisthigh" as float);
      else 
		jointtwisthigh.value=0;
      if((getuserprop n "pmljointswing1")   != undefined) then
          jointswing1.value=  (getuserprop n "pmljointswing1"  as float);
      else 
		jointswing1.value=0;
      if((getuserprop n "pmljointswing2")  != undefined) then
          jointswing2.value= (getuserprop n "pmljointswing2" as float);
      else 
		jointswing2.value=0;
    )
    on jointtype changed jt do
    (
      local n
      for n in current_bodies do
      (
        setuserprop n "pmljointtype" jt
        if(jt>2) then
        (
          if((getuserprop n  "pmljointaxis") == undefined) then
            setuserprop n  "pmljointaxis" [0,0,1];
          if((getuserprop n  "pmljointnrml") == undefined) then
            setuserprop n  "pmljointnrml" [1,0,0];
          if((getuserprop n  "pmljointtwistlow") == undefined) then
            setuserprop n  "pmljointtwistlow" jointtwistlow.value;
          if((getuserprop n  "pmljointtwisthigh") == undefined) then
            setuserprop n  "pmljointtwisthigh" jointtwisthigh.value;
          if((getuserprop n  "pmljointswing1") == undefined) then
            setuserprop n  "pmljointswing1" jointswing1.value;
          if((getuserprop n  "pmljointswing2") == undefined) then
            setuserprop n  "pmljointswing2" jointswing2.value;
          if(current_bodies.count==1) then grabnodejstate n;
        )
      )
    )
    on axis entered txt do
    (
      -- renormalize
    )
    on clearj pressed do
    (
      for n in current_bodies do ( clearjoint n;)
      grabnodejstate current_body
    )
    on calchj pressed do
    (
      local n
      for n in current_bodies do (calcjoint n;)
      grabnodejstate current_body;
    )
    on calcsj pressed do
    (
      local n
      for n in current_bodies do (calcspherejointlimits n;)
      grabnodejstate current_body;
    )
    on calcik pressed do
    (
      local n
      for n in current_bodies do (calcd6jointlimits n;)
      grabnodejstate current_body;
    )
    on jointlimits changed  junkstate do
    (
      for n in current_bodies do setuserprop n "pmljointlimits"   (jointlimits.tristate)
    )
    on jointtwistlow changed junkval do
    (
      for n in current_bodies do setuserprop n "pmljointtwistlow"  jointtwistlow.value
    )
    on jointtwisthigh changed junkval do
    (
      for n in current_bodies do setuserprop n "pmljointtwisthigh" jointtwisthigh.value
    )
    on jointswing1 changed junkval do
    (
      for n in current_bodies do setuserprop n "pmljointswing1"    jointswing1.value
    )
    on jointswing2 changed junkval do
    (
      for n in current_bodies do setuserprop n "pmljointswing2"    jointswing2.value
    )
    function enableizer s =
    (
      jointtype.enabled = s
      jointlimits.enabled = s
      jointtwistlow.enabled = s  
      jointtwisthigh.enabled = s 
      jointswing1.enabled = s 
      jointswing2.enabled = s 
      calchj.enabled = s  
      calcsj.enabled = s  
      calcik.enabled = s  
      clearj.enabled = s  
    )
    function updateselection =
    (
      local s;
      current_bodies=#()
      current_body=undefined
      plabel.caption = "Parent: (NA)"
      if $ == undefined  then
      (

      )
      else if(classof $ != ObjectSet) then
      (
        if (IsRootRigidBody $) then append current_bodies $
      )
      else
      (
        local n
        for n in $ do
        (
          if (isrootrigidbody n) then append current_bodies n
        )
      )   
      if(current_bodies.count==0) then
      (
        sel.text = "Selected: (no rigid bodies)"
        plabel.caption = "Parent: " + (if(current_bodies.count==0) then "NA (empty selection)" else "(multiple selection)")
      )
      else if(current_bodies.count==1) then 
      (
        current_body = current_bodies[1];
        sel.text = "Selected: " + current_body.name
        plabel.caption = (if (current_body.parent == undefined) then "Parent: <None> (using world)" else ("Parent: " + current_body.parent.name))
      )
      else 
      (
        sel.text = "Selected: " + (current_bodies.count as string) + " bodies" 
        plabel.caption = "Parent: (multiple selection)"
      )
      enableizer (current_bodies.count>0)
      grabnodejstate current_body;
    )
      
    on px_Joint_Editor open do
    (
      callbacks.addScript     #selectedNodesPostDelete "px_Joint_Editor.updateselection()" id:#px_JOINT
      callbacks.addScript     #selectionSetChanged     "px_Joint_Editor.updateselection()" id:#px_JOINT
      updateselection()
    )
    on px_Joint_Editor close do
    (
      callbacks.removeScripts #selectedNodesPostDelete id:#px_JOINT
      callbacks.removeScripts #selectionSetChanged     id:#px_JOINT
    )
)


callbacks.removeScripts #selectedNodesPostDelete id:#px_JOINT
callbacks.removeScripts #selectionSetChanged     id:#px_JOINT

