varying vec3 v_pos;
varying vec2 v_tex;
varying vec3 v_col;

void main()
{
	v_pos = gl_Vertex.xyz;
	v_tex = gl_MultiTexCoord0.xy;
	v_col = gl_Color.rgb;
	gl_Position = ftransform();
}

//____FRAGMENT_SHADER____


varying vec3 v_pos;
varying vec2 v_tex;
varying vec3 v_col;

uniform sampler2D u_depthtex;

// noise funcs shamelessly copied from iq/rgba
// http://www.iquilezles.org/

float hash( float n )
{
    return fract(sin(n)*43758.5453);
}

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);

    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*57.0 + 113.0*p.z;

    float res = mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                        mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
                    mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                        mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
    return res;
}

float fbm( vec3 p )
{
    float f;
    f  = 0.5000*noise( p ); p = p*2.02;
    f += 0.2500*noise( p ); p = p*2.03;
    f += 0.1250*noise( p ); p = p*2.01;
    f += 0.0625*noise( p ); 
    return f;
}


void main()
{
	float dist = 1000.0 -texture2D(u_depthtex, vec2(0.5, v_tex.y)).r;
	float br = 1.0;
	if (dist > v_tex.x) br = 1.0;
	else br = 0.0;

	float smoke = sin(fbm(v_pos * 0.125) * 31.415 * 0.25) * 0.20 + 0.50;
	smoke = clamp(smoke, 0.0, 0.5) + 0.5;
	smoke = smoke * (1.0 - fbm(v_pos) * 0.3);
	gl_FragColor = vec4(vec3(smoke) * v_col * br, 1.0);

	//br = dist;
	//gl_FragColor = vec4(vec3(br), 1.0);
}


