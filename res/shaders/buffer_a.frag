#version 400 core

in vec2 texCoord;
uniform int iFrame;
uniform vec2 iMouse;
uniform vec2 iResolution;
uniform float iTime;
out vec4 fragColor;
vec2 fragCoord = gl_FragCoord.xy;

uniform sampler2D	iChannel0;
uniform sampler2D	organic;
uniform sampler2D	earth;

uniform samplerCube	skybox;

// The minimunm distance a ray must travel before we consider an intersection.
// This is to prevent a ray from intersecting a surface it just bounced off of.
const float c_minimumRayHitTime = 0.01f;

// after a hit, it moves the ray this far along the normal away from a surface.
// Helps prevent incorrect intersections when rays bounce off of objects.
const float c_rayPosNormalNudge = 0.01f;

// the farthest we look for ray hits
const float c_superFar = 10000.0f;

// camera FOV
const float c_FOVDegrees = 90.0f;

// number of ray bounces allowed
const int c_numBounces = 8;

// a multiplier for the skybox brightness
const float c_skyboxBrightnessMultiplier = 1.0f;

// a pixel value multiplier of light before tone mapping and sRGB
const float c_exposure = 0.5f;

// how many renders per frame - to get around the vsync limitation.
const int c_numRendersPerFrame = 8;

const float c_pi = 3.14159265359f;
const float c_twopi = 2.0f * c_pi;

const bool	b2b_specular_plane	= true;

vec3 LessThan(vec3 f, float value) {
	return vec3((f.x < value) ? 1.0f : 0.0f,
				(f.y < value) ? 1.0f : 0.0f,
				(f.z < value) ? 1.0f : 0.0f);
}

vec3 LinearToSRGB(vec3 rgb) {
	rgb = clamp(rgb, 0.0f, 1.0f);

	return mix(pow(rgb, vec3(1.0f / 2.4f)) * 1.055f - 0.055f,
			   rgb * 12.92f,
			   LessThan(rgb, 0.0031308f)
	);
}

vec3 SRGBToLinear(vec3 rgb) {
	rgb = clamp(rgb, 0.0f, 1.0f);

	return mix(pow(((rgb + 0.055f) / 1.055f), vec3(2.4f)),
			   rgb / 12.92f,
			   LessThan(rgb, 0.04045f));
}

// ACES tone mapping curve fit to go from HDR to LDR
//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESFilm(vec3 x) {
	float	a = 2.51f;
	float	b = 0.03f;
	float	c = 2.43f;
	float	d = 0.59f;
	float	e = 0.14f;
	return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}

uint wang_hash(inout uint seed) {
	seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
	seed *= uint(9);
	seed = seed ^ (seed >> 4);
	seed *= uint(0x27d4eb2d);
	seed = seed ^ (seed >> 15);
	return seed;
}

float RandomFloat01(inout uint state) {
	return float(wang_hash(state)) / 4294967296.0;
}

vec3 RandomUnitVector(inout uint state) {
	float	z = RandomFloat01(state) * 2.0f - 1.0f;
	float	a = RandomFloat01(state) * c_twopi;
	float	r = sqrt(1.0f - z * z);
	float	x = r * cos(a);
	float	y = r * sin(a);
	return vec3(x, y, z);
}

mat4 rotationAxisAngle( vec3 v, float angle )
{
	float	s = sin(angle);
	float	c = cos(angle);
	float	ic = 1.0 - c;

	return mat4(v.x*v.x*ic + c,	 v.y*v.x*ic - s*v.z, v.z*v.x*ic + s*v.y, 0.0f,
				v.x*v.y*ic + s*v.z, v.y*v.y*ic + c,	 v.z*v.y*ic - s*v.x, 0.0f,
				v.x*v.z*ic - s*v.y, v.y*v.z*ic + s*v.x, v.z*v.z*ic + c,	 0.0f,
				0.0f,				 0.0f,				 0.0f,			 1.0f);
}

mat4 translate( float x, float y, float z )
{
	return mat4(1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				x,	  y,	z,	  1.0f);
}

struct	SMaterial {
	vec3	albedo;					// the color used for diffuse lighting
	vec3	emissive;				// how much the surface glows
	float	specularChance;			// percentage chance of doing a specular reflection
	float	specularRoughness;		// how rough the specular reflections are
	vec3 	specularColor;			// the color tint of specular reflections
	float	IOR;					// index of refraction. used by fresnel and refraction.
	float	refractionChance;		// percent chance of doing a refractive transmission
	float	refractionRoughness;	// how rough the refractive transmissions are
	vec3	refractionColor;		// absorption for beer's law
};

struct	SHitRecord {
	int		id;		// mat id
	bool	frontFace;
	float	dist;	// distance from ray origin to intersection point
	vec3	normal;	// normal vector at intersection point
	vec2	uv;		// local coordinate
};

struct	SRay {
	vec3	ori;
	vec3	dir;
};

SMaterial	materialZero() {
	SMaterial mat;
	mat.albedo = vec3(0.0f, 0.0f, 0.0f);
	mat.emissive = vec3(0.0f, 0.0f, 0.0f);
	mat.specularChance = 0.0f;
	mat.specularRoughness = 0.0f;
	mat.specularColor = vec3(0.0f, 0.0f, 0.0f);
	mat.IOR = 1.0f;
	mat.refractionChance = 0.0f;
	mat.refractionRoughness = 0.0f;
	mat.refractionColor = vec3(0.0f, 0.0f, 0.0f);
	return mat;
}

vec3 checkerboard(vec2 uv) {
	vec3 col = vec3(0.2);
	col += 0.4*smoothstep(-0.01,0.01,cos(uv.x*0.5)*cos(uv.y*0.5));
	col *= smoothstep(-1.0,-0.98,cos(uv.x))*smoothstep(-1.0,-0.98,cos(uv.y));
	return col;
}

bool	sphereIntersection(SRay ray, inout SHitRecord rec, vec4 sphere) {
	// translate to local coordinate
	vec3	m = ray.ori - sphere.xyz;

	// calculate the quadratic coeeficients
	float	b = dot(m, ray.dir);
	float	c = dot(m, m) - sphere.w * sphere.w;

	// exit if r originates outside sp (c > 0) and r points away from sp (b > 0)
	if (c > 0.0f && b > 0.0f)	return false;

	// r does not intersect sp
	float	dis = b * b - c;
	if (dis < 0.0f)	return false;

	// r intersects sp, compute smallest t value of intersection
	rec.frontFace = true;
	float	sqrtd = sqrt(dis);
	float	t = -b - sqrtd;
	if (t <= c_minimumRayHitTime) {
		rec.frontFace = false;
		t = -b + sqrtd;
	}
	// t value larger than record
	if (t <= c_minimumRayHitTime || rec.dist <= t)	return false;

	// save intersection information in record
	vec3	p = (m + ray.dir * t) / sphere.w;
	rec.uv = vec2((atan(p.z, p.x) + c_pi) / (2 * c_pi), acos(-p.y) / c_pi);
	rec.dist = t;
	rec.normal = p * (rec.frontFace ? 1.0f : -1.0f);
	return true;
}

bool	boxIntersection(SRay ray, inout SHitRecord rec, mat4 txx, mat4 txi, vec3 dim) {
	// convert from world to box space
	vec3	rd = (txx * vec4(ray.dir, 0.0f)).xyz;
	vec3	ro = (txx * vec4(ray.ori, 1.0f)).xyz;

	// ray-box intersection in box space
	vec3	m = 1.0f / rd;
	vec3	s = vec3((rd.x < 0.0f) ? 1.0f : -1.0f,
					 (rd.y < 0.0f) ? 1.0f : -1.0f,
					 (rd.z < 0.0f) ? 1.0f : -1.0f);
	vec3	t1 = m * (-ro + s * dim);
	vec3	t2 = m * (-ro - s * dim);

	// compute t near and t far intersections
	float	tN = max( max( t1.x, t1.y ), t1.z );
	float	tF = min( min( t2.x, t2.y ), t2.z );

	// no intersection
	if (tN > tF || tF < 0.0f)	return false;

	// r intersects bx
	rec.frontFace = true;
	if (tN <= c_minimumRayHitTime) {
		rec.frontFace = false;
		tN = tF;
	}

	// t value larger than record
	if (tN <= c_minimumRayHitTime || rec.dist <= tN)	return false;

	// r intersects box
	rec.dist = tN;
	// compute normal (in world space)
	if (t1.x > t1.y && t1.x > t1.z) { rec.normal = txi[0].xyz * s.x; rec.uv = (ro + tN * rd).yz; }
	else if (t1.y > t1.z)			{ rec.normal = txi[1].xyz * s.y; rec.uv = (ro + tN * rd).zx; }
	else							{ rec.normal = txi[2].xyz * s.z; rec.uv = (ro + tN * rd).xy; }
	return true;
}

bool	cylinderIntersection(SRay ray, inout SHitRecord rec, vec3 pos, vec3 normal, float radius, float height) {
	// translate to local coordinate
	vec3	m = ray.ori - pos;
	vec3	n = normalize(normal);
	float	nd = dot(n, ray.dir);
	float	nm = dot(n, m);

	// calculate the quadratic coefficients
	float	a = 1.0f - nd * nd;
	float	b = dot(m, ray.dir) - nm * nd;
	float	c = dot(m, m) - nm * nm - radius * radius;

	// exit if r originates outside cy (c > 0) and r points away from cy (b > 0)
	if (c > 0.0f && b > 0.0f)	return false;

	// r does not intersect sp
	float	dis = b * b - a * c;
	if (dis < 0.0f)	return false;

	float	h, t, tm = c_superFar, tc = c_superFar;
	bool	mFrontFace, cFrontFace;

	float	sqrtd = sqrt(dis);

	mFrontFace = true;
	t = (-b - sqrtd) / a;
	if (t <= c_minimumRayHitTime) {
		mFrontFace = false;
		t = (-b + sqrtd) / a;
	}
	h = nm + t * nd;
	if (abs(h) <= height)	tm = t;

	// get possible intersection on caps
	float t1 = (-height - nm) / nd;
	float t2 = ( height - nm) / nd;
	if (min(t1, t2) > c_minimumRayHitTime) {
		t = min(t1, t2);
		if (abs(b + t * a) < sqrtd) {
			cFrontFace = true;
			tc = t;
		}
	} else if (max(t1, t2) > c_minimumRayHitTime) {
		t = max(t1, t2);
		if (abs(b + t * a) < sqrtd) {
			cFrontFace = false;
			tc = t;
		}
	}

	t = min(tm, tc);
	if (t <= c_minimumRayHitTime || rec.dist <= t)	return false;

	// local coordinate calculation
	vec3	p = m + ray.dir * t;
	vec3	u = dot(n, vec3(1.0f, 1.0f, 0.0f)) == 0.0f ?
						vec3(0.0f, 1.0f, 0.0f) :
						normalize(cross(n, vec3(0.0f, 0.0f, 1.0f)));
	vec3	v = normalize(cross(u, n));
	vec3	q = p * mat3(u, v, n);
	rec.uv = vec2(2.0f * atan(q.y, q.x), q.z);

	rec.dist = t;
	if (t == tm) {
		rec.frontFace = mFrontFace;
		rec.normal = ((p - h * n) / radius) * (mFrontFace ? 1.0f : -1.0f);
	} else {
		rec.frontFace = cFrontFace;
		rec.normal = n * -sign(nd);
	}
	return true;
}

bool	planeIntersection(SRay ray, inout SHitRecord rec, vec3 pos, vec3 normal) {
	// get the vector from the center of this plane to where the ray begins.
	vec3	m = ray.ori - pos;
	float	a = dot(ray.dir, normal);
	float	b = dot(m, normal);

	float	t = -b / a;
	// t value larger than record
	if (t <= c_minimumRayHitTime || rec.dist <= t)	return false;

	// r intersects pl
	rec.frontFace = true;

	// save intersection information in record
	vec3	p = ray.ori + t * ray.dir;
	rec.uv = p.xy * normal.z + p.zx * normal.y + p.yz * normal.x;
	rec.dist = t;
	// rec.normal = normal;
	rec.normal = normal * (a < 0.0f ? 1.0f : -1.0f);
	return true;
}

bool	triIntersection(SRay ray, SHitRecord rec, vec3 v0, vec3 v1, vec3 v2 )
{
	// get the vector from the center of this quad to where the ray begins.
	vec3	a = v0 - v1;
	vec3	b = v2 - v0;
	vec3	p = v0 - ray.ori;

	vec3	n = cross(a, b);
	vec3	q = cross(p, ray.dir);

	float	i = 1.0f / dot(ray.dir, n);

	float	u = dot(q, b) * i;
	float	v = dot(q, a) * i;
	float	t = dot(n, p) * i;

	// no intersection
	if(u < 0.0f || v < 0.0f || (u + v) > 1.0f )	return false;
	// t value larger than record
	if (t <= c_minimumRayHitTime || rec.dist <= t)	return false;

	// r intersects tr
	rec.frontFace = true;

	// save intersection information in record
	rec.uv = vec2(u, v);
	rec.dist = t;
	rec.normal = n * (i < 0.0f ? 1.0f : -1.0f);
	return true;
}

bool	quadIntersection(SRay ray, inout SHitRecord rec, vec3 pos, vec3 v0, vec3 v1) {
	// get the vector from the center of this quad to where the ray begins.
	vec3	a = pos - v0;
	vec3	b = v1 - pos;
	vec3	p = pos - ray.ori;

	vec3 n = cross(a, b);
	vec3 q = cross(ray.dir, p);

	float i = 1.0f / dot(ray.dir, n);

	float u = dot(q, a) * i;
	float v = dot(q, b) * i;
	float t = dot(n, p) * i;

	// no intersection
	if (u < -1.0f || u > 1.0f || v < -1.0f || v > 1.0f)	return false;
	// t value larger than record
	if (t <= c_minimumRayHitTime || rec.dist <= t)	return false;

	// r intersects qd
	rec.frontFace = true;

	// save intersection information in record
	rec.uv = vec2(u, v) / 2.0f + 0.5f;
	rec.dist = t;
	rec.normal = normalize(i < 0.0f ? n : -n);
	return true;
}

bool	diskIntersection(SRay ray, inout SHitRecord rec, vec3 pos, vec3 normal, float radius) {
	// get the vector from the center of this quad to where the ray begins.
	vec3	m = ray.ori - pos;
	vec3	n = normalize(normal);

	float	a = dot(n, ray.dir);
	float	t = -dot(n, m) / a;
	vec3	q = m + ray.dir * t;

	// no intersection
	if (dot(q, q) > radius * radius) return false;
	// t value larger than record
	if (t <= c_minimumRayHitTime || rec.dist <= t)	return false;

	// r intersects disk
	rec.frontFace = true;

	// save intersection information in record
	rec.dist = t;
	rec.normal = normalize(a < 0.0f ? n : -n);
	return true;
}

void sceneIntersection(SRay ray, inout SHitRecord rec, out SMaterial mat, inout uint rngState) {
	// to move the scene around, since we can't move the camera yet
	vec3 sceneTranslation = vec3(0.0f, 0.0f, 10.0f);
	vec4 sceneTranslation4 = vec4(sceneTranslation, 0.0f);

	// {
	// 	vec3	pos	= vec3(   0.00f, -20.00f,  50.00f) + sceneTranslation;
	// 	if (planeIntersection(ray, rec, pos, vec3(0.0f, 1.0f, 0.0f))) {
	// 		mat = materialZero();
	// 		// mat.albedo = vec3(0.1f, 0.1f, 0.7f);
	// 		mat.albedo = checkerboard(2.0f*rec.uv);
	// 	}
	// }

	// back wall
	{
		vec3	pos	= vec3(   0.00f,   0.00f,  25.00f) + sceneTranslation;
		vec3	v0	= vec3(  12.60f,   0.00f,   0.00f) + pos;
		vec3	v1	= vec3(   0.00f,  12.60f,   0.00f) + pos;
		if (quadIntersection(ray, rec, pos, v0, v1)) {
			mat = materialZero();
			mat.albedo = vec3(0.7f, 0.7f, 0.7f);
		}
	}

	// floor
	{
		vec3	pos	= vec3(   0.00f, -12.45f,  20.00f) + sceneTranslation;
		vec3	v1	= vec3(   0.00f,   0.00f,   5.00f) + pos;
		vec3	v0	= vec3(  12.60f,   0.00f,   0.00f) + pos;
		if (quadIntersection(ray, rec, pos, v0, v1)) {
			mat = materialZero();
			mat.albedo = vec3(0.7f, 0.7f, 0.7f);
			// mat.specularChance = 1.0f;
			// mat.specularColor = vec3(0.7f, 0.7f, 0.7f);
		}
	}

	// ceiling
	{
		vec3	pos	= vec3(   0.00f,  12.50f,  20.00f) + sceneTranslation;
		vec3	v0	= vec3(   0.00f,   0.00f,   5.00f) + pos;
		vec3	v1	= vec3(  12.60f,   0.00f,   0.00f) + pos;
		if (quadIntersection(ray, rec, pos, v0, v1)) {
			mat = materialZero();
			mat.albedo = vec3(0.7f, 0.7f, 0.7f);
		}
	}

	// left wall
	{
		vec3	pos	= vec3( -12.50f,   0.00f,  20.00f) + sceneTranslation;
		vec3	v0	= vec3(   0.00f,  12.60f,   0.00f) + pos;
		vec3	v1	= vec3(   0.00f,   0.00f,   5.00f) + pos;
		if (quadIntersection(ray, rec, pos, v0, v1)) {
			mat = materialZero();
			mat.albedo = vec3(0.7f, 0.1f, 0.1f);
			// mat.albedo = vec3(0.7f);
		}
	}

	// right wall
	{
		vec3	pos	= vec3(  12.50f,   0.00f,  20.00f) + sceneTranslation;
		vec3	v0	= vec3(   0.00f,  12.60f,   0.00f) + pos;
		vec3	v1	= vec3(   0.00f,   0.00f,   5.00f) + pos;
		if (quadIntersection(ray, rec, pos, v0, v1)) {
			mat = materialZero();
			mat.albedo = vec3(0.1f, 0.7f, 0.1f);
			// mat.albedo = vec3(0.7f);
		}
	}

	// light
	{
		vec3	pos	= vec3(   0.00f,  12.40f,  20.00f) + sceneTranslation;
		vec3	v0	= vec3(   5.00f,   0.00f,   0.00f) + pos;
		vec3	v1	= vec3(   0.00f,   0.00f,   2.50f) + pos;
		if (quadIntersection(ray, rec, pos, v0, v1)) {
			mat = materialZero();
			mat.emissive = vec3(1.0f, 0.9f, 0.7f) * 25.0f;
		}
	}

	// stripped pattern
	{
		vec3	pos	= vec3(   0.00f,  -8.50f,  24.90f) + sceneTranslation;
		vec3	v0	= vec3(   5.50f,   0.00f,   0.00f) + pos;
		vec3	v1	= vec3(   0.00f,   3.50f,   0.00f) + pos;
		if (quadIntersection(ray, rec, pos, v0, v1)) {
			mat = materialZero();
			mat.albedo = clamp(vec3(floor(mod(8.0f * rec.uv.y, 1.0f) * 2.0f)), 0.0f, 1.0f);
		}
	}

	// picture frame
	{
		vec3	pos	= vec3(   0.00f,   0.00f,  24.90f) + sceneTranslation;
		vec3	v0	= vec3(   0.00f,   3.50f,   0.00f) + pos;
		vec3	v1	= vec3(   5.50f,   0.00f,   0.00f) + pos;
		if (quadIntersection(ray, rec, pos, v0, v1)) {
			mat = materialZero();
			mat.albedo = texture(organic, rec.uv).rgb;
		}
	}

	// earth
	if (sphereIntersection(ray, rec, vec4(9.0f, -9.3f, 20.0f, 3.0f) + sceneTranslation4)) {
		mat = materialZero();
		mat.albedo = texture(earth, rec.uv + vec2(0.25f, 0.0f)).rgb;
		mat.specularChance = 0.5f;
		mat.specularColor = texture(earth, rec.uv + vec2(0.25f, 0.0f)).rgb;
	}

	if (sphereIntersection(ray, rec, vec4(9.0f, 9.0f, 20.0f, 3.0f) + sceneTranslation4)) {
		mat = materialZero();
		mat.albedo = checkerboard(64.0f * rec.uv);
	}

	// // shiny green balls of varying specularRoughnesses
	// {
	// 	if (sphereIntersection(ray, rec, vec4(-10.0f, 0.0f, 23.0f, 1.75f) + sceneTranslation4)) {
	// 		mat = materialZero();
	// 		mat.albedo = vec3(1.0f, 1.0f, 1.0f);
	// 		mat.specularChance = 1.0f;
	// 		mat.specularRoughness = 0.0f;
	// 		mat.specularColor = vec3(0.3f, 1.0f, 0.3f);
	// 	}

	// 	if (sphereIntersection(ray, rec, vec4(-5.0f, 0.0f, 23.0f, 1.75f) + sceneTranslation4)) {
	// 		mat = materialZero();
	// 		mat.albedo = vec3(1.0f, 1.0f, 1.0f);
	// 		mat.specularChance = 1.0f;
	// 		mat.specularRoughness = 0.25f;
	// 		mat.specularColor = vec3(0.3f, 1.0f, 0.3f);
	// 	}

	// 	if (sphereIntersection(ray, rec, vec4(0.0f, 0.0f, 23.0f, 1.75f) + sceneTranslation4)) {
	// 		mat = materialZero();
	// 		mat.albedo = vec3(1.0f, 1.0f, 1.0f);
	// 		mat.specularChance = 1.0f;
	// 		mat.specularRoughness = 0.5f;
	// 		mat.specularColor = vec3(0.3f, 1.0f, 0.3f);
	// 	}

	// 	if (sphereIntersection(ray, rec, vec4(5.0f, 0.0f, 23.0f, 1.75f) + sceneTranslation4)) {
	// 		mat = materialZero();
	// 		mat.albedo = vec3(1.0f, 1.0f, 1.0f);
	// 		mat.specularChance = 1.0f;
	// 		mat.specularRoughness = 0.75f;
	// 		mat.specularColor = vec3(0.3f, 1.0f, 0.3f);
	// 	}

	// 	if (sphereIntersection(ray, rec, vec4(10.0f, 0.0f, 23.0f, 1.75f) + sceneTranslation4)) {
	// 		mat = materialZero();
	// 		mat.albedo = vec3(1.0f, 1.0f, 1.0f);
	// 		mat.specularChance = 1.0f;
	// 		mat.specularRoughness = 1.0f;
	// 		mat.specularColor = vec3(0.3f, 1.0f, 0.3f);
	// 	}
	// }

	// if (sphereIntersection(ray, rec, vec4(-9.0f, -9.5f, 20.0f, 3.0f) + sceneTranslation4)) {
	// 	mat = materialZero();
	// 	mat.albedo = vec3(0.9f, 0.9f, 0.5f);
	// 	mat.specularChance = 0.1f;
	// 	mat.specularRoughness = 0.2f;
	// 	mat.specularColor = vec3(0.9f, 0.9f, 0.9f);
	// }
	// if (cylinderIntersection(ray, rec, vec3(-8.0f, -9.0f, 20.0f) + sceneTranslation, vec3(0.0f, -1.0f, -1.0f), 2.0f, 2.5f)) {
	// 	mat = materialZero();
	// 	mat.albedo = vec3(0.9f, 0.9f, 0.3f) * checkerboard(8.0f * rec.uv);
	// 	mat.specularChance = 0.5f;
	// 	mat.specularRoughness = 0.2f;
	// 	mat.specularColor = vec3(0.9f, 0.9f, 0.3f) * checkerboard(8.0f * rec.uv);
	// }
	// {
	// 	vec3	pos	= vec3(  -8.00f,  -7.20f,  21.80f) + sceneTranslation;
	// 	vec3	v0	= vec3(   2.00f,   0.00f,   0.00f) + pos;
	// 	vec3	v1	= vec3(   0.00f,   1.50f,  -1.50f) + pos;
	// 	if (quadIntersection(ray, rec, pos, v0, v1)) {
	// 		mat = materialZero();
	// 		mat.albedo = vec3(0.9f, 0.9f, 0.3f);
	// 		mat.specularChance = 1.0f;
	// 		mat.specularColor = vec3(0.9f, 0.9f, 0.3f);
	// 	}
	// }
	// {
	// 	vec3	pos	= vec3(  -8.00f, -10.80f,  18.20f) + sceneTranslation;
	// 	vec3	v0	= vec3(   2.00f,   0.00f,   0.00f) + pos;
	// 	vec3	v1	= vec3(   0.00f,   0.80f,  -0.80f) + pos;
	// 	if (quadIntersection(ray, rec, pos, v0, v1)) {
	// 		mat = materialZero();
	// 		mat.albedo = vec3(0.9f, 0.9f, 0.3f);
	// 		mat.specularChance = 1.0f;
	// 		mat.specularColor = vec3(0.9f, 0.9f, 0.3f);
	// 	}
	// }

	// {
	// 	mat4	txi = translate(-9.0f, -9.5f,  20.0f + sceneTranslation.z) * rotationAxisAngle(vec3(0.0f, 1.0f, 0.0f),  c_pi / 3.0f);
	// 	mat4	txx = rotationAxisAngle(vec3(0.0f, 1.0f, 0.0f), -c_pi / 3.0f) * translate( 9.0f,  9.5f, -20.0f - sceneTranslation.z);
	// 	if (boxIntersection(ray, rec, txx, txi, vec3(2.0f))) {
	// 		mat = materialZero();
	// 		// mat.albedo = checkerboard(7.5f * rec.uv);
	// 		mat.albedo = vec3(0.9f, 0.9f, 0.1f);
	// 		mat.specularChance = 0.2f;
	// 		mat.specularColor = vec3(0.9f, 0.9f, 0.1f);
	// 		mat.IOR = 1.2f;
	// 		mat.refractionChance = 1.00f;
	// 		mat.refractionColor = vec3(0.1f, 0.1f, 1.0f);
	// 	}
	// }

	// if (sphereIntersection(ray, rec, vec4(0.0f, -9.4f, 20.0f, 3.0f) + sceneTranslation4)) {
	// 	mat = materialZero();
	// // 	mat.albedo = vec3(0.9f, 0.5f, 0.9f);
	// // 	mat.emissive = vec3(0.0f, 0.0f, 0.0f);
	// // 	mat.specularChance = 0.3f;
	// // 	mat.specularRoughness = 0.2;
	// // 	mat.specularColor = vec3(0.9f, 0.9f, 0.9f);
	// 	mat.IOR = 1.1f;
	// 	mat.refractionChance = 1.00f;
	// 	mat.refractionColor = vec3(0.1f, 0.4f, 1.0f);
	// }
	// if (cylinderIntersection(ray, rec, vec3(0.0f, -9.0f, 20.0f) + sceneTranslation, vec3(0.0f, 0.0f, 1.0f), 2.0f, 2.5f)) {
	// 	mat = materialZero();
	// 	mat.albedo = vec3(0.9f, 0.4f, 0.0f);
	// 	mat.specularChance = 0.1f;
	// 	mat.specularColor = vec3(0.9f, 0.4f, 0.0f);
	// 	mat.IOR = 1.0f;
	// 	mat.refractionChance = 1.00f;
	// 	mat.refractionColor = vec3(0.1f, 0.4f, 1.0f);
	// }

	// // a ball which has blue diffuse but red specular. an example of a "bad mat".
	// // a better lighting model wouldn't let you do this sort of thing
	// if (sphereIntersection(ray, rec, vec4(9.0f, -9.4f, 20.0f, 3.0f) + sceneTranslation4)) {
	// 	mat = materialZero();
	// // 	mat.albedo = vec3(0.0f, 0.0f, 1.0f);
	// // 	mat.specularChance = .5f;
	// // 	mat.specularRoughness = 0.4f;
	// // 	mat.specularColor = vec3(1.0f, 0.0f, 0.0f);
	// 	mat.albedo = checkerboard(8.0f * rec.uv);
	// 	mat.specularChance = 0.5f;
	// 	mat.specularColor = checkerboard(8.0f * rec.uv);
	// }
	// if (cylinderIntersection(ray, rec, vec3(8.0f, -9.0f, 20.0f) + sceneTranslation, vec3(0.0f, 1.0f, -1.0f), 2.0f, 2.5f)) {
	// 	mat = materialZero();
	// 	mat.albedo = vec3(0.1f, 0.5f, 0.9f);
	// 	mat.specularChance = 1.0f;
	// 	mat.specularColor = vec3(0.1f, 0.5f, 0.9f);
	// }
	// {
	// 	vec3	pos	= vec3(   8.00f,  -7.20f,  18.20f) + sceneTranslation;
	// 	vec3	v0	= vec3(   2.00f,   0.00f,   0.00f) + pos;
	// 	vec3	v1	= vec3(   0.00f,   1.50f,   1.50f) + pos;
	// 	if (quadIntersection(ray, rec, pos, v0, v1)) {
	// 		mat = materialZero();
	// 		mat.albedo = vec3(0.1f, 0.5f, 0.9f);
	// 		mat.specularChance = 1.0f;
	// 		mat.specularColor = vec3(0.1f, 0.5f, 0.9f);
	// 	}
	// }

	// if (sphereIntersection(ray, rec, vec4(1.5f, 0.0f, 23.0f, 1.75f) + sceneTranslation4)) {
	// 	mat = materialZero();
	// 	mat.albedo = vec3(1.0f, 1.0f, 1.0f);
	// 	mat.specularChance = 1.0f;
	// 	mat.specularRoughness = 1.0f;
	// 	mat.specularColor = vec3(0.3f, 1.0f, 0.3f);
	// }

	if (cylinderIntersection(ray, rec, vec3(0.0f, -8.5f, 20.0f) + sceneTranslation, vec3(0.0f, 0.0f, 1.0f), 2.0f, 2.5f)) {
		mat = materialZero();
		mat.albedo = vec3(0.9f, 0.4f, 0.0f);
		mat.specularChance = 0.00f;
		mat.specularColor = vec3(0.9f, 0.4f, 0.0f);
		mat.IOR = 1.1f;
		mat.refractionChance = 1.0f;
		mat.refractionColor = vec3(0.1f, 0.4f, 1.0f);
	}
	// {
	// 	mat4	txi = translate( 0.0f, -8.5f,  20.0f + sceneTranslation.z) * rotationAxisAngle(vec3(0.0f, 1.0f, 0.0f), 0.0f);
	// 	mat4	txx = rotationAxisAngle(vec3(0.0f, 1.0f, 0.0f), 0.0f) * translate( 0.0f,  8.5f, -20.0f - sceneTranslation.z);
	// 	if (boxIntersection(ray, rec, txx, txi, vec3(2.0f, 2.0f, 2.5f))) {
	// 		mat = materialZero();
	// 		mat.albedo = vec3(0.9f, 0.4f, 0.0f);
	// 		mat.specularChance = 0.02f;
	// 		mat.specularColor = vec3(0.9f, 0.4f, 0.0f);
	// 		mat.IOR = 1.1f;
	// 		mat.refractionChance = 1.0f;
	// 		mat.refractionColor = vec3(0.1f, 0.4f, 1.0f);
	// 	}
	// }
	// if (sphereIntersection(ray, rec, vec4(0.0f, -9.0f, 20.0f, 3.0f) + sceneTranslation4)) {
	// 	mat = materialZero();
	// 	mat.albedo = vec3(0.9f, 0.4f, 0.0f);
	// 	mat.specularChance = 0.0f;
	// 	mat.specularColor = vec3(0.9f, 0.4f, 0.0f);
	// 	mat.IOR = 1.1f;
	// 	mat.refractionChance = 1.0f;
	// 	mat.refractionColor = vec3(0.1f, 0.4f, 1.0f);
	// }
}

float FresnelReflectAmount(float n1, float n2, vec3 normal, vec3 incident, float f0, float f90) {
	// Schlick aproximation
	float r0 = (n1 - n2) / (n1 + n2);
	r0 *= r0;
	float cosX = -dot(normal, incident);
	if (n1 > n2)
	{
		float n = n1 / n2;
		float sinT2 = n * n * (1.0 - cosX * cosX);
		// Total internal reflection
		if (sinT2 > 1.0) return f90;
		cosX = sqrt(1.0 - sinT2);
	}
	float x = 1.0 - cosX;
	float ret = r0 + (1.0 - r0) * x * x * x * x * x;

	// adjust reflect multiplier for object reflectivity
	return mix(f0, f90, ret);
}

vec3	GetColorForRay(vec3 startRayPos, vec3 startRayDir, inout uint rngState)
{
	// initialize
	vec3	ret = vec3(0.0f, 0.0f, 0.0f);
	vec3	throughput = vec3(1.0f, 1.0f, 1.0f);
	SRay	ray = SRay(startRayPos, startRayDir);

	for (int bounceIndex = 0; bounceIndex <= c_numBounces; ++bounceIndex) {
		// shoot a ray out into the world
		SHitRecord	rec;
		SMaterial	mat;
		rec.dist = c_superFar;
		sceneIntersection(ray, rec, mat, rngState);

		// if the ray missed, we are done
		if (rec.dist == c_superFar) {
			ret += SRGBToLinear(texture(skybox, ray.dir).rgb) * c_skyboxBrightnessMultiplier * throughput;
			break;
		}

		// do absorption if we are hitting from inside the object
		if (!rec.frontFace)
			throughput *= exp(-mat.refractionColor * rec.dist);

		// get the pre-fresnel chances
		float specularChance = mat.specularChance;
		float refractionChance = mat.refractionChance;

		// take fresnel into account for specularChance and adjust other chances.
		// specular takes priority.
		// chanceMultiplier makes sure we keep diffuse / refraction ratio the same.
		float rayProbability = 1.0f;
		if (specularChance > 0.0f)
		{
			specularChance = FresnelReflectAmount(
				!rec.frontFace ? mat.IOR : 1.0,
				rec.frontFace ? mat.IOR : 1.0,
				ray.dir, rec.normal, mat.specularChance, 1.0f);

			float chanceMultiplier = (1.0f - specularChance) / (1.0f - mat.specularChance);
			refractionChance *= chanceMultiplier;
		}

		// calculate whether we are going to do a diffuse, specular, or refractive ray
		float doSpecular = 0.0f;
		float doRefraction = 0.0f;
		float raySelectRoll = RandomFloat01(rngState);
		if (specularChance > 0.0f && raySelectRoll < specularChance) {
			doSpecular = 1.0f;
			rayProbability = specularChance;
		}
		else if (refractionChance > 0.0f && raySelectRoll < specularChance + refractionChance) {
			doRefraction = 1.0f;
			rayProbability = refractionChance;
		}
		else
			rayProbability = 1.0f - (specularChance + refractionChance);

		// numerical problems can cause rayProbability to become small enough to cause a divide by zero.
		rayProbability = max(rayProbability, 0.001f);

		// update the ray position
		ray.ori = (ray.ori + ray.dir * rec.dist) + (doRefraction == 1.0f ? -rec.normal : rec.normal) * c_rayPosNormalNudge;

		// Calculate a new ray direction.
		// Diffuse uses a normal oriented cosine weighted hemisphere sample.
		// Perfectly smooth specular uses the reflection ray.
		// Rough (glossy) specular lerps from the smooth specular to the rough diffuse by the mat specularRoughness squared
		// Squaring the specularRoughness is just a convention to make specularRoughness feel more linear perceptually.
		vec3 diffuseRayDir = normalize(rec.normal + RandomUnitVector(rngState));

		vec3 specularRayDir = reflect(ray.dir, rec.normal);
		specularRayDir = normalize(mix(specularRayDir, diffuseRayDir, mat.specularRoughness * mat.specularRoughness));

		vec3 refractionRayDir = refract(ray.dir, rec.normal, !rec.frontFace ? mat.IOR : 1.0f / mat.IOR);
		refractionRayDir = normalize(mix(refractionRayDir, normalize(-rec.normal + RandomUnitVector(rngState)), mat.refractionRoughness * mat.refractionRoughness));

		ray.dir = mix(diffuseRayDir, specularRayDir, doSpecular);
		ray.dir = mix(ray.dir, refractionRayDir, doRefraction);

		// add in emissive lighting
		ret += mat.emissive * throughput;

		// update the colorMultiplier
		if (doRefraction == 0.0f)
			throughput *= mix(mat.albedo, mat.specularColor, doSpecular);

		// since we chose randomly between diffuse, specular, refract,
		// we need to account for the times we didn't do one or the other.
		throughput /= rayProbability;

		// Russian Roulette
		// As the throughput gets smaller, the ray is more likely to get terminated early.
		// Survivors have their value boosted to make up for fewer samples being in the average.
		{
			float p = max(throughput.r, max(throughput.g, throughput.b));
			if (RandomFloat01(rngState) > p)
				break;

			// Add the energy we 'lose' by randomly terminating paths
			throughput *= 1.0f / p;
		}
	}

	// return pixel color
	return ret;
}

void main()
{
	// initialize a random number state based on frag coord and frame
	uint rngState = uint(uint(fragCoord.x) * uint(1973) + uint(fragCoord.y) * uint(9277) + uint(iFrame) * uint(26699)) | uint(1);

	// The ray starts at the camera position (the origin)
	vec3 rayPosition = vec3(0.0f, 0.0f, 0.0f);

	// calculate the camera distance
	float cameraDistance = 1.0f / tan(c_FOVDegrees * 0.5f * c_pi / 180.0f);

	// calculate subpixel camera jitter for anti aliasing
	vec2 jitter = vec2(RandomFloat01(rngState), RandomFloat01(rngState)) - 0.5f;

	// calculate coordinates of the ray target on the imaginary pixel plane.
	// -1 to +1 on x,y axis. 1 unit away on the z axis
	vec3 rayTarget = vec3(((fragCoord+jitter)/iResolution.xy) * 2.0f - 1.0f, cameraDistance);

	// correct for aspect ratio
	float aspectRatio = iResolution.x / iResolution.y;
	rayTarget.y /= aspectRatio;

	// calculate a normalized vector for the ray direction.
	// it's pointing from the ray position to the ray target.
	vec3 rayDir = normalize(rayTarget - rayPosition);

	// raytrace for this pixel
	vec3 color = vec3(0.0f, 0.0f, 0.0f);
	for (int index = 0; index < c_numRendersPerFrame; ++index)
		color += GetColorForRay(rayPosition, rayDir, rngState) / float(c_numRendersPerFrame);

	// // average the frames together
	// vec4 lastFrameColor = texture(iChannel0, texCoord);
	// float blend = lastFrameColor.a == 0.0f ? 1.0f : 1.0f / (1.0f + (1.0f / lastFrameColor.a));
	// color = mix(lastFrameColor.rgb, color, blend);

	// // show the result
	// fragColor = vec4(color, blend);

	// average the frames together
	vec3 lastFrameColor = texture(iChannel0, texCoord).rgb;
	color = mix(lastFrameColor, color, 1.0f / float(iFrame+1));

	// show the result
	fragColor = vec4(color, 1.0f);
}
