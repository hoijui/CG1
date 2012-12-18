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

varying vec3 N; // nomal
varying vec3 L; // light direction
varying vec3 H; // half vector

void main()	{
	vec3 myN;
	vec3 myH;
	vec4 color;
	float NdotL;
	float NdotH;

	/* "A fragment shader can't write a varying variable, hence we need a new variable to store the normalized interpolated normal." */
	myN = normalize(N);
	myH = normalize(H);

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
		NdotH = dot(myN,myH);
		if (NdotH > 0.0) {
			// rs * L * (H.N)^n
			color += gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotH, gl_FrontMaterial.shininess);
		}
	}

	// set current pixel color
	gl_FragColor = color;
}
