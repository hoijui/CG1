// variables that are interpolated to the fragment shader
varying vec3 N; // nomal
varying vec3 V; // vertex

void main() {
	/* "The vertex shader has access to the normals, as specified in the OpenGL application, through the attribute variable gl_Normal. This is the normal as defined in the OpenGL application with the glNormal function, hence in model local space. If no rotations or scales are performed on the model in the OpenGL application, then the normal defined in world space, provided to the vertex shader as gl_Normal, coincides with the normal defined in the local space. The normal is a direction and therefore it is not affected by translations." */
	/*  The "normal matrix" is an inverse*transpose of the model*view matrix, which means that it moves our normals into eye space, but also avoids warping them when scaling in the model matrix is non-uniform. If we never scale in our mesh then we can just use the model*view matrix instead." */
	// get normal, transform into eye space
	N = normalize(gl_NormalMatrix * gl_Normal);

	// get vertex position in eye space
	V = vec3(gl_ModelViewMatrix * gl_Vertex);

	// setup correct position
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
