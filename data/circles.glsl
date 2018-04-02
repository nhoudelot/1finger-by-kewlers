varying vec2 v_pos;

void main()
{
	v_pos = gl_Vertex.xy;
	gl_Position = ftransform();
}

//____FRAGMENT_SHADER____

varying vec2 v_pos;
const int numCircles = 3;
uniform float u_time, u_aspect;
uniform vec3 u_col;

void main()
{
	float col = 0.0;

	for (int i = 0; i < numCircles; i++)
	{
		vec2 cpos = vec2(sin(float(i * 2) + u_time * 5.86) * 0.75, sin(float(i * 3) + u_time * 7.71) * 0.66);
		float dist = distance(cpos, v_pos * vec2(1.0, u_aspect));
		float ccol = sin(dist * 128.0) * 8.0;
		ccol = clamp(ccol, -1.0, 1.0) * 0.5 + 0.5;
		col = mix(col, 1.0 - col, ccol);
	}
	
	gl_FragColor = vec4(col * u_col, 1.0);
}