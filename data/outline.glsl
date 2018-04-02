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

void main()
{
	vec4 mid = texture2D(u_source, v_texcoord);
	vec4 a, b, c, d;
	
	a = texture2D(u_source, v_texcoord + vec2(-u_pixel.x, -u_pixel.y));
	b = texture2D(u_source, v_texcoord + vec2( u_pixel.x, -u_pixel.y));
	c = texture2D(u_source, v_texcoord + vec2( u_pixel.x,  u_pixel.y));
	d = texture2D(u_source, v_texcoord + vec2(-u_pixel.x,  u_pixel.y));

	vec4 col = abs(mid - a) + abs(mid - b) + abs(mid - c) + abs(mid - d);
	float alpha = (mid.a + a.a + b.a + c.a + d.a) * 0.2;
	gl_FragColor = vec4(col.rgb, alpha);
}