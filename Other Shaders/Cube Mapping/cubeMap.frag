#version 430
#extension GL_NV_shadow_samplers_cube : enable

in vec3 vertPos;
in vec3 N;
in vec3 lightPos;
in vec3 texcoords;
uniform samplerCube cube_texture;

uniform vec3 Ka;			// Ambient reflectivity
uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ld;            // Diffuse light intensity

in vec3 pos_eye;
in vec3 n_eye;
uniform mat4 V;


layout( location = 0 ) out vec4 FragColour;

void main() {

	//light calculations
	vec3 L = normalize(lightPos - vertPos);  

	//calculate diffuse light intensity
	vec3 Id = Ld * max(dot(N,L), 0.0);
	Id = clamp(Id, 0.0, 1.0); 
	vec4 diffuse = vec4(Id,1.0) * vec4(Kd,1.0);

	//calculate Ambient Light intensity
	vec3 Ia = 0.5 * Ka;
	Ia = clamp(Ia, 0.0, 1.0); 

	//cube map calculations
	vec3 incident_eye = normalize(pos_eye);
	vec3 normal = normalize(n_eye);
	
	//reflection calculation
	vec3 reflected =incident_eye - (2 * normal * dot(incident_eye,normal));

	//change to world space
	reflected = vec3(inverse(V) * vec4(reflected,0.0));

    FragColour = textureCube(cube_texture, reflected); //+ diffuse + vec4(Ia,1.0);
}
