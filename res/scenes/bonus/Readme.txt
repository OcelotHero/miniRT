The color attribute can either accept normal r,g,b values or path to json file
describing the material properties as described in Readme found in materials.
If only r,g,b values is available, it is equivalent to having a json file
containing only the `albedo` key with the corresponding r,g,b value, i.e:
	{ "albedo"	: "r,g,b"}

If file is not found CHOOSE to either return error or set the value to default
following the rules listed in Readme found in materials. (the subject pdf
requires error to be returned in case the r,g,b values are not found)

//------------------------------------------------------------------------------
Valid attribute formating and their corresponding values for the new geometry:
	bx: BOX
		x,y,z	- position vector
			-> each value can be within the range of (-inf, inf)
		x,y,z	- rotation axis vector
			-> each value can only be within the range of [-1, 1], i.e. normalized coordinate
		theta	- angle
			-> value can be within the range of (-inf, inf) given in degrees
		(CHOOSE EITHER)
		~	x_dim,y_dim,z_dim	- dim vector
				-> each value can be within the range of (-inf, inf)
		~	x_dim
				-> value can be within the range of (-inf, inf)
			y_dim
				-> value can be within the range of (-inf, inf)
			z_dim
				-> value can be within the range of (-inf, inf)
		color or path_to_json as aforementioned

	cn: CONE
		x,y,z	- position vector
			-> each value can be within the range of (-inf, inf)
		x,y,z	- axis vector
			-> each value can only be within the range of [-1, 1], i.e. normalized coordinate
		r1		- front cap radius
			-> value can be within the range of (-inf, inf)
		r2		- back cap radius
			-> value can be within the range of (-inf, inf)
		h		- height
			-> value can be within the range of (-inf, inf) given in degrees
		color or path_to_json as aforementioned

	tr: TRIAGL
		x,y,z	- position vector
			-> each value can be within the range of (-inf, inf)
		x,y,z	- first vertex position vector
			-> each value can be within the range of (-inf, inf)
		(CHOOSE EITHER)
		~	x,y,z	- second vertex posiiton vector
				-> each value can be within the range of (-inf, inf)
		~	x
				-> value can be within the range of (-inf, inf)
			y
				-> value can be within the range of (-inf, inf)
			z
				-> value can be within the range of (-inf, inf)
		color or path_to_json as aforementioned

	qd: QUAD
		x,y,z	- position vector
			-> each value can be within the range of (-inf, inf)
		x,y,z	- first major axis position vector
			-> each value can be within the range of (-inf, inf)
		(CHOOSE EITHER)
		~	x,y,z	- second major axis posiiton vector
				-> each value can be within the range of (-inf, inf)
		~	x
				-> value can be within the range of (-inf, inf)
			y
				-> value can be within the range of (-inf, inf)
			z
				-> value can be within the range of (-inf, inf)
		color or path_to_json as aforementioned

	ds:	DISK
		x,y,z	- position vector
			-> each value can be within the range of (-inf, inf)
		x,y,z	- normal vector
			-> each value can only be within the range of [-1, 1], i.e. normalized coordinate
		r		- radius
			-> value can only be within the range of [0, inf)

!!!SPECIAL CASE!!!
	A: AMBI
		intensity	- light intensity
			-> value can only be within the range of [0, 1]
		r,g,b		- light color
			-> each value can only be within the range of [0, 255]

	lp: PTLIGHT
		x,y,z		- position vector
			-> each value can be within the range of (-inf, inf)
		intensity	- light intensity
			-> value can only be within the range of [0, 1]
		r,g,b		- light color
			-> each value can only be within the range of [0, 255]

	ls: SPLIGHT
		x,y,z		- position vector
			-> each value can be within the range of (-inf, inf)
		x,y,z		- direction vector
			-> each value can only be within the range of [-1, 1]
		inner		- spotlight inner cone angle cutoff
			-> value can only be within the range of [0, 180]
		outer		- spotlight outer cone angle cutoff
			-> value can only be within the range of [inner, 180]
		intensity	- light intensity
			-> value can only be within the range of [0, 1]
		r,g,b		- light color
			-> each value can only be within the range of [0, 255]

NB:	SPLIGHT (spotlight) is a generalized point light sources that accepts 2 additional
	parameters, the inner and outer cone angle given in degrees. Incident ray falling
	inside the inner cone gets full intensity, whereas those outside the outer cone gets
	none, with interpolated value when falling in between the two regions. This means that
	point light is equivalent to spotlight with the inner and outer cone set to 180 degrees.

!!!Additional keyword!!!
	CB:	QMAP
		intensity	- background intensity
			-> value can only be within the range of [0, 1]

	R:	SIZE
		width		- window width
			-> value can only be within the range of [0, inf)
		height		- window height
			-> value can only be within the range of [0, inf)

//------------------------------------------------------------------------------
Memory layout packing for attribute values stored in t_object param array
(the 4 values after the first 2 vectors):
	bx: BOX
		theta {x_dim,y_dim,z_dim | x_dim y_dim z_dim} (CHOOE EITHER FORMAT)
			-> |x_dim		|y_dim		|z_dim		|theta		|

	cn:	CONE
		r1	r2	h
			-> |r1			|r2			|h1			|.			|

	tr: TRIAGL
		{x,y,z | x y z} (CHOOE EITHER FORMAT)
			-> |x			|y			|z			|.			|

	qd: QUAD
		{x,y,z | x y z} (CHOOE EITHER FORMAT)
			-> |x			|y			|z			|.			|

	ds: DISK
		radius
			-> |radius		|.			|.			|.			|

	. - indicates padding

!!!SPECIAL CASE!!!
	A: AMBI
		intensity	r,g,b
			-> |r			|g			|b			|intensity	|

	lp: PTLIGHT
		intensity	r,g,b
			-> |r			|g			|b			|intensity	|

	ls: SPLIGHT
		intensity	r,g,b
			-> |r			|g			|b			|intensity	|