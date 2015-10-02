#version 430

// The Light information
struct LightInfo {
  vec4 Position;  // Light position in eye coords.
  vec3 AmbientIntensity; 
  vec3 DiffuseIntensity;
  vec3 SpecularIntensity;
};
uniform LightInfo Light;

// The Material information
struct MaterialInfo {
  vec3 Kd;            // Ambient and Diffuse reflectivity
  vec4 Ks;			 // Specular Reflectivity
};
uniform MaterialInfo Material;

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

// The textures produced by the pass1
uniform sampler2D PositionTex, NormalTex, ColorTex, SpecularTex; 

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;


layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 PositionData; //COLOR_ATTACHMENT0
layout (location = 2) out vec3 NormalData; //COLOR_ATTACHMENT1
layout (location = 3) out vec3 ColorData; //COLOR_ATTACHMENT2
layout (location = 4) out vec4 SpecularData; //COLOR_ATTACHMENT3


//Calculate the fragment colour under ambient + diffuse light
//using position, normal and diffuse colour information sampled from
//textures

vec3 ambientModel()
{
	vec3 ambient;
	ambient = vec3(0.0,0.0,0.0);
	return ambient;
}

vec3 diffuseModel( vec3 pos, vec3 norm, vec3 diff )
{
    vec3 s = normalize(vec3(Light.Position) - pos);
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = clamp(Light.AmbientIntensity * diff + Light.DiffuseIntensity * diff * sDotN,0,1);

    return diffuse;
}

vec3 specularModel(vec3 pos, vec3 norm, vec4 spec)
{


	// reflect(L,N) = R
	// normalize(-vertPos) = V

	// vec3 S = Ls * Ks * (pow(dot(R,V),nc));
	float specPow = spec.a * 255;
	vec3 L = normalize(Light.Position.xyz - pos); 
	vec3 N = normalize(norm);
	vec3 R = -reflect(L,N);
	vec3 V = normalize(-pos);
	float RV = clamp(dot(R,V),0.0,1.0);
	

	vec3 Is = vec3(0,0,0);
	
	if (dot(V,N) > 0.0)
	{
	 Is =  spec.rgb * pow(RV,6);
	}

  return Is;
}

//pass1
subroutine (RenderPassType)
void pass1()
{
    // Store position, normal, and diffuse color in textures
    PositionData = Position;
    NormalData = Normal;
    ColorData = vec3(Material.Kd);
	float specPow = Material.Ks.a / 255;
	SpecularData = vec4(Material.Ks.rgb, specPow);

}

//pass2
subroutine(RenderPassType)
void pass2()
{
	vec4 specColor = vec4(texture(SpecularTex, TexCoord) );

	vec3 diffColor = vec3(texture(ColorTex, TexCoord) );
	vec3 Norm = vec3(texture(NormalTex, TexCoord) );
	vec3 Pos = vec3(texture(PositionTex,TexCoord) );

	//FragColor =vec4(diffColor, 1.0);

	//FragColor = vec4(diffuseModel(Pos, Norm, diffColor), 1.0);


	//FragColor = vec4(specularModel(Pos, Norm, specColor), 1.0);
	FragColor = vec4(diffuseModel(Pos, Norm, diffColor), 1.0) + vec4(specularModel(Pos, Norm, specColor), 1.0);
}
void main() {
    // This will call either pass1 or pass2
    RenderPass();
}
