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
	vec4 col =
		(texture2D(u_source, v_texcoord + vec2(-u_pixel.x, -u_pixel.y)) +
		 texture2D(u_source, v_texcoord + vec2( u_pixel.x, -u_pixel.y)) +
		 texture2D(u_source, v_texcoord + vec2( u_pixel.x,  u_pixel.y)) +
		 texture2D(u_source, v_texcoord + vec2(-u_pixel.x,  u_pixel.y))) * 0.25;
	gl_FragColor = clamp((col - 0.1) * 1.1, 0.0, 1.0);
}



