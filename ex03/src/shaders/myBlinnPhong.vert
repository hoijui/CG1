// variables that are interpolated to the fragment shader
varying vec3 N; // nomal
varying vec3 L; // light direction
varying vec3 H; // halv vector

void main() {
	vec3 E; // eye vector

	/* "The vertex shader has access to the normals, as specified in the OpenGL application, through the attribute variable gl_Normal. This is the normal as defined in the OpenGL application with the glNormal function, hence in model local space. If no rotations or scales are performed on the model in the OpenGL application, then the normal defined in world space, provided to the vertex shader as gl_Normal, coincides with the normal defined in the local space. The normal is a direction and therefore it is not affected by translations." */
	// get normal, transform into eye space
	N = normalize(gl_NormalMatrix * gl_Normal);

	/* "Note that according to the OpenGL specification, the light is stored in eye space. Also since we're talking about a directional light, the position field is actually direction." */
	// get light direction
	L = normalize(vec3(gl_LightSource[0].position));

	/* "This is called swizzling. You can use x, y, z, or w, referring to the first, second, third, and fourth components, respectively. The reason it has that name "swizzling" is because the following syntax is entirely valid:" */
	// get eye vector
	E = normalize(gl_Position.xyz);

	// get half vector
	H = normalize(gl_LightSource[0].halfVector.xyz);

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
