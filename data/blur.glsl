varying vec2 v_texcoord;

void main()
{
	v_texcoord = gl_MultiTexCoord0.xy;
	gl_Position = ftransform();
}

//____FRAGMENT_SHADER____

varying vec2 v_texcoord;
uniform sampler2D u_source;
uniform vec2 u_pixel;
uniform int u_radius;

void main()
{
	vec3 col = vec3(0.0);

	for (int i = -u_radius; i <= u_radius; i++)
	{
		float mult = 1.0 - abs(float(i-u_radius));
		col.r += texture2D(u_source, v_texcoord + u_pixel * float(i) - u_pixel * 4.0).r;
		col.g += texture2D(u_source, v_texcoord + u_pixel * float(i)).g;
		col.b += texture2D(u_source, v_texcoord + u_pixel * float(i) + u_pixel * 4.0).b;
	}

	gl_FragColor = vec4(col / float(u_radius * 2), 1.0);
}