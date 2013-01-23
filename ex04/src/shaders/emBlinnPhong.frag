uniform sampler2D colorMap;
uniform sampler2D envMap;

void main (void)
{
	// color refers to the "normal" texture bound to 0
	vec4 color = texture2D(colorMap, gl_TexCoord[0].st);
	// env refers to the calculated env map
	vec4 env = texture2D(envMap, gl_TexCoord[1].st);

	gl_FragColor = color + env;
}
