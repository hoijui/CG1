/// @file
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Copyright (C) 2011      Computer Graphics, TU Berlin
///
////////////////////////////////////////////////////////////////////////////////////////////////////
///
///  module     : blinnphong.frag
///
///  project    : Computer Graphics 1
///
///  description: Simple fragment shader
///
////////////////////////////////////////////////////////////////////////////////////////////////////

varying vec3 N; // surface nomal in eye coords
varying vec3 V; // vertex in eye coords

void main()	{
	vec3 myN;
	vec3 E; // eye vector
	vec3 L; // light direction
	vec3 H; // half vector
	vec4 color;
	float NdotL;
	float NdotH;

	// make sure normal keeps normal
	myN = normalize(N);

	/* "Note that according to the OpenGL specification, the light is stored in eye space. Also since we're talking about a directional light, the position field is actually direction." */
	// get light direction
	L = normalize(vec3(gl_LightSource[0].position) - V);

	// get eye vector
	E = normalize(-V); // use -V because V is in eye space (0,0,0); we need direction from surface to eye

	// get half vector
	H = normalize(E + L);

	// always add ambient light
	color = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	color += gl_LightModel.ambient * gl_FrontMaterial.ambient;

	// check light direction ...
	NdotL = dot(myN,L);
	// ... and add diffuse/specular light only if reflection is possible
	if (NdotL > 0.0) {
		// add diffuse light
		color += gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse * NdotL; // warum hier nochmal *NdotL ???
		// add specular light
		NdotH = dot(myN,H);
		if (NdotH > 0.0) {
			// rs * L * (H.N)^n
			color += gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotH, gl_FrontMaterial.shininess);
		}
	}

	// set current pixel color
	gl_FragColor = color;
}
