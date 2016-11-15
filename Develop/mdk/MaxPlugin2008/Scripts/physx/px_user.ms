-- 
--  PhysX User Settings
--
--  This file is included last in the startup of the PhysX plugin and
--  should be used for making initializations of user objects
--

-- Preset materials, change/remove/add into materials that fits your current project
-- The materials available here by default are mockups
px_presetmaterial_removeall();
px_presetmaterial_add "Standard material" 0.3 0.25 0.1
px_presetmaterial_add "Low friction (e.g. ice)" 0.05 0.01 0.05
px_presetmaterial_add "High friction (e.g. rubber)" 0.9 0.6 0.3


