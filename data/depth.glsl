varying float dist;

void main()
{
	vec4 pos = gl_ModelViewMatrix * gl_Vertex;
	dist = 1000.0 + pos.z;
	gl_Position = ftransform();
}

//____FRAGMENT_SHADER____

varying float dist;

void main()
{
	gl_FragColor = vec4(dist);
}
