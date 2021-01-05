#version 460

layout(location = 0) in flat uint material;
layout(location = 1) in flat uint drawID;
layout(location = 2) in vec4 f_position;
layout(location = 3) in vec4 f_color;
layout(location = 4) in vec4 f_normal;
layout(location = 5) in vec4 f_lightPosition; // in view space

layout(location = 0) out vec4 out_color;

#include "world.glslh"

void main() {
	switch (material) {

	case MATERIAL_FLAT:
		out_color = f_color;
		break;

	case MATERIAL_PHONG:
		float ambient = phongData.data[drawID].ambient;
		float diffuse = phongData.data[drawID].diffuse;
		float specular = phongData.data[drawID].specular;
		float shine = phongData.data[drawID].shine;

		vec4 normal = normalize(f_normal);
		vec4 lightDirection = normalize(f_lightPosition - f_position);
		vec4 viewDirection = normalize(-f_position);
		vec4 halfwayDirection = normalize(lightDirection + viewDirection);

		vec4 outAmbient = world.ambientColor * ambient;
		vec4 outDiffuse = world.lightColor * diffuse * max(dot(normal, lightDirection), 0.0);
		vec4 outSpecular = world.lightColor * specular * pow(max(dot(normal, halfwayDirection), 0.0), shine);
		vec4 lightStrength = outAmbient + outDiffuse + outSpecular;

		out_color = vec4(lightStrength.rgb, 1.0) * f_color;
//		out_color = vec4(mix(out_color.rgb, fHighlight.rgb, fHighlight.a), out_color.a);
		break;

	}
}
