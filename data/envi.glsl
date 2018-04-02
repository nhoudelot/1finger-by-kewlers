varying vec3 v_normal;

void main()
{
	v_normal = gl_NormalMatrix * gl_Normal;
	gl_Position = ftransform();
}

//____FRAGMENT_SHADER____

varying vec3 v_normal;
uniform sampler2D u_envitex;

void main()
{
  gl_FragColor = texture2D(u_envitex, v_normal.xy * 0.5 + 0.5);
}
