#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 TextureCoords;
layout (location = 3) in vec3 Tangent;

out vec3 vertPos; //Vertex position in eye coords
out vec3 N; //Transformed normal
out vec3 lightPos; //Light position in eye coords

out vec3 lightDirection;
out vec3 targetDirection;
out vec3 pos_eye; //Vertex position in eye coords

out vec3 TangentSurface2Light;
out vec3 TangentSurface2View;

uniform vec3 LightPosition; // Light position 
uniform mat3 NormalMatrix;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 CamPos;
uniform vec3 SpotPosition;

out vec2 uvCoord;
out vec3 Tangent0;

void main()
{
   vertPos = vec3(V * M * vec4(VertexPosition,1.0)); 
     
   lightPos = vec3(V * M * vec4(LightPosition,1.0));  

   N = normalize( NormalMatrix * VertexNormal);

   lightDirection = normalize(SpotPosition - VertexPosition);
   targetDirection = normalize(targetDirection - SpotPosition);
   pos_eye = vec3(V*M*vec4(VertexPosition,1.0));


   uvCoord = TextureCoords;

	//normal, tangent and bitangent calculations
	vec3 n = normalize(NormalMatrix * VertexNormal);
	vec3 t = normalize(NormalMatrix * Tangent);
	vec3 b = cross(n,t);

	//matrix for tangent
	mat3 mat = mat3(t.x, b.x, n.x, t.y, b.y, n.y, t.z, b.z, n.z);

	//convert to tangent space

	vec3 light = LightPosition;

	vec3 vector = normalize(light - vertPos);
	TangentSurface2Light = mat * vector;

	vector = normalize(pos_eye - vertPos);
	TangentSurface2View = mat *  vector;

      
   gl_Position = P * V * M * vec4(VertexPosition,1.0);
}
