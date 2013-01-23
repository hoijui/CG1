void main()
{
	// setup correct position
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	/* "In order to perform texturing operations in GLSL we need to have access to the texture coordinates per vertex. GLSL provides some attribute variables, one for each texture unit:

	attribute vec4 gl_MultiTexCoord0;
	attribute vec4 gl_MultiTexCoord1;
	...
	The vertex shader has access to the attributes defined above to get the texture coordinates specified in the OpenGL application. Then it must compute the texture coordinate for the vertex and store it in the pre defined varying variable gl_TexCoord[i], where i indicates the texture unit.
	The simple following instruction sets the vertex texture coordinate for texture unit 0 just by copying the texture coordinate specified in the OpenGL application." */
	gl_TexCoord[0] = gl_MultiTexCoord0;

	// view vector (from camera to vertex)
	vec3 v = normalize( vec3(gl_ModelViewMatrix * gl_Vertex) );
	// surface normal
	vec3 n = normalize( gl_NormalMatrix * gl_Normal );
	// reflection vector
	// = 2 * dot(v,n) * n - v
	vec3 r = reflect(v, n);

	float m = 2.0 * sqrt(r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0));
	// u
	gl_TexCoord[1].s = r.x/m + 0.5;
	// v
	gl_TexCoord[1].t = r.y/m + 0.5;
}
