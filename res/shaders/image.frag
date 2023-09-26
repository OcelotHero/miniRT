#version 400 core

uniform vec2 iResolution;
out vec4 fragColor;
vec2 fragCoord = gl_FragCoord.xy;

uniform sampler2D iChannel0;

// a pixel value multiplier of light before tone mapping and sRGB
const float EXPOSURE = 0.5f;

//-----------------------------------------------------
// Utility functions
//-----------------------------------------------------
vec3 LessThan(vec3 f, float value) {
	return vec3(
		(f.x < value) ? 1.0f : 0.0f,
		(f.y < value) ? 1.0f : 0.0f,
		(f.z < value) ? 1.0f : 0.0f);
}

// Tone conversion functions
//-----------------------------------------------------
vec3 LinearToSRGB(vec3 rgb) {
	rgb = clamp(rgb, 0.0f, 1.0f);

	return mix(pow(rgb, vec3(1.0f / 2.4f)) * 1.055f - 0.055f,
				   rgb * 12.92f,
				   LessThan(rgb, 0.0031308f));
}

// ACES tone mapping curve fit to go from HDR to LDR
// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESFilm(vec3 x) {
	float	a = 2.51f;
	float	b = 0.03f;
	float	c = 2.43f;
	float	d = 0.59f;
	float	e = 0.14f;
	return clamp((x*(a*x + b)) / (x*(c*x + d) + e), 0.0f, 1.0f);
}

void main() {
	vec3 color = texture(iChannel0, fragCoord / iResolution).rgb;

	// apply exposure (how long the shutter is open)
	color *= EXPOSURE;
	// convert unbounded HDR color range to SDR color range
	color = ACESFilm(color);
	// convert from linear to sRGB for display
	color = LinearToSRGB(color);

	fragColor = vec4(color, 1.0f);
}
