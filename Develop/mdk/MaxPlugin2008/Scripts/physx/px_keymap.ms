--
-- Functions for manipulating a 2-dimensional array of key -> value pairs
--

function pxMap_hasKey &map keyName =
(
	for pair in map do
	(
		if (pair.count == 2) then
		(
			if (pair[1] == keyName) then
			(
				return pair;
			)
		)
	)
	return undefined;
)

function pxMap_addPair &map keyName value =
(
--messagebox "pxmap_addpair"
	local tmp = pxMap_hasKey &map keyName;
	if (tmp == undefined) then
	(
		tmp = #(keyName, value);
		append map tmp;
	) else (
		tmp[1] = keyName;
		tmp[2] = value;
	)
)

function pxMap_getValue &map keyName defaultValue =
(
	local tmp = pxMap_hasKey &map keyName;
	if (tmp != undefined) then
	(
		return tmp[2];
	)
	return defaultValue;
)
