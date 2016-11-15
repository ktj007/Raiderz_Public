-- 
--  PhysX Setting Utilities
--
--  Support functions for controlling PhysX scene and SDK settings
--
--

global px_setting_scenescale = 1; -- default for meters, 100.0f for centimeters

function px_setup_visualization extensive =
(
	if (px == undefined) then return 0;

	-- default visualizations
	px.setSDKParameter "NX_VISUALIZATION_SCALE" 1.0;
	px.setSDKParameter "NX_VISUALIZE_JOINT_LOCAL_AXES" 1.0;
	px.setSDKParameter "NX_VISUALIZE_JOINT_WORLD_AXES" 1.0;
	px.setSDKParameter "NX_VISUALIZE_JOINT_LIMITS" 1.0;
	px.setSDKParameter "NX_VISUALIZE_WORLD_AXES" 1.0;
	px.setSDKParameter "NX_VISUALIZE_COLLISION_SHAPES" 1.0;

	if (extensive) then
	(
		-- add extra visualization here
	)

	return 1;
)

function px_resetSDKParameters =
(
	if (px == undefined) then return 0;
	px.resetSDKParameter("all");
)

function px_setup_centimeters = 
(
	if (px == undefined) then return 0;
	
	px_setting_scenescale = 100.0;
	px_setup_defaults();
)

function px_setup_meters = 
(
	if (px == undefined) then return 0;
	
	px_setting_scenescale = 1.0;
	return px_setup_defaults();
)

function px_setup_defaults =
(
	if (px == undefined) then return 0;
	px.setGravity([0, 0, -9.81*px_setting_scenescale]);
	
	px.setSDKParameter "NX_SKIN_WIDTH" 0.025*px_setting_scenescale;
	px.setSDKParameter "NX_BOUNCE_THRESHOLD" (-2.0*px_setting_scenescale);
	px.setSDKParameter "NX_DEFAULT_SLEEP_LIN_VEL_SQUARED" 0.15*0.15*(px_setting_scenescale*px_setting_scenescale);
	px.setSDKParameter "NX_DEFAULT_SLEEP_ANG_VEL_SQUARED" 0.14*0.14;
	px.setSDKParameter "NX_DYN_FRICT_SCALING" 1.0;
	px.setSDKParameter "NX_STA_FRICT_SCALING" 1.0;
	px.setSDKParameter "NX_MAX_ANGULAR_VELOCITY" 7.0;
	return 1;
)

global px_setting_presetMaterials = #();

struct PxPresetMaterialStruct (name, staticfriction = 0.0, dynamicfriction = 0.0, restitution = 0.0);

function px_presetmaterial_removeall = 
(
	px_setting_presetMaterials = #();
)

function px_presetmaterial_get name = 
(
	local i = undefined;
	for i in px_setting_presetMaterials do
	(
		try (
			if (i.name == name) then
			(
				return i;
			)
		) catch(0)
	)
	return undefined;
)

function px_presetmaterial_add name staticfriction dynamicfriction restitution =
(
	local temp = px_presetmaterial_get name;
	if (temp == undefined) then
	(
		temp = PxPresetMaterialStruct name staticfriction dynamicfriction restitution;
		append px_setting_presetMaterials temp;
	)
	temp.staticfriction = staticfriction;
	temp.dynamicfriction = dynamicfriction;
	temp.restitution = restitution;
	return temp;
)

