#version 430

layout(binding = 0) uniform sampler2D texDif;
layout(binding = 1) uniform sampler2D texNorm;
layout(binding = 2) uniform sampler2D texHeight;
layout(binding = 3) uniform sampler2D texSpec;
in vec2 uvCoord;

in vec3 vertPos;
in vec3 N;
in vec3 lightPos;

in vec3 lightDirection;
in vec3 targetDirection;
in vec3 pos_eye;

uniform vec3 Ka;			// Ambient reflectivity
uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ld;            // Diffuse light intensity
uniform vec3 Ls;			// Specular light intensity
uniform vec3 Ks;			// Specular reflection coefficient

uniform float parallaxScale;

in vec3 TangentSurface2Light;
in vec3 TangentSurface2View;

layout( location = 0 ) out vec4 FragColour;

vec3 normalMappingLighting(in vec2 T, in vec3 L)
{
   vec3 texColour = vec3(texture(texDif, T).rgb);		//sampled diffuse texture

   vec3 norm = 2.0 * texture(texNorm, T).xyz - 1.0;		//new normals from normal texture
   vec4 spec = vec4(texture(texSpec, T));				//new specular value from specular texture
   norm = normalize(norm);								//normalize the norm

   //calculate Ambient Light intensity

   vec3 Ia = 0.2 * texColour;
   Ia = clamp(Ia, 0.0, 1.0); 
    
   //calculate Diffuse Light Intensity making sure it is not negative 
  
   vec3 Id = Ld * max(dot(norm,L), 0.0);
   Id = Id * texColour;
   Id = clamp(Id, 0.0, 1.0);     


 
	//calculate specular light intensity
	float specPow = (spec.a * 255) * 10; 
	vec3 R = -reflect(L,norm);
	vec3 V = normalize(-vertPos);
	float RV = clamp(dot(R,V),0.0,1.0);
	

	vec3 Is = vec3(0,0,0);
	
	if (dot(V,norm) > 0.0)
	{
	 Is =  spec.rgb * pow(RV,15);
	}

   return  Ia + Id + Is;
}

vec2 parallaxOcclusionMapping(in vec3 V, in vec2 T)
{
	
	float numLayers = 12.5;													//set number of layers

	float layerHeight = 1.0 / numLayers;									// calculate height of each layer
	float currentLayerHeight = 0;											//current depth of layer

	vec2 texShift = parallaxScale * V.xy / V.z / numLayers;					//shift of tex coordinates at each layer

	vec2 currentTexCoords = T;												//current texture coordinates

	float textureHeight = texture(texHeight, currentTexCoords).r;			//depth from height map

	//while point is above the surface
	while(textureHeight > currentLayerHeight)
	{
		currentLayerHeight += layerHeight;							//to next layer
		currentTexCoords -= texShift;								//shift of tex coords
		textureHeight = texture(texHeight, currentTexCoords).r;		//new depth from height map
	}

	vec2 previousTexCoords = currentTexCoords + texShift;			//previous texture coordinates

	//heights for interpolation
	float nextH = textureHeight - currentLayerHeight;
	float prevH = texture(texHeight, previousTexCoords).r - currentLayerHeight + layerHeight;

	//proportions for linear interpolation
	float weight = nextH / (nextH - prevH);	
	
	//interpolation of tex coords						
	vec2 finalTexCoords = previousTexCoords * weight + currentTexCoords * (1.0-weight);
	finalTexCoords = finalTexCoords;
	
	//return new texture coordinates
	return finalTexCoords;
}


void main() {

	vec3 L = normalize(TangentSurface2Light); 
	vec3 eye = normalize(TangentSurface2View);
	
	//set a scale and bias for parallax mapping
	float scale, bias;
	scale = 0.06;
	bias = 0.02;

	//calculate height value from height texture at uv coords
	///float height = texture(texHeight, uvCoord);

	///use the scale and bias to get parallax mapping more like the surface properties
	//float h = height * scale - bias;

	//calculate new texture coordinates based on old coordinates, height and normalized surface to view vector.
	//vec2 T = uvCoord + (eye.xy * h);

	//use parallax occlusion mapping function to get new texture coordinates from POM
	vec2 T = parallaxOcclusionMapping(eye, uvCoord);

	//call the normal mapping lighting function with new texture coordinates to calculate lighting
	vec4 colour = vec4(normalMappingLighting(T, L), 1.0);
	
	FragColour = colour;
 
}
