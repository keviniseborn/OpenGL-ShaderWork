#version 430
#extension GL_NV_shadow_samplers_cube : enable

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 vertPos; //Vertex position in eye coords
out vec3 N; //Transformed normal
out vec3 lightPos; //Light position in eye coords

uniform vec3 LightPosition; // Light position 
uniform mat3 NormalMatrix;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 pos_eye; //Vertex position in eye coords
out vec3 n_eye;

void main()
{

   vertPos = vec3(V * M * vec4(VertexPosition,1.0)); 
     
   lightPos = vec3(V * M * vec4(LightPosition,1.0));  

   N = NormalMatrix * VertexNormal;
   N = normalize(N);

   pos_eye = vec3(V*M*vec4(VertexPosition,1.0));
   n_eye = vec3(V*M*vec4(VertexNormal,0.0));
  
   N = normalize(N);
   gl_Position = P * V * M *vec4(VertexPosition,1.0);

}
