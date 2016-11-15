--
--  PhysX
--  Physics Utilities and Exporter
--  use/modify as you see fit
--
-- Installation instructions:  put this file in <3dsMaxRoot>/scripts/physx/
--
--
--
-- Some things are better implemented in C++ than in maxscript.  Support functions
-- can be found in the pxplugin.dlm dynamic linked library.
-- If that library has been loaded properly there will be a bunch of functions exposed to
-- maxscript.  Such as:  pxLegacy.hull( mymesh );
-- If not then a simple maxscript substitute function will do the job, although probably not as good
--
-- global outfile   -- defined in px_xmlutils.ms
--

-- System Status Definiations
global needRefreshObjects  = 0;
global waitOpenNewScene    = 0;

-- PhysX Type Definitions
global PX_PHYSTYPE_DYNAMIC    = 1;
global PX_PHYSTYPE_KINEMATIC  = 2;
global PX_PHYSTYPE_STATIC     = 3;
global PX_PHYSTYPE_RB_OVER    = 4;
global PX_PHYSTYPE_UNDEFINED  = 5;
global PX_PHYSTYPE_CLOTH      = 6;
global PX_PHYSTYPE_SOFTBODY   = 7;
global PX_PHYSTYPE_FORCEFIELD = 8;
global PX_PHYSTYPE_FLUID      = 9;
global PX_PHYSTYPE_METAL_CLOTH      = 10;
global PX_PHYSTYPE_RB_PROXY   = 11;

-- PhysX State Definitions
global PX_PHYSTATUS_ENABLED  = 1;
global PX_PHYSTATUS_DISABLED  = 2;

-- Default undefined behavior.
global PX_PHYSTYPE_DEFAULT   = PX_PHYSTYPE_DYNAMIC;

-- Define unit exchange rates
global RATE_RAD_TO_DEGREE = 180.0/pi;
global RATE_DEGREE_TO_RAD = pi/180.0;

filein "px_tools.ms"
filein "px_base.ms"
filein "px_joint.ms"        -- defines an extra parent-child joint editing capabilites
filein "px_fluid.ms"        -- the fluid object (helper)
filein "px_emitter.ms"      -- the fluid emitter object (helper)
filein "px_d6joint.ms"      -- new D6 joint spec.
--filein "px_ragdoll.ms"      -- new D6 joint spec.
filein "px_settings.ms"		-- utilities for controlling PhysX settings
filein "px_control.ms"      -- the physx control panel

function px_removesubrollouts =
(
  removerollout px_control;
)

max utility mode
addrollout px_control
max create mode

try ( px.pxSetOutputStream listener ) catch ( print "PhysX Warning:  unable to call function px.setstream from PXPLUGIN.DLM, the plugin might not be correctly installed."; )

filein "px_user.ms"			-- user-specific initialization

"PhysX Physics Utilities and Exporter Loaded"
