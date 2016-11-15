-- 
--  PhysX Physics Utilities
--
--  basic support functions common to all modules
--
--


global px_showwarnings = true;
function px_warning s =
(
  if px_showwarnings then messagebox s;
  return 0;
)

function px_getshapeflag node flagname =
(
	if (node == undefined) then return false;
	local result = try(
		tempVal = getUserProp node flagname;
		tempVal != undefined and tempVal != false;
	) catch (false);
	return result;
)

function px_setshapeflag node flagname value =
(
	if (node == undefined) then return false;
	if (not (classOf(value) == BooleanClass)) then return false;
	setUserProp node flagname value;
	--Also call the plugin to set the shape flag, if the shape has already been instanced
	px.pxsetshapeflag node flagname value;
	return true;
)

function findroot n =
(
  if(n==undefined) then return undefined;
  while(n.parent != undefined) do
  (
    n=n.parent
  )
  return n;
)

function findobject s =
(
  for o in objects do if (o.name == s) then return o; 
  return undefined; 
)

function ripspaces s = 
(
  local i
  for i=1 to s.count do if s[i]==" " then s[i]="_"
  s
)

function vmin a b = 
(
  -- i dont like to have to reimplement this but i didn't find an existing version
  local c = a
  if b.x < c.x then c.x = b.x
  if b.y < c.y then c.y = b.y
  if b.z < c.z then c.z = b.z
  return c;
)
function vmax a b = 
(
  local c = a
  if b.x > c.x then c.x = b.x
  if b.y > c.y then c.y = b.y
  if b.z > c.z then c.z = b.z
  return c;
)


function getpoint3prop n s=
(
  -- handy routine to convert a user property's ascii value to a point3
  local a = getuserprop n s
  if(a==undefined) then return [0,0,0];
  local v = filterstring a " ,[]\r\n"
  return [v[1] as float,v[2] as float,v[3] as float]
)


function isbiped n = 
(
  local c = classof n;
  return (c==Biped_Object) or (c==TriMeshGeometry)
)
function px_isshape n = 
(
  --if (classof n) == Biped_Object then return false
  if (superclassof n) !=GeometryClass then return false
  if n.modifiers["skin"] != undefined then return false
  return true;
)
function IsRigidBody n =
(
  if (isgrouphead  n) then return true;
  if (px_isshape n) then return true;
  return false;
)

function IsRootRigidBody n =
(
  if (isgroupmember n) then return false;
  return isrigidbody(n);
)

function isproxy n =
(
  -- if this geometry serves a a proxy for its parent.
  if(n.parent==undefined) then return false;
  sg = getuserprop n.parent "Simulation_Geometry"
  if(sg==undefined) then return false;
  pg = getuserprop n.parent "Proxy_Geometry"
  if(pg==undefined) then return false
  if(pg!=n.name) then return false
  if(not (sg==3 or sg==6)) then messagebox ("Warn: Proxy Geometry " + n.name + " is not being used by parent "+n.parent.name)
  return true;
)
function getproxy n =
(
  local pg= getuserprop n "Proxy_Geometry"
  if(pg ==undefined or pg=="") then  return undefined
  local pobj = findobject pg;
  --if not (px_isshape pobj) then return undefined
  return pobj
)

function getposition n =
(
  if (classof n.controller) == BipSlave_Control then return (biped.gettransform n #pos)  -- * px_exportscale
  if (isbiped n) then return [0,0,0]
  local p
  try ( p= n.position;) catch (p=[0,0,0])
  return p  -- * px_exportscale
)


function getrotation n =
(
  if (classof n.controller) == BipSlave_Control then return conjugate(biped.gettransform n #rotation) -- things were oreinted wrong so trying the conj
  if  (isbiped n) then return (quat 0 0 0 1)
  local q
  try ( q= n.rotation;) catch (q=(quat 0 0 0 1))
  return q 
)


function getpositionlocal n =
(
  local p = [0,0,0]
  try ( p=n.controller.position ) catch (
    if(n.parent == undefined) then 
    (
      p= getposition n;
    )
    else
    (
      local pp = getposition n.parent
      local pr = getrotation n.parent
      local mp = getposition n
      p= (mp-pp) * ((pr) as matrix3) 
    )
  )
  return p
)


function getrotationlocal n =
(
  local q=(quat 0 0 0 1)
  try( q = conjugate n.controller.rotation) catch (
    if(n.parent == undefined) then 
    (
      q = getrotation n;
    )
    else
    (
      local pr = getrotation n.parent
      local mr = getrotation n
      q= (conjugate pr) * mr 
    )
  )
  return  q
)
function getscale n =
(
  local s = [1,1,1] ;
  if (classof n.controller) == BipSlave_Control then return s * (biped.gettransform n #scale)
  if (isbiped n) then return s
  try (s=n.scale) catch ()
  return s
)

function px_boxvol m =
(
  local i,bmin,bmax
  if (m.numverts==0) then return 0;
  bmin=(getvert m 1)
  bmax=(getvert m 1)
  for i=1 to m.numverts do 
  (
    local v = (getvert m i) 
    bmin = (vmin bmin v)
    bmax = (vmax bmax v)
  )
  local dif = (bmax-bmin) -- * px_exportscale
  return  dif.x*dif.y*dif.z
)

function px_calcvolume n = 
(
  return px_boxvol n.mesh
)

function selectedarray =
(
  if (classof $) == objectset then return $
  if $==undefined then return #()
  else return #( $ )
)

  function convertLegacyJoint legacyD6 =
  (
		if (not iskindof legacyD6 px6DOF) then
		(
		   return undefined;
		)
		
		local newPxJoint
		conversionMsg = "Converting " + legacyD6.name;
		print conversionMsg

		-- figure out which type of attachment point we should calculate
		local attachmentPointType = 1;
		if (legacyD6.aptype != undefined) then
		(
			attachmentPointType = legacyD6.aptype - 1;
			if (attachmentPointType > 4) then
			(
				attachmentPointType = 1;
			)
		)

		newPxJoint = pxJoint name:(legacyD6.name + "converted");
		
		newPxJoint.transform = legacyD6.transform;
		
		newPxJoint.body0 = legacyD6.body0;
		newPxJoint.body1 = legacyD6.body1;
		newPxJoint.aptype = attachmentPointType;
		newPxJoint.swing1_angle = legacyD6.swing1_angle;
		newPxJoint.swing1_rest = legacyD6.swing1_rest;
		newPxJoint.swing1_spring = legacyD6.swing1_spring;
		newPxJoint.swing1_damp = legacyD6.swing1_damp;
		newPxJoint.swing1_locked = legacyD6.swing1_locked;
		newPxJoint.swing1_limited = legacyD6.swing1_limited;
		newPxJoint.swing2_angle = legacyD6.swing2_angle;
		newPxJoint.swing2_rest = legacyD6.swing2_rest;
		newPxJoint.swing2_spring = legacyD6.swing2_spring;
		newPxJoint.swing2_damp = legacyD6.swing2_damp;
		newPxJoint.swing2_locked = legacyD6.swing2_locked;
		newPxJoint.swing2_limited = legacyD6.swing2_limited;
		newPxJoint.twist_rest = legacyD6.twist_rest;
		newPxJoint.twist_spring = legacyD6.twist_spring;
		newPxJoint.twist_damp = legacyD6.twist_damp;
		newPxJoint.twist_enbl = legacyD6.twist_enbl;
		newPxJoint.twistlow = legacyD6.twistlow;
		newPxJoint.twisthigh = legacyD6.twisthigh;
		newPxJoint.twist_lmt = legacyD6.twist_lmt;
		newPxJoint.x_state = legacyD6.x_state;
		newPxJoint.y_state = legacyD6.y_state;
		newPxJoint.z_state = legacyD6.z_state;
		newPxJoint.xlate_rad = legacyD6.xlate_rad;
		if (legacyD6.projectionMode != undefined) then
			newPxJoint.projectionMode = legacyD6.projectionMode;
		newPxJoint.projectionDist = legacyD6.projectionDist;
		newPxJoint.projectionAngle = legacyD6.projectionAngle;
		newPxJoint.collision = legacyD6.collision;
		newPxJoint.gearing = legacyD6.gearing;
		newPxJoint.gearRatio = legacyD6.gearRatio;
		newPxJoint.breakable = legacyD6.breakable;
		newPxJoint.maxForce = legacyD6.maxForce;
		newPxJoint.maxTorque = legacyD6.maxTorque;
		newPxJoint.helpersize = legacyD6.helpersize;

		delete legacyD6;
		return newPxJoint
  )

function px_addjoint n =
(
  local t = getuserprop n "pmljointtype" 
  if(t==undefined or (t as integer)<2 ) then return 0;
  local axis = (getpoint3prop n "pmljointaxis") 
  local nrml = (getpoint3prop n "pmljointnrml") 
  local limits = [180,180,-180,180] 
  local translimitmin = (getpoint3prop n "pxtranslimitmin")
  local translimitmax = (getpoint3prop n "pxtranslimitmax")
  if(t==3) then
  (
    limits = rotlimitmin = [0,0,-180,180] 
  )
  haslimits = (getuserprop n "pmljointlimits")
  if(haslimits!= undefined and (haslimits as integer)!=0) then
  (
    local s1 = 0
    local s2 = 0
    local mn = getuserprop n "pmljointtwistlow"  
    local mx = getuserprop n "pmljointtwisthigh" 
    if(t==4) then
    (
      s1 = getuserprop n "pmljointswing1"
      s2 = getuserprop n "pmljointswing2"
    )
    limits =  [ s1, s2,mn,mx] 
  )
  if(t==2) then
  (
    limits = [0,0,0,0] 
  )
  px.pxaddjoint n axis nrml limits translimitmin translimitmax
)

function px_addjointnode n =
(
  --local jf0 = n.transform * (if(n.pnode!=undefined) then (inverse n.pnode.transform) else matrix3(1))
  --local jf1 
  --if(n.jf1b == 1) then jf1 = matrix3(1)
  --else jf1 = n.transform * (if(n.cnode!=undefined) then (inverse n.cnode.transform) else matrix3(1))
  --local limits = (matrix3 [n.twistlow,-n.swing1,-n.swing2] [n.twisthigh,n.swing1,n.swing2] [n.mipx,n.miny,n.minz] [n.maxx,n.maxy,n.maxz])
  --local drive  = (matrix3  [n.tspring,n.rdamp,0]  [n.sspring,n.rdamp,0] [n.rspring,n.rdamp,0] [n.lspring,n.ldamp,0])
  --px.pxaddjointnode n n.pnode n.cnode jf0 jf1 limits drive
  --px.setjointdriver n n.dnode (n.drst-1)
  n.addToScene n;
)

function px_add_cloth n =
(
  try(
    local cname = classOf(n) as string;
    if(cname != "Editable_mesh") then (
      convertToMesh n;     -- make it as editable mesh first
      print (format "PhysX Notice: % is converted to Editable Mesh automatically! It is required to make a PhysX cloth.\n" n.name);
    ) 
    
    local t= try(getUserProp n "PhysicsType" as integer) catch (PX_PHYSTYPE_DEFAULT)
    if(t!=PX_PHYSTYPE_CLOTH) then return undefined; -- Not cloth!
  
    px.pxaddcloth n false;
    
  ) catch(
    print ("PhysX Warning: Fail to make the mesh as PhysX cloth!\n");
  );
)

function px_add_metal_cloth n =
(
  try(
    local cname = classOf(n) as string;
    if(cname != "Editable_mesh") then (
      convertToMesh n;     -- make it as editable mesh first
      print (format "PhysX Notice: % is converted to Editable Mesh automatically! It is required to make a PhysX metal cloth.\n" n.name);
    ) 
    
    local t= try(getUserProp n "PhysicsType" as integer) catch (PX_PHYSTYPE_DEFAULT)
    if(t!=PX_PHYSTYPE_METAL_CLOTH) then return undefined; -- Not cloth!
  
    px.pxaddcloth n true;
    
  ) catch(
    print ("PhysX Warning: Fail to make the mesh as PhysX metal cloth!\n");
  );
)

function px_add_fluid n =
(
	if ((classOf(n) as string) != "PxFluid") then return undefined;
	--if (n.mNode == undefined) then n.mNode = n;
	if (n.BoundToNode == undefined) then n.BoundToNode = n;
	px.pxaddfluid n;
)

function px_add_fluidemitter n =
(
	if ((classOf(n) as string) != "PxEmitter") then return undefined;
	px.pxaddfluidemitter n;
)

function px_add_softbody n = 
(
	return px.pxAddSoftBody n;
)

function px_add_forcefield n = 
(
	return px.pxAddForceField n;
)

function px_add n =
(
  local v = undefined
  local t= try(getUserProp n "PhysicsType" as integer) catch (PX_PHYSTYPE_DEFAULT)
  if(t==PX_PHYSTYPE_UNDEFINED) then  -- disabled
  (
    return v; -- non-physical
  )
  else if not (isrootrigidbody n) then
  (
    return v;
  )
  else
  (
    local name = n.name
	local selectIt = false
    for i in $selection while (not selectIt) do
    (
		if (i == n) then selectIt = true
    )
    local proxyname = getuserprop n "Proxy_Geometry"
    local proxy = undefined 
    if proxyname != undefined then 
    (
		proxy = findobject proxyname
		-- if proxy != undefined then pxTools.RemoveScales proxy   -- process proxy
	)
	local isBiped = pxTools.isBipedNode n
	local v = undefined
	--if isBiped then v = n else v = pxTools.RemoveScales n
	v = n
	if selectIt then selectMore v
	--format "proxy = % v = %\n" proxy v
	if v != undefined then
	(
		try
		(
			if (proxy != undefined) then
			(
				--local proxyPivot = pxTools.getNodePivot proxy
				--pxTools.alignPivotNodeAndProxy v proxy
				px.pxadd v
				--pxTools.setNodePivot proxy proxyPivot
			)
			else
			(
				--format "not proxy node.\n"
				px.pxadd v;
			)
			if(t==PX_PHYSTYPE_KINEMATIC or t==PX_PHYSTYPE_STATIC) then
			(
			  px.setdynamic v 0;
			)
		) catch ()
	)
	else
	(
		format "Warning: after scaling node %, it becomes undefined\n" name
	)
  )
  return v
)

function eraseprop n p =
(
  local a = filterstring (getuserpropbuffer n) "\n"
  local s
  local pb=""
  for s in a do if (findstring s (p + " =") )==undefined then pb += (s + "\n");
  setuserpropbuffer n pb
)

function px_onSystemReset =
(
	if (px != undefined) then
	(
		px.pxremoveall();
	)
	-- print ("px_onSystemReset 1\n");
  if (px_control != undefined) then
  (
    -- print ("px_control is defined\n");
    if (px_control.onSystemReset != undefined) then
    (
		-- print ("px_onSystemReset 2\n");
        px_control.onSystemReset();
    )
  )
)

function px_findPotentialRigidBodies objs &dest =
(
	local n = undefined;
	for n in objs do
	(
		if (findItem dest n) == 0 then
		(
			if(isrootrigidbody n) then
			(
				append dest n;
			)
		)
	)
)

function px_findRigidBodies objs &dest includeUndefined =
(
	local n = undefined;
	for n in objs do
	(
		if (findItem dest n) == 0 then
		(
			if(isrootrigidbody n) then
			(
				local type = getuserprop n "PhysicsType";
				if( (includeUndefined and (type == undefined)) or type == PX_PHYSTYPE_DYNAMIC or type == PX_PHYSTYPE_KINEMATIC or type == PX_PHYSTYPE_STATIC) then
				(
					append dest n;
				)
			)
		)
	)
)

function px_getShapesForRigidBody body = 
(
	local rbNode = body;
	local pgNode = getproxy body;
	if (pgNode != undefined) then
	(
		rbNode = pgNode;
	)
	
	-- go through the hierarchy that starts with rbNode and list all shapes
	local resultList = #();
	local tempStack = #();
	append tempStack rbNode;
	local currentNode = undefined;
	while (tempStack.count > 0) do
	(
		currentNode = tempStack[1];
		deleteItem tempStack 1;
		if (px_isshape currentNode) then 
		(
			append resultList currentNode;
		)
		if (currentNode.children != undefined) then
		(
			local i = 0;
			for i = 1 to currentNode.children.count do
			(
				append tempStack currentNode.children[i];
			)
		)
	)
	return resultList;
)

function px_findShapes objs &dest =
(
	local n = undefined;
	for n in objs do
	(
		local tempList = px_getShapesForRigidBody n;
		local i = undefined;
		for i in tempList do
		(
			if (findItem dest i) == 0 then
			(
				append dest i;
			)
		)
	)
)

function px_convertToConvex n maxVertices inflation =
(
	if (superclassof n) != GeometryClass then return undefined;

	local mesh = snapshot n;
	mesh.parent = undefined
	try(convertToMesh mesh) catch(undefined);
	if (mesh != undefined) then
	(
		local convex = px.createConvexHull mesh.mesh maxVertices inflation;
		delete mesh;
		if (convex != undefined) then
		(
			local node = Box();
			if (node != undefined) then
			(
				node.name = uniqueName (n.name + "_convex");
				convertToMesh node;
				node.mesh = convex;
				node.transform = n.transform;
				
				setUserProp node "IsConcave" 0
				setUserProp node "GeometryType" 5

				delete convex
				return node;
			)
		)
	)
	return undefined;
)

function px_getPxActorProxy actor =
(
	if (actor == undefined) then
	(
		return undefined;
	)

	local proxyname = try(getuserprop actor "Proxy_Geometry" as string) catch("");
	if (proxyname=="" or proxyname==undefined) then return undefined;
	return findobject(proxyname);
)

function px_getPxActorAndProxy n = 
(
	if (n == undefined) then
	(
		return undefined;
	)

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
		if (type < PX_PHYSTYPE_RB_OVER) then 
		(
			local proxy = px_getPxActorProxy n;
			if (proxy != undefined) then
			(
				return proxy;
			)
			return n;
		)
	)
	return undefined;
)

function px_getPxActor n = 
(
	if (n == undefined) then
	(
		return undefined;
	)

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
		if (type < PX_PHYSTYPE_RB_OVER) then 
		(
			return n;
		)
	)
	return undefined;
)

function px_postOpenFile =
(
	needRefreshObjects = 1;
	-- print (format "px_postOpenFile: needRefreshObjects = %\n" needRefreshObjects);
)


function px_preNewFile =
(
	needRefreshObjects = 1;
	waitOpenNewScene = 1;
	-- print (format "px_preNewFile: waitOpenNewScene = %\n" waitOpenNewScene);
	px_onSystemReset();
)


callbacks.addScript #filePreOpen "px_preNewFile()" id:#cbOpenSysReset
callbacks.addScript #systemPreNew "px_preNewFile()" id:#cbNewSysReset
callbacks.addScript #preImport "px_preNewFile()" id:#cbImportSysReset
callbacks.addScript #preSystemShutdown "px_onSystemReset()" id:#cbShutDownSysReset
-- callbacks.addScript #filePostOpen "px_onSystemReset()" id:#cbPostOpenSysReset
callbacks.addScript #filePostOpen "px_postOpenFile()" id:#cbPostOpenSysReset
