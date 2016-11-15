--
-- PhysX Debug Visuals.
--
--
filein "px_base.ms"

utility px_debug "PhysX Debug Visuals" 
(
	checkbox    activatedebug     "Activate Debug Visuals"   checked:false
	
	on activatedebug changed val do (
		local ret = px.pxvisualizephysics(activatedebug.state);
		if(ret==0) then
			activatedebug.state = FALSE;
	)
	
	on px_debug open do
	(
		local vp = px.getdebugphysics()
		activatedebug.state = vp
	)
	
)
