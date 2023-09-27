The JSON file can either be a collection of objects as in `material.json` or
refering to one material object as in `green_metal.json`, both are valid.

//------------------------------------------------------------------------------
Valid attribute formating and their corresponding values:
	albedo, emissive, specularColor, refractionColor
		r,g,b
			-> default: 0,0,0
			-> each value can only be within the range of [0, 255]

	texture
		path_to_texture(:u_offset,v_offset)
			-> default: 0,0
			-> each value can only be within the range of [-1, 1]
		checkered(:u_scale,v_scale)
			-> default: 0,0
			-> each value can only be within the range of [0, inf)

	normalMap
		path_to_texture:(u_offset,v_offset,) scale
			-> default: 0,0
			-> each value can only be within the range of [-1, 1]
			-> scale is always required with a range of [0, inf)

	specularChance, specularRoughness, refractionChance, refractionRoughness
		-> default: 0
		-> valid values within the range of [0, 1]

	intensity
		-> default: 1
		-> valid value wihtin the range of [0, inf)

	IOR
		-> default: 1
		-> valid value within the range of [1, inf)

	undeclared key-value pair are set to default
	invalid key can be ignored and program can proceed normally without printing error
	when texture key is found and valid, albedo and spec_color is initialized to ones

//------------------------------------------------------------------------------
Memory layout packing for the attribute values:
	albedo, emissive, specularColor, refractionColor (vec4)
		r,g,b
			-> |r			|g			|b			|
		path_to_texture(:u_offset,v_offset)
			-> |u_offset	|v_offset	|tex_index	|
		checkered(:u_scale,v_scale)
			-> |u_scale		|v_scale	|-1			|

	normalMap
		path_to_texture:(u_offset,v_offset,) scale
			-> |u_offset	|v_offset	|scale		|tex_index	|
