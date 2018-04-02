varying vec2 v_texcoord;

void main()
{
	v_texcoord = gl_MultiTexCoord0.xy;
	gl_Position = ftransform();
}

//____FRAGMENT_SHADER____

varying vec2 v_texcoord;

uniform sampler2D u_source1;
uniform sampler2D u_source2;
uniform sampler2D u_source3;
uniform sampler2D u_source4;
uniform float u_glowbr;

void main()
{
	vec3 orig = texture2D(u_source1, v_texcoord).rgb;
	vec3 glow1 = texture2D(u_source2, v_texcoord).rgb;
	vec3 glow2 = texture2D(u_source3, v_texcoord).rgb;
	vec3 glow3 = texture2D(u_source4, v_texcoord).rgb;

	vec3 glow = pow((glow1 + glow2 + glow3) * u_glowbr, vec3(0.35, 0.5, 0.4));
	glow = pow(glow, vec3(2.0));
	orig = pow(orig * vec3(0.9, 1.1, 0.9), vec3(0.75));

  	vec3 col = glow + orig;
	gl_FragColor = vec4(col, 1.0);
}


