/**
 * Path tracing implementation coded in GLSL.
 *
 * Acknowledgements:
 *		OpenGL GLSL
 *			learnopengl		@ https://learnopengl.com/
 *		Ray tracing tutorial
 *			Peter Shirley	@ https://raytracing.github.io/
 *		Ray-surface intersection
 *			inigo-quilez	@ https://iquilezles.org/articles/intersectors/
 *		Ray-surface intersection derivation
 *			Chris Dragan	@ https://hugi.scene.org/online/hugi24/coding%20graphics%20chris%20dragan%20raytracing%20shapes.htm
 *		Path tracing tutorial
 *			demofox			@ https://blog.demofox.org/2020/05/25/casual-shadertoy-path-tracing-1-basic-camera-diffuse-emissive/
 *		Direct point light sampling
 *			reinder			@ https://www.shadertoy.com/view/4tl3z4
 *			RichieSams		@ https://computergraphics.stackexchange.com/questions/5152/progressive-path-tracing-with-explicit-light-sampling
 *		Normal map texturing
 *			inigo-quilez	@ https://www.shadertoy.com/view/ldSGzR
 *			demofox			@ https://www.shadertoy.com/view/ldj3zz
 */

#version 400 core

uniform int iFrame;
uniform vec2 iResolution;
out vec4 fragColor;
vec2 fragCoord = gl_FragCoord.xy;

uniform samplerCube	skybox;
uniform sampler2D	framebuffer;

uniform sampler2D	tex02;
uniform sampler2D	tex03;
uniform sampler2D	tex04;
uniform sampler2D	tex05;
uniform sampler2D	tex06;
uniform sampler2D	tex07;
uniform sampler2D	tex08;
uniform sampler2D	tex09;
uniform sampler2D	tex10;
uniform sampler2D	tex11;
uniform sampler2D	tex12;
uniform sampler2D	tex13;
uniform sampler2D	tex14;
uniform sampler2D	tex15;

#define MAX_SIZE 256

#define SPHERE	0x00800
#define	CYLND	0x02000
#define	CONE	0x04000
#define	BOX		0x08000
#define PLANE	0x01000
#define	QUAD	0x10000
#define	DISK	0x20000
#define	TRIAGL	0x40000

struct	SMaterial {
	vec4	albedo;			// the color used for diffuse lighting
	vec4	emissive;		// how much the surface glows
	vec4 	specColor;		// the color tint of specular reflections
	vec4	refrColor;		// absorption for beer's law
	vec4	normalMap;		// normal-altering texture map
	float	intensity;		// emissive multiplier
	float	specChance;		// percentage chance of doing a specular reflection
	float	specRoughness;	// how rough the specular reflections are
	float	IOR;			// index of refraction used by fresnel and refraction
	float	refrChance;		// percent chance of doing a refractive transmission
	float	refrRoughness;	// how rough the refractive transmissions are
};

struct SObject {
	ivec4	type;
	vec3	pos;
	vec3	axis;
	vec4	param;
};

struct SPointLight {
	vec3	pos;
	vec4	color;
};

struct	SHitRecord {
	bool	front;	// whether intersection occurs on geometry's front face
	float	t;		// distance from ray origin to intersection point
	vec3	p;		// intersection point
	vec3	normal;	// normal vector at intersection point
	vec2	uv;		// local coordinate
};

struct	SRay {
	vec3	ori;	// ray origin
	vec3	dir;	// ray direction
};

layout (std140) uniform Objects {
	int			nObj;
	SObject		camera;
	SObject		objects[MAX_SIZE];
	SMaterial	materials[MAX_SIZE];
};

layout (std140) uniform Lights {
	int			nLight;
	vec4		ambient;
	SPointLight	lights[MAX_SIZE];
};

// The minimunm distance a ray must travel before we consider an intersection.
// This is to prevent a ray from intersecting a surface it just bounced off of.
const float MINIMUM_DIST = 0.001f;

// after a hit, it moves the ray this far along the normal away from a surface.
// Helps prevent incorrect intersections when rays bounce off of objects.
const float NORMAL_NUDGE = 0.001f;

// the farthest we look for ray hits
const float MAXIMUM_DIST = 10000.0f;

// a multiplier for the skybox brightness
const float SKYBOX_INTENSITY = 1.0f;

// a multiplier for pointlight brightness
const float POINTLIGHT_INTENSITY = 4000.0f;

// tuned Blinn-Phong parameters to simulate PBR shading
const float SHININESS = 4096.0f;
const float SHININESS_DECAY = 10.0f;
const float ATTENUATION_COMPENSATION = 48.0f;

// attenuation compensation for pointlight sources for PBR shading
const float ATTENUATION_COMPENSATION_PBR = 128.0f;

// number of ray bounces allowed
const int NUM_BOUNCES = 16;

// how many renders per frame - to get around the vsync limitation.
const int NUM_RENDER = 8;

// whether to use PBR shading on geometries illuminated by point light
const bool PBR = true;

const float PI = 3.14159265359f;
const float TWOPI = 2.0f * PI;

//-----------------------------------------------------
// Utility functions
//-----------------------------------------------------
vec3	lessThan(vec3 f, float value) {
	return vec3((f.x < value) ? 1.0f : 0.0f,
				(f.y < value) ? 1.0f : 0.0f,
				(f.z < value) ? 1.0f : 0.0f);
}

// Tone conversion functions
//-----------------------------------------------------
vec3	sRGBToLinear(vec3 rgb) {
	rgb = clamp(rgb, 0.0f, 1.0f);

	return mix(pow(((rgb + 0.055f) / 1.055f), vec3(2.4f)),
			   rgb / 12.92f,
			   lessThan(rgb, 0.04045f));
}

// Pseudorandom number generator
uint rand_pcg(inout uint seed) {
	uint state = seed;
	seed = seed * 747796405u + 2891336453u;
	uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

float randomFloat(inout uint state) {
	return float(rand_pcg(state)) / 4294967296.0;
}

vec3	randomUnitVector(inout uint state) {
	float	z = randomFloat(state) * 2.0f - 1.0f;
	float	a = randomFloat(state) * TWOPI;
	float	r = sqrt(1.0f - z * z);
	float	x = r * cos(a);
	float	y = r * sin(a);
	return vec3(x, y, z);
}

//-----------------------------------------------------
// Matrix utility functions
//-----------------------------------------------------
// Rodrigues' axis-angle formula
mat4	rotationAxisAngle(vec3 v, float angle) {
	float	s = sin(angle);
	float	c = cos(angle);
	float	ic = 1.0 - c;

	return mat4(v.x*v.x*ic + c,	 v.y*v.x*ic - s*v.z, v.z*v.x*ic + s*v.y, 0.0f,
				v.x*v.y*ic + s*v.z, v.y*v.y*ic + c,	 v.z*v.y*ic - s*v.x, 0.0f,
				v.x*v.z*ic - s*v.y, v.y*v.z*ic + s*v.x, v.z*v.z*ic + c,	 0.0f,
				0.0f,				 0.0f,				 0.0f,			 1.0f);
}

mat4	translate(vec3 t) {
	return mat4(1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				t.x,  t.y,	t.z,  1.0f);
}

//-----------------------------------------------------
// Texturing functions
//-----------------------------------------------------
vec4	getTexture(float id, vec2 uv) {
	if (id <  2.5f)	return texture(tex02, uv);
	if (id <  3.5f)	return texture(tex03, uv);
	if (id <  4.5f)	return texture(tex04, uv);
	if (id <  5.5f)	return texture(tex05, uv);
	if (id <  6.5f)	return texture(tex06, uv);
	if (id <  7.5f)	return texture(tex07, uv);
	if (id <  8.5f)	return texture(tex08, uv);
	if (id <  9.5f)	return texture(tex09, uv);
	if (id < 10.5f)	return texture(tex10, uv);
	if (id < 11.5f)	return texture(tex11, uv);
	if (id < 12.5f)	return texture(tex12, uv);
	if (id < 13.5f)	return texture(tex13, uv);
	if (id < 14.5f)	return texture(tex14, uv);
	return texture(tex15, uv);
}

// Bump texturing
vec3	doBump(vec2 uv, float id, vec3 nor, float scale) {
	vec3	u = cross(vec3(0.0f, 1.0f, 0.0f), nor);
	vec3	v = cross(nor, u);
	float	d = dot(v, u);

	float	eps = 0.005f;
	float	signal = dot(getTexture(id, uv).rgb, vec3(0.33f));
	float	dbdx = dot(getTexture(id, vec2(eps, 0.0f) + uv).rgb, vec3(0.33f)) - signal;
	float	dbdy = dot(getTexture(id, vec2(0.0f, eps) + uv).rgb, vec3(0.33f)) - signal;

	vec3	surfGrad = dbdx * u + dbdy * v;
	return normalize(abs(d) * nor - sign(d) * scale * surfGrad);
}

// Chekerboard texturing
vec3	checkerboard(vec2 uv) {
	vec3 	col = vec3(0.2f);
	col += 0.4f * smoothstep(-0.01f, 0.01f, cos(uv.x * 0.5f) * cos(uv.y * 0.5f));
	col *= smoothstep(-1.0f, -0.98f, cos(uv.x)) * smoothstep(-1.0f, -0.98f, cos(uv.y));
	return col;
}

vec3	getMaterialColor(SHitRecord rec, vec4 param) {
	if (param.w < -0.5f)	return checkerboard(param.xy * rec.uv);
	if (param.w <  0.5f)	return param.rgb;
	return getTexture(param.w, param.xy + rec.uv).rgb;
}

vec3	getNormal(SHitRecord rec, vec4 param) {
	if (param.w < 0.5f)	return rec.normal;
	return doBump(param.xy + rec.uv, param.w, rec.normal, param.z);
}

//-----------------------------------------------------
// Intersection functions
//-----------------------------------------------------
bool	sphereIntersection(SRay ray, inout SHitRecord rec, SObject object) {
	// translate to local coordinate
	vec3	m = ray.ori - object.pos;

	// calculate the quadratic coeeficients
	float	b = dot(m, ray.dir);
	float	c = dot(m, m) - object.param.x * object.param.x;

	// exit if r originates outside sp (c > 0) and r points away from sp (b > 0)
	if (c > 0.0f && b > 0.0f)	return false;

	// r does not intersect sp
	float	dis = b * b - c;
	if (dis < 0.0f)	return false;

	// r intersects sp, compute smallest t value of intersection
	float	sqrtd = sqrt(dis);
	float	t = -b - sqrtd;
	bool	front;
	front = true;
	if (t <= MINIMUM_DIST) {
		front = false;
		t = -b + sqrtd;
	}
	// t value larger than record
	if (t <= MINIMUM_DIST || rec.t <= t)	return false;

	// save intersection information in record
	vec3	p = (m + ray.dir * t) / object.param.x;
	rec.t = t;
	rec.front = front;
	rec.normal = p * (rec.front ? 1.0f : -1.0f);
	rec.uv = vec2((atan(p.z, p.x) + PI) / (2 * PI), acos(-p.y) / PI);
	return true;
}

bool	boxIntersection(SRay ray, inout SHitRecord rec, SObject object) {
	// box-to-world transformation
	mat4	txi = translate( object.pos) * rotationAxisAngle(object.axis,  object.param.w * PI / 180.0f);
	// world-to-box transformation
	mat4	txx = rotationAxisAngle(object.axis, -object.param.w * PI / 180.0f) * translate(-object.pos);

	// convert from world to box space
	vec3	rd = (txx * vec4(ray.dir, 0.0f)).xyz;
	vec3	ro = (txx * vec4(ray.ori, 1.0f)).xyz;

	// ray-box intersection in box space
	vec3	m = 1.0f / rd;
	vec3	s = vec3((rd.x < 0.0f) ? 1.0f : -1.0f,
					 (rd.y < 0.0f) ? 1.0f : -1.0f,
					 (rd.z < 0.0f) ? 1.0f : -1.0f);
	vec3	t1 = m * (-ro + s * object.param.xyz);
	vec3	t2 = m * (-ro - s * object.param.xyz);

	// compute t near and t far intersections
	float	tN = max( max( t1.x, t1.y ), t1.z );
	float	tF = min( min( t2.x, t2.y ), t2.z );

	// no intersection
	if (tN > tF || tF < 0.0f)	return false;

	// r intersects bx
	bool	front;
	front = true;
	if (tN <= MINIMUM_DIST) {
		front = false;
		tN = tF;
	}
	// t value larger than record
	if (tN <= MINIMUM_DIST || rec.t <= tN)	return false;

	// save intersection information in record
	if (t1.x > t1.y && t1.x > t1.z) { rec.uv = (ro + tN * rd).yz; }
	else if (t1.y > t1.z)			{ rec.uv = (ro + tN * rd).zx; }
	else							{ rec.uv = (ro + tN * rd).xy; }

	// compute normal (in world space)
	rec.t = tN;
	rec.front = front;
	vec3 res = front ? step(vec3(tN), t1) : step(t2, vec3(tN));
	rec.normal = (txi * vec4(-sign(rd) * res, 0.0f)).xyz;
	return true;
}

bool	cylinderIntersection(SRay ray, inout SHitRecord rec, SObject object) {
	// translate to local coordinate
	vec3	m = ray.ori - object.pos;
	vec3	n = normalize(object.axis);
	float	nd = dot(n, ray.dir);
	float	nm = dot(n, m);

	// calculate the quadratic coefficients
	float	a = 1.0f - nd * nd;
	float	b = dot(m, ray.dir) - nm * nd;
	float	c = dot(m, m) - nm * nm - object.param.x * object.param.x;

	// exit if r originates outside cy (c > 0) and r points away from cy (b > 0)
	if (c > 0.0f && b > 0.0f)	return false;

	// r does not intersect cy
	float	dis = b * b - a * c;
	if (dis < 0.0f)	return false;

	// check intersection with cy
	bool	mFront, cFront;
	float	h, t, tm = MAXIMUM_DIST, tc = MAXIMUM_DIST;
	float	sqrtd = sqrt(dis);

	// get possible intersection on mantle
	mFront = true;
	t = (-b - sqrtd) / a;
	h = nm + t * nd;
	if (t <= MINIMUM_DIST || abs(h) > object.param.y) {
		mFront = false;
		t = (-b + sqrtd) / a;
		h = nm + t * nd;
	}
	if (abs(h) <= object.param.y)	tm = t;

	// get possible intersection on caps
	cFront = true;
	t = (-sign(nd) * object.param.y - nm) / nd;
	if (t <= MINIMUM_DIST || abs(b + t * a) > sqrtd) {
		cFront = false;
		t = (sign(nd) * object.param.y - nm) / nd;
	}
	if (abs(b + t * a) <= sqrtd)	tc = t;

	// t value larger than record
	t = min(tm, tc);
	if (t <= MINIMUM_DIST || rec.t <= t)	return false;

	// local coordinate calculation
	vec3	p = m + ray.dir * t;
	vec3	u = dot(n, vec3(1.0f, 1.0f, 0.0f)) == 0.0f ?
						vec3(0.0f, 1.0f, 0.0f) :
						normalize(cross(n, vec3(0.0f, 0.0f, 1.0f)));
	vec3	v = normalize(cross(u, n));
	vec3	q = p * mat3(u, v, n);

	// save intersection information in record
	rec.t = t;
	if (t == tm) {
		rec.front = mFront;
		rec.normal = ((p - h * n) / object.param.x) * (mFront ? 1.0f : -1.0f);
	} else {
		rec.front = cFront;
		rec.normal = n * -sign(nd);
	}
	rec.uv = vec2(2.0f * atan(q.y, q.x), length(q.xy) + q.z);
	return true;
}

bool	coneIntersection(SRay ray, inout SHitRecord rec, SObject object) {
	// translate to local coordinate
	vec3	m = ray.ori - object.pos;
	vec3	n = normalize(object.axis);
	float	nd = dot(n, ray.dir);
	float	nm = dot(n, m);

	// calculate the quadratic coefficients
	float	rm = dot(vec2(0.5f), object.param.xy);
	float	k = (object.param.y - object.param.x) / (2.0f * object.param.z);
	float	y = 1.0f + k * k;
	float	r = k * nm - rm;
	float	a = 1.0f - y * nd * nd;
	float	b = dot(m, ray.dir) - y * nm * nd + rm * k * nd;
	float	c = dot(m, m) - nm * nm - r * r;

	// exit if r originates outside cy (c > 0) and r points away from cy (b > 0)
	if (c > 0.0f && b > 0.0f)	return false;

	// r does not intersect sp
	float	dis = b * b - a * c;
	if (dis < 0.0f)	return false;

	// check intersection with cy
	bool	mFront, cFront;
	float	h, t, tm = MAXIMUM_DIST, tc = MAXIMUM_DIST;
	float	sqrtd = sqrt(dis);

	// get possible intersection on mantle
	mFront = true;
	t = (-b - sqrtd) / a;
	h = nm + t * nd;
	if (t <= MINIMUM_DIST || abs(h) > object.param.z) {
		mFront = false;
		t = (-b + sqrtd) / a;
		h = nm + t * nd;
	}
	if (abs(h) <= object.param.z)	tm = t;

	// get possible intersection on caps
	vec3	l;
	vec2	r2 = object.param.xy * object.param.xy;

	cFront = true;
	t = (-sign(nd) * object.param.z - nm) / nd;
	l = m + sign(nd) * object.param.z * n + ray.dir * t;
	if (t <= MINIMUM_DIST ||
		dot(l, l) > (cFront != nd > 0.0f ? r2.x : r2.y)) {
		cFront = false;
		t = (sign(nd) * object.param.z - nm) / nd;
		l = m - sign(nd) * object.param.z * n + ray.dir * t;
	}
	if (dot(l, l) > (cFront != nd > 0.0f ? r2.x : r2.y))	tc = t;

	// t value larger than record
	t = min(tm, tc);
	if (t <= MINIMUM_DIST || rec.t <= t)	return false;

	// local coordinate calculation
	vec3	p = m + ray.dir * t;
	vec3	u = dot(n, vec3(1.0f, 1.0f, 0.0f)) == 0.0f ?
					   vec3(0.0f, 1.0f, 0.0f) :
					   normalize(cross(n, vec3(0.0f, 0.0f, 1.0f)));
	vec3	v = normalize(cross(u, n));
	vec3	q = p * mat3(u, v, n);

	// save intersection information in record
	rec.t = t;
	if (t == tm) {
		rec.front = mFront;
		rec.normal = normalize(p - (y * h - rm * k) * n) * (mFront ? 1.0f : -1.0f);
	} else {
		rec.front = cFront;
		rec.normal = n * -sign(nd);
	}
	rec.uv = vec2(2.0f * atan(q.y, q.x), length(q.xy) + q.z);
	return true;
}

bool	planeIntersection(SRay ray, inout SHitRecord rec, SObject object) {
	// get the vector from the center of this plane to where the ray begins.
	vec3	m = ray.ori - object.pos;
	float	a = dot(ray.dir, object.axis);
	float	b = dot(m, object.axis);

	float	t = -b / a;
	// t value larger than record
	if (t <= MINIMUM_DIST || rec.t <= t)	return false;

	// r intersects pl
	rec.front = true;

	// local coordinate calculation
	vec3	p = ray.ori + t * ray.dir;
	vec3	n = normalize(object.axis);
	vec3	u = dot(n, vec3(1.0f, 1.0f, 0.0f)) == 0.0f ?
					   vec3(0.0f, 1.0f, 0.0f) :
					   normalize(cross(n, vec3(0.0f, 0.0f, 1.0f)));
	vec3	v = normalize(cross(u, n));

	// save intersection information in record
	rec.t = t;
	rec.normal = object.axis * (a < 0.0f ? 1.0f : -1.0f);

	// local coordinate calculation
	rec.uv = vec2(dot(p, u), dot(p, v));
	return true;
}

bool	quadIntersection(SRay ray, inout SHitRecord rec, SObject object) {
	// get the vector from the center of this quad to where the ray begins.
	vec3	a = object.pos - object.axis;
	vec3	b = object.param.xyz - object.pos;
	vec3	p = object.pos - ray.ori;

	vec3	n = cross(a, b);
	vec3	q = cross(ray.dir, p);

	float	i = 1.0f / dot(ray.dir, n);

	float	u = dot(q, a) * i;
	float	v = dot(q, b) * i;
	float	t = dot(n, p) * i;

	// no intersection
	if (u < -1.0f || u > 1.0f || v < -1.0f || v > 1.0f)	return false;
	// t value larger than record
	if (t <= MINIMUM_DIST || rec.t <= t)	return false;

	// r intersects qd
	rec.front = true;

	// save intersection information in record
	rec.t = t;
	rec.uv = vec2(u, v) / 2.0f + 0.5f;
	rec.normal = normalize(i < 0.0f ? n : -n);
	return true;
}

bool	triIntersection(SRay ray, SHitRecord rec, SObject object) {
	// get the vector from the center of this quad to where the ray begins.
	vec3	a = object.pos - object.axis;
	vec3	b = object.param.xyz - object.pos;
	vec3	p = object.pos - ray.ori;

	vec3	n = cross(a, b);
	vec3	q = cross(p, ray.dir);

	float	i = 1.0f / dot(ray.dir, n);

	float	u = dot(q, b) * i;
	float	v = dot(q, a) * i;
	float	t = dot(n, p) * i;

	// no intersection
	if(u < 0.0f || v < 0.0f || (u + v) > 1.0f )	return false;
	// t value larger than record
	if (t <= MINIMUM_DIST || rec.t <= t)	return false;

	// r intersects tr
	rec.front = true;

	// save intersection information in record
	rec.t = t;
	rec.uv = vec2(u, v);
	rec.normal = n * (i < 0.0f ? 1.0f : -1.0f);
	return true;
}

bool	diskIntersection(SRay ray, inout SHitRecord rec, SObject object) {
	// get the vector from the center of this quad to where the ray begins.
	vec3	m = ray.ori - object.pos;
	vec3	n = normalize(object.axis);

	float	a = dot(n, ray.dir);
	float	t = -dot(n, m) / a;
	vec3	p = m + ray.dir * t;

	// no intersection
	if (dot(p, p) > object.param.x * object.param.x) return false;
	// t value larger than record
	if (t <= MINIMUM_DIST || rec.t <= t)	return false;

	// r intersects disk
	rec.front = true;

	// save intersection information in record
	rec.t = t;
	rec.normal = normalize(a < 0.0f ? n : -n);

	// local coordinate calculation
	vec3	u = dot(n, vec3(1.0f, 1.0f, 0.0f)) == 0.0f ?
						vec3(0.0f, 1.0f, 0.0f) :
						normalize(cross(n, vec3(0.0f, 0.0f, 1.0f)));
	vec3	v = normalize(cross(u, n));
	vec3	q = p * mat3(u, v, n);
	rec.uv = vec2(2.0f * atan(q.y, q.x), length(q.xy) + q.z);
	return true;
}

//-----------------------------------------------------
// Scene functions
//-----------------------------------------------------
bool	objIntersection(SRay ray, SObject obj, inout SHitRecord rec) {
	switch (obj.type.x & 0xfff00) {
		case SPHERE:
			return sphereIntersection(ray, rec, obj);
		case CYLND:
			return cylinderIntersection(ray, rec, obj);
		case CONE:
			return coneIntersection(ray, rec, obj);
		case BOX:
			return boxIntersection(ray, rec, obj);
		case PLANE:
			return planeIntersection(ray, rec, obj);
		case QUAD:
			return quadIntersection(ray, rec, obj);
		case DISK:
			return diskIntersection(ray, rec, obj);
		case TRIAGL:
			return triIntersection(ray, rec, obj);
		default:
			return false;
	}
}

int	sceneIntersection(SRay ray, out SHitRecord rec) {
	int		id = -1;
	rec.t = MAXIMUM_DIST;

	for (int i = 0; i < nObj; ++i)
		if (objIntersection(ray, objects[i], rec))	id = i;

	return id;
}

int	boundingObjIntersection(SRay ray) {
	int			id = -1;
	float		t = MAXIMUM_DIST;
	SHitRecord	rec;

	for (int i = 0; i < nObj; ++i) {
		rec.t = MAXIMUM_DIST;
		if (objIntersection(ray, objects[i], rec) && !rec.front && rec.t < t) {
			t = rec.t;
			id = i;
		}
	}

	return id;
}

int	pointLightIntersection(SRay ray, inout SHitRecord rec) {
	int	id = -1;
	for (int i = 0; i < nLight; ++i) {
		SObject	pointLight;

		pointLight.pos = lights[i].pos;
		pointLight.param.x = 0.1f;
		if (sphereIntersection(ray, rec, pointLight))	id = i;
	}
	return id;
}

//-----------------------------------------------------
// BRDF functions
//-----------------------------------------------------
float	fresnelReflectAmount(float ior, vec3 normal, vec3 incident, float p) {
	// Schlick aproximation
	float	r0 = (1.0f - ior) / (1.0f + ior);
	r0 *= r0;
	float	cosT = -dot(normal, incident);
	if (ior < 1.0f) {
		float	n = 1.0f / ior;
		float	sinT2 = n * n * (1.0 - cosT * cosT);
		// total internal reflection
		if (sinT2 > 1.0f) return 1.0f;
		cosT = sqrt(1.0f - sinT2);
	}
	float	x = 1.0f - cosT;
	float	t = r0 + (1.0f - r0) * x * x * x * x * x;

	// adjust reflect multiplier for object reflectivity
	return mix(p, 1.0f, t);
}

float	getInteraction(SRay ray, SHitRecord rec, SMaterial mat, float cIOR,
					   inout uint rngState, out float doSpecular, out float doRefraction) {
	// get the pre-fresnel chances
	float	specChance = mat.specChance;
	float	refrChance = mat.refrChance;

	// Take fresnel into account for specChance and adjust other chances.
	// Specular takes priority.
	// ChanceMultiplier makes sure we keep diffuse / refraction ratio the same.
	float rayProbability = 1.0f;
	if (specChance > 0.0f && cIOR != mat.IOR) {
		specChance = fresnelReflectAmount(
			rec.front ? mat.IOR / cIOR : cIOR / mat.IOR,
			rec.normal, ray.dir, specChance);

		float	chanceMultiplier = (1.0f - specChance) / (1.0f - mat.specChance);
		refrChance *= chanceMultiplier;
	}

	// calculate whether we are going to do a diffuse, specular, or refractive ray
	doSpecular = 0.0f;
	doRefraction = 0.0f;
	float	raySelectRoll = randomFloat(rngState);
	if (specChance > 0.0f && raySelectRoll < specChance) {
		doSpecular = 1.0f;
		rayProbability = specChance;
	} else
	if (refrChance > 0.0f && raySelectRoll < specChance + refrChance) {
		doRefraction = 1.0f;
		rayProbability = refrChance;
	} else
		rayProbability = 1.0f - (specChance + refrChance);

	// numerical problems can cause rayProbability to become small enough to cause a divide by zero.
	rayProbability = max(rayProbability, 0.001f);

	return	rayProbability;
}

vec3	evaluateInteraction(inout SRay ray, SHitRecord rec, SMaterial mat, float cIOR,
							inout uint rngState, float doSpecular, float doRefraction) {
	// Calculate a new ray direction based on the sampling probability.
	// Diffuse uses a normal oriented cosine weighted hemisphere sample.
	// Perfectly smooth specular uses the reflection ray.
	// Rough (glossy) specular lerps from the smooth specular to the rough diffuse by the mat specRoughness squared
	// Squaring the specRoughness is just a convention to make specRoughness feel more linear perceptually.
	vec3	diffuseRayDir = normalize(rec.normal + randomUnitVector(rngState));

	vec3	specularRayDir = reflect(ray.dir, rec.normal);
	specularRayDir = normalize(mix(specularRayDir, diffuseRayDir, mat.specRoughness * mat.specRoughness));

	vec3	refractionRayDir = refract(ray.dir, rec.normal, !rec.front ? mat.IOR / cIOR : cIOR / mat.IOR);
	if (refractionRayDir == vec3(0.0f) && doSpecular == 0.0f) {
		doRefraction = 0.0f;
		doSpecular = 1.0f;
	}
	else
		refractionRayDir = normalize(mix(refractionRayDir, -diffuseRayDir, mat.refrRoughness * mat.refrRoughness));

	// update the ray position
	ray.ori = ray.ori + ray.dir * rec.t +
			  (doRefraction == 1.0f ? -rec.normal : rec.normal) * NORMAL_NUDGE;

	ray.dir = mix(diffuseRayDir, refractionRayDir, doRefraction);
	ray.dir = mix(ray.dir, specularRayDir, doSpecular);

	if (doRefraction == 1.0f)
		return vec3(1.0f);
	return mix(getMaterialColor(rec, mat.albedo),
			   getMaterialColor(rec, mat.specColor), doSpecular);
}

//-----------------------------------------------------
// PBR functions
//-----------------------------------------------------
vec3 fresnelSchlick(float cosT, vec3 f0) {
	float	x = clamp(1.0f - cosT, 0.0f, 1.0f);
	return f0 + (1.0f - f0) * x * x * x * x * x;
}

float distributionGGX(float nh, float roughness) {
	float	a = roughness * roughness * roughness * roughness;

	float	nu = a;
	float	de = (nh * nh * (a - 1.0f) + 1.0f);
	de = PI * de * de;

	return nu / de;
}

float geometrySchlickGGX(float nv, float roughness) {
	float r = roughness + 1.0f;
	float k = r * r / 8.0f;

	float nu = nv;
	float de = nv * (1.0f - k) + k;

	return nu / de;
}

float geometrySmith(float nv, float nl, float roughness) {
	float	ggx2  = geometrySchlickGGX(nv, roughness);
	float	ggx1  = geometrySchlickGGX(nl, roughness);

	return ggx1 * ggx2;
}

//-----------------------------------------------------
// Point light functions
//-----------------------------------------------------
vec3	samplePointLight(SRay ray, SHitRecord rec, SMaterial mat, vec3 throughput,
						 inout uint rngState, float doSpecular, float doRefraction) {
	if (nLight == 0)	return vec3(0.0f);

	// uniformly choose between available point lights
	SPointLight	light = lights[int(floor(randomFloat(rngState) * nLight))];

	vec3		p = ray.ori + ray.dir * rec.t +
					(doRefraction == 1.0f ? -rec.normal : rec.normal) * NORMAL_NUDGE;
	vec3		ld = normalize(light.pos - p);
	float		d = length(light.pos - p);

	// shoot shadow ray
	SRay		shRay = SRay(p, ld);
	SHitRecord	shRec;

	shRec.t = MAXIMUM_DIST;
	int	id = sceneIntersection(shRay, shRec);

	// initialize
	vec3	n = rec.normal;
	vec3	v = -ray.dir;
	vec3	l = ld;
	vec3	h = normalize(l + v);
	float	nl = max(dot(n, l), 0.0f);
	float	nh = max(dot(n, h), 0.0f);

	// calculate light radiance
	float	attenuation = 1.0f / (d * d);
	vec3	radiance = light.color.rgb * light.color.w * attenuation;

	// only shade material in case of diffuse or specular
	if (doRefraction == 0.0f && shRec.t >= d) {
		if (PBR) {
			// PBR shading
			float	attComp = ATTENUATION_COMPENSATION_PBR;
			float	nv = max(dot(n, v), 0.0f);
			float	f0 = (1.0f - mat.IOR) / (1.0f + mat.IOR);
			vec3	F0 = mix(vec3(f0 * f0), throughput, mat.specChance);

			// cook-torrance brdf
			float	NDF = distributionGGX(nh, mat.specRoughness);
			float	G = geometrySmith(nv, nl, mat.specRoughness);
			vec3	F = fresnelSchlick(max(dot(h, v), 0.0f), F0);

			vec3	kS = F;
			vec3	kD = vec3(1.0f) - kS;
			kD *= 1.0f - mat.specChance;

			vec3	nu = NDF * G * F;
			float	de = 4.0f * nv * nl + 0.0001f;
			vec3	specHighlight = nu / de;

			return nLight * radiance * attComp * nl *
						(kD / PI + max(specHighlight / max(throughput, vec3(0.001f)), vec3(0.0f)));
		} else {
			// Blinn-Phong shading
			float	attComp = ATTENUATION_COMPENSATION;
			float	shininess = SHININESS;
			float	shineDecay = SHININESS_DECAY;
			float	specHighlight = pow(nh, shininess * exp(-shineDecay * mat.specRoughness));

			return nLight * radiance * attComp * nl *
						(doSpecular == 0.0f ? 1.0f : specHighlight);
		}
	}
	return vec3(0.0f);
}

//-----------------------------------------------------
// Ray tracer functions
//-----------------------------------------------------
vec3	rayColor(SRay ray, inout uint rngState) {
	// initialize
	vec3	color		= vec3(0.0f);
	vec3	throughput	= vec3(1.0f);

	SHitRecord	rec;
	float	cIOR = 1.0f;
	float	doSpecular = 0.0f, doRefraction = 0.0f;
	for (int i = 0; i < NUM_BOUNCES; ++i) {
		// shoot a ray out into the world
		int	objID = sceneIntersection(ray, rec);

		SHitRecord	lRec = rec;
		int	litID = i == 0 ? -1 : pointLightIntersection(ray, lRec);

		// if the ray missed, we are done and return the background color
		if (objID < 0 && litID < 0) {
			color += throughput * SKYBOX_INTENSITY * sRGBToLinear(texture(skybox, ray.dir).rgb);
			break;
		}

		// fetch material
		SMaterial	mat;
		if (objID >= 0)
			mat = materials[objID];

		// fetch pointlight source
		SMaterial	lMat;
		if (litID >= 0) {
			lMat.emissive = vec4(lights[litID].color.rgb, 0.0f);
			lMat.intensity = lights[litID].color.a * POINTLIGHT_INTENSITY;
		}

		rec.normal = getNormal(rec, mat.normalMap);
		// do absorption if we are hitting from inside the object
		if (!rec.front || (rec.front && cIOR != 1.0f))
			throughput *= max(exp(-getMaterialColor(rec, mat.refrColor) * mat.intensity * rec.t), vec3(0.0f));

		// add in emissive lighting
		color += getMaterialColor(rec, mat.emissive) * mat.intensity * throughput;

		// doSpecular = 0.0f, doRefraction = 0.0f;
		if (litID >= 0 && (doSpecular == 1.0f || doRefraction == 1.0f))
			color += getMaterialColor(lRec, lMat.emissive) * lMat.intensity * throughput;

		{
			// get index-of-refraction at boundary
			if (!rec.front) {
				SRay	nRay;

				nRay.ori = ray.ori + ray.dir * rec.t - rec.normal * NORMAL_NUDGE;
				nRay.dir = ray.dir;
				int	nObjID = boundingObjIntersection(nRay);

				cIOR = nObjID == -1 ? 1.0f : materials[nObjID].IOR;
			}

			// sample ray interaction chances
			float	rayProbability = getInteraction(ray, rec, mat, cIOR, rngState, doSpecular, doRefraction);

			// Evaluate the ray interaction, accumulating the weight by updating the throughput
			// and calculating the ray origin and direction for the new ray.
			SRay	nRay = ray;
			throughput *= evaluateInteraction(nRay, rec, mat, cIOR, rngState, doSpecular, doRefraction);

			// Since we chose randomly between diffuse, specular, refract,
			// divide by the probability of choosing that specific interaction
			// to make the throughput unbiased.
			throughput /= rayProbability;

			// direct point light sampling
			color += throughput * samplePointLight(ray, rec, mat, throughput, rngState,
												   doSpecular, doRefraction);

			// update index-of-refraction to current medium
			if ((rec.front && doRefraction == 1.0f) || (!rec.front && doSpecular == 1.0f))
				cIOR = mat.IOR;

			// shoot a new ray
			ray = nRay;
		}

		// Russian Roulette
		// As the throughput gets smaller, the ray is more likely to get terminated early.
		// Survivors have their value boosted to make up for fewer samples being in the average.
		{
			float p = max(throughput.r, max(throughput.g, throughput.b));
			if (randomFloat(rngState) > p)
				break;

			// Add the energy we 'lose' by randomly terminating paths
			throughput *= 1.0f / p;
		}
	}
	// ambient
	if (rec.t != MAXIMUM_DIST)
		color += ambient.rgb * ambient.w * throughput;

	// return pixel color
	return color;
}

//-----------------------------------------------------
// Camera functions
//-----------------------------------------------------
void	getCameraVectors(out vec3 cameraPos, out vec3 cameraFwd, out vec3 cameraUp, out vec3 cameraRight) {
	cameraPos = camera.pos;
	cameraFwd = normalize(-camera.axis);
	cameraRight = normalize(cross(vec3(0.0f, 1.0f, 0.0f), cameraFwd));
	cameraUp = normalize(cross(cameraFwd, cameraRight));
}

//-----------------------------------------------------
// Main
//-----------------------------------------------------
void main() {
	// initialize a random number state based on frag coord and frame
	uint rngState = uint(uint(fragCoord.x) * uint(1973) + uint(fragCoord.y) * uint(9277) +
					uint(iFrame) * uint(26699)) | uint(1);

	// calculate subpixel camera jitter for anti aliasing
	vec2 jitter = vec2(randomFloat(rngState), randomFloat(rngState)) - 0.5f;

	// get the camera vectors
	vec3 cameraPos, cameraFwd, cameraUp, cameraRight;
	getCameraVectors(cameraPos, cameraFwd, cameraUp, cameraRight);
	vec3 rayDir;
	{
		// calculate a screen position from -1 to +1 on each axis
		vec2 uvJittered = (fragCoord + jitter) / iResolution;
		vec2 screen = uvJittered * 2.0f - 1.0f;

		// adjust for aspect ratio
		float aspectRatio = iResolution.x / iResolution.y;
		screen.y /= aspectRatio;

		// make a ray direction based on camera orientation and field of view angle
		float cameraDistance = tan((180.0f - camera.param.w) * PI / 360.0f);
		rayDir = vec3(screen, -cameraDistance);
		rayDir = normalize(mat3(cameraRight, cameraUp, cameraFwd) * rayDir);
	}

	// raytrace for this pixel
	vec3 color = vec3(0.0f, 0.0f, 0.0f);
	for (int index = 0; index < NUM_RENDER; ++index)
		color += rayColor(SRay(cameraPos, rayDir), rngState) / float(NUM_RENDER);

	// average the frames together
	vec4 lastFrameColor = texture(framebuffer, fragCoord / iResolution);
	float blend = (iFrame < 2 || lastFrameColor.a == 0.0f) ? 1.0f : 1.0f / (1.0f + (1.0f / lastFrameColor.a));
	color = mix(lastFrameColor.rgb, color, blend);

	// show the result
	fragColor = vec4(color, blend);
}
