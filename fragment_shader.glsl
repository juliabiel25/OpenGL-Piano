#version 330



uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform vec3 lightPosition;

out vec4 pixelColor;			

in vec4 n;
in vec4 l;
in vec4 v;

in vec4 n2;
in vec4 l2;
in vec4 v2;

in vec4 n3;
in vec4 l3;
in vec4 v3;

in vec2 iTexCoord0;

void main(void) {

	// interpolized vectors
	vec4 ml = normalize(l);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
		
	vec4 ml2 = normalize(l2);
	vec4 mn2 = normalize(n2);
	vec4 mv2 = normalize(v2);

	// ambient lighting
	vec3 ambientLight = vec3(0.4f, 0.4f, 0.4f);		

	// assign textures
	vec4 kd = texture(texture_diffuse, iTexCoord0);
	vec4 ks = texture(texture_specular, iTexCoord0);



	//===============
	// LIGHT SOURCE 1
	//===============

	// reflection vector
	vec4 mr = reflect(-ml, mn);

	// calculate diffuse lighting
	vec3 diffuseColor = vec3(1.f, 1.f, 1.f);
	float nl = clamp(dot(mn, ml), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1), 50);
	
	// finalize
	vec4 ls1 = vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb * rv, 0);



	//===============
	// LIGHT SOURCE 2
	//===============

	// reflection vector
	vec4 mr2 = reflect(-ml2, mn2);

	// calculate diffuse lighting
	vec3 diffuseColor2 = vec3(1.f, 1.f, 1.f);
	float nl2 = clamp(dot(mn2, ml2), 0, 1);
	float rv2 = pow(clamp(dot(mr2, mv2), 0, 1), 10);
	
	// finalize
	vec4 ls2 = vec4(kd.rgb * nl2, kd.a) + vec4(ks.rgb * rv2, 0);



	



	// ==============
	//  JOIN LIGHTS
	// ==============

	pixelColor = ls1 + ls2 + kd * vec4(ambientLight, 1.f) ;

	
}
