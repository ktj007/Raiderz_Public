

-- This manipulator is mostly to visualize the parameters of the PhysX 6DOF Joint.

clearselection() -- max can crash if a manipulatable object is selected while running this script

plugin simpleManipulator pxd6jointManip
    name:"PhysXJointManip"
    invisible:true
(
  -- Create the green and red colors for the gizmo
  local greenColor    = colorMan.getColor #manipulatorsActive
  local redColor      = colorMan.getColor #manipulatorsSelected
  local offsetLength  = 0.0  -- Used for mouse tracking
  local mousem

  on canManipulate target return (classof target)== px_D6Joint;
  -- on canManipulateNode n return (classof n)== px_D6Joint;

  -- Create the manipulator gizmo.
  -- This is called initially and whenever the manipulator target changes
  on updateGizmos do
  (
    this.clearGizmos() -- Clear the current gizmo cache
    if not node.isselected then  return "not selected"
    local giz = manip.makeGizmoShape()
    giz.startnewline()
    if(target.swing1<1) then
    (
      for i= -5 to 5 do   
      (
        local a = target.swing2 * i *  0.2
        local p = [(cos a), sin a, 0]*target.point_helper.size 
        giz.addpoint p;     
        giz.addpoint [0,0,0];     
        giz.addpoint p;     
      )
    )
    else if(target.swing2<1) then
    (
      for i= -5 to 5 do   
      (
        local a = target.swing1 * i * 0.2
        local p = [cos a, 0, sin a]*target.point_helper.size 
        giz.addpoint p;     
        giz.addpoint [0,0,0];     
        giz.addpoint p;     
      )
    )
    else 
    (
      for a = 0 to 360 by 15 do
      (
        local s = sin a
        local c = cos a
        local maxAng = target.swing1*target.swing2/(target.swing1*s*s+target.swing2*c*c+0.01);
        local p = [(cos maxAng),s* (sin maxAng),c*(sin maxAng)]*target.point_helper.size 
        giz.addpoint p;     
        giz.addpoint [0,0,0];     
        giz.addpoint p;     
      )
    )
    -- Add the circle to the manipulator
    this.addGizmoShape giz (0) greenColor redColor  

    -- return the ToolTip string
    return node.name + " Swing1=" + target.swing1 as string + " Swing2=" + target.swing2 as string 
  )

  -- mouseMove is called on every mouse move when dragging the manip
  -- It needs to convert the mouse position 'm' into a new value for the radius
  on mouseMove m which do
  (
    local dm = m-mousem;
    mousem=m
    if(which==0) then
    (
      target.swing1 += dm.x
      target.swing2 += dm.y
      if target.swing1>180 then target.swing1=180
      if target.swing1<0   then target.swing1=0
      if target.swing2>180 then target.swing2=180
      if target.swing2<0   then target.swing2=0
    )   
  )
  
  on MouseDown m which do
  (
    offsetLength = 0.0
    mousem=m
  )
)

plugin simpleManipulator pxd6jointtwistManip
    name:"pxd6jointtwistManip"
    invisible:true
(
  -- Create the green and red colors for the gizmo
  local greenColor =  (colorMan.getColor #manipulatorsActive) + [0.6,0,0]
  local redColor =    colorMan.getColor #manipulatorsSelected
  local offsetLength = 0.0  -- Used for mouse tracking
  local mousem

  on canManipulate target return (classof target)== px_D6Joint;
  -- on canManipulateNode n return (classof n)== px_D6Joint;

  -- Create the manipulator gizmo.
  -- This is called initially and whenever the manipulator target changes
  on updateGizmos do
  (
    this.clearGizmos() -- Clear the current gizmo cache
    if not node.isselected then  return "not selected"
    local giz = manip.makeGizmoShape()
    numsteps = (-target.twistlow as integer)/30 + 1
    for i= 0 to numsteps do   
    (
      giz.startnewline()
      local a = target.twistlow * (i as float) / numsteps
      local p = [0,(cos a), sin a]*target.point_helper.size 
      local d = [target.point_helper.size * 0.2,0,0]
      giz.addpoint [0,0,0];     
      giz.addpoint p;     
      giz.addpoint (p-d);     
      giz.addpoint (-d);      
    )
    giz.startnewline()
    for i= 0 to numsteps do   
    (
      local a = target.twistlow * (i as float) / numsteps
      local p = [0,(cos a), sin a]*target.point_helper.size 
      local d = [target.point_helper.size * 0.2,0,0]
      giz.addpoint (p-d);     
    )
    giz.startnewline()
    for i= 0 to numsteps do   
    (
      local a = target.twistlow * (i as float) / numsteps
      local p = [0,(cos a), sin a]*target.point_helper.size 
      giz.addpoint p;     
    )
    this.addGizmoShape giz 0 greenColor redColor  

    local gizhigh = manip.makeGizmoShape()
    numsteps = (target.twisthigh as integer)/30 + 1
    for i= 0 to numsteps do   
    (
      gizhigh.startnewline()
      local a = target.twisthigh * (i as float) / numsteps
      local p = [0,(cos a), sin a]*target.point_helper.size 
      local d = [target.point_helper.size * 0.2,0,0]
      gizhigh.addpoint [0,0,0];     
      gizhigh.addpoint p;     
      gizhigh.addpoint (p-d);     
      gizhigh.addpoint (-d);      
    )
    gizhigh.startnewline()
    for i= 0 to numsteps do   
    (
      local a = target.twisthigh * (i as float) / numsteps
      local p = [0,(cos a), sin a]*target.point_helper.size 
      local d = [target.point_helper.size * 0.2,0,0]
      gizhigh.addpoint (p-d);     
    )
    gizhigh.startnewline()
    for i= 0 to numsteps do   
    (
      local a = target.twisthigh * (i as float) / numsteps
      local p = [0,(cos a), sin a]*target.point_helper.size 
      gizhigh.addpoint p;     
    )
    this.addGizmoShape gizhigh 0 greenColor redColor  


    -- return the ToolTip string
    return node.name + " TwistLimits=" + target.twistlow as string + 
            " to " + target.twisthigh as string 
  )

  on mouseMove m which do
  (
    local dm = m-mousem;
    mousem=m
    if (which==0) then  target.twistlow  -= dm.x
    if (which==1) then  target.twisthigh += dm.x
    if target.twistlow  < -180  then target.twistlow  = -180
    if target.twistlow  > 0     then target.twistlow  =  0
    if target.twisthigh > 180   then target.twisthigh =  180
    if target.twisthigh < 0     then target.twisthigh =  0
  )

  on MouseDown m which do
  (
    offsetLength = 0.0
    mousem=m
  )
)



function gizsquare s d1 d2 = 
(
  local giz = manip.makeGizmoShape()
  for i= 1 to 2 do 
  (
    giz.startnewline()
    giz.addpoint (s + d1 * (i/3.0))
    giz.addpoint (s + d1 * (i/3.0) + d2 )
    giz.startnewline()
    giz.addpoint (s + d2 * (i/3.0))
    giz.addpoint (s + d2 * (i/3.0) + d1 )
  )
  return giz
)

function gizsqline giz s d1 d2 =
(
  giz.startnewline()
  giz.addpoint s
  giz.addpoint (s+d1)
  giz.addpoint (s+d1+d2)
  giz.addpoint (s+d2)
  giz.addpoint s
) 
function gizbounds lmin lmax = 
(
  local giz = manip.makeGizmoShape()
  local dx = [(lmax-lmin).x,0,0]
  local dy = [0,(lmax-lmin).y,0]
  local dz = [0,0,(lmax-lmin).z]
  gizsqline giz lmin dx dy
  gizsqline giz lmin dy dz
  gizsqline giz lmin dz dx
  gizsqline giz lmax -dx -dy
  gizsqline giz lmax -dy -dz
  gizsqline giz lmax -dz -dx
  return giz
)


plugin simpleManipulator pxd6jointlinearlimitManip
    name:"pxd6jointlinearlimitManip"
    invisible:true
(
  -- Create the green and red colors for the gizmo
  local greenColor = ( colorMan.getColor #manipulatorsActive) * 0.8 + [0,0,0.5]
  local redColor =    colorMan.getColor #manipulatorsSelected
  local offsetLength = 0.0  -- Used for mouse tracking
  local mousem

  on canManipulate target return (classof target)== px_D6Joint;
  -- on canManipulateNode n return (classof n)== px_D6Joint;

  -- Create the manipulator gizmo.
  -- This is called initially and whenever the manipulator target changes
  on updateGizmos do
  (
    this.clearGizmos() -- Clear the current gizmo cache
    if not node.isselected then  return "not selected"
    local lmin = [target.mipx,target.miny,target.minz]
    local lmax = [target.maxx,target.maxy,target.maxz]
    local range = lmax-lmin
    -- local giz = manip.makebox  ((lmin+lmax)/2) range.x range.y range.z 3 3 3
    this.addGizmoShape (gizsquare lmin [0, range.y,0]  [0,0, range.z])  0 greenColor redColor  
    this.addGizmoShape (gizsquare lmax [0,-range.y,0]  [0,0,-range.z])  0 greenColor redColor  
    this.addGizmoShape (gizsquare lmin [ range.x,0,0]  [0,0, range.z])  0 greenColor redColor  
    this.addGizmoShape (gizsquare lmax [-range.x,0,0]  [0,0,-range.z])  0 greenColor redColor  
    this.addGizmoShape (gizsquare lmin [0, range.y,0]  [ range.x,0,0])  0 greenColor redColor  
    this.addGizmoShape (gizsquare lmax [0,-range.y,0]  [-range.x,0,0])  0 greenColor redColor  
    this.addGizmoShape (gizbounds lmin lmax) 2 greencolor redcolor
    -- return the ToolTip string
    return node.name + " limitmin=" + (lmin as string) + " limitmax=" + (lmax as string) 
  )

  -- mouseMove is called on every mouse move when dragging the manip
  -- It needs to convert the mouse position 'm' into a new value for the radius
  on mouseMove m which do
  (
    local dm = m-mousem;
    mousem=m
    --print ("linearmanip input"+(m as string) + " " + (which as string));
    if(which==0) then target.mipx += dm.x 
    if(which==1) then target.maxx += dm.x 
    if(which==2) then target.miny += dm.x 
    if(which==3) then target.maxy += dm.x 
    if(which==4) then target.minz += dm.x 
    if(which==5) then target.maxz += dm.x 
    if(target.mipx>0) then target.mipx=0
    if(target.miny>0) then target.miny=0
    if(target.minz>0) then target.minz=0
    if(target.maxx<0) then target.maxx=0
    if(target.maxy<0) then target.maxy=0
    if(target.maxz<0) then target.maxz=0
  )

  on MouseDown m which do
  (
    offsetLength = 0.0
    mousem=m
  )
)


