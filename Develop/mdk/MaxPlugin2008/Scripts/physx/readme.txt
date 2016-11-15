* px_rigidbodyproperties.ms 수정

Line 180 :

[변경전]
if (mass_ui.value == 0.0 and density_ui.value == 0.0) then
(
	mass_ui.value = 1.0;
)

[변경후]
if (mass_ui.value == 0.0 and density_ui.value == 0.0) then
(
	mass_ui.value = 0.0; --1.0;
	density_ui.value = 1.0;
)
