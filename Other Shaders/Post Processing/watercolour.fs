#version 430

in vec3 Position;
in vec3 N;
in vec2 TexCoord;

uniform sampler2D RenderTex;
uniform sampler2D planeTex;

uniform float EdgeThreshold;
uniform int Width;
uniform int Height;

subroutine vec4 RenderPassType();
subroutine uniform RenderPassType RenderPass;

struct LightInfo {
  vec4 Position;  // Light position in eye coords.
  vec3 Intensity; // A,D,S intensity
};
uniform LightInfo Light;

struct MaterialInfo {
  vec3 Ka;            // Ambient reflectivity
  vec3 Kd;            // Diffuse reflectivity
  vec3 Ks;            // Specular reflectivity
  float Shininess;    // Specular shininess factor
};
uniform MaterialInfo Material;

layout( location = 0 ) out vec4 FragColor;

vec3 phongModel( vec3 pos, vec3 norm )
{
    //Calculate the light vector
   vec3 L = normalize(vec3(Light.Position) - pos); 
 
   //Calculate View vector
   vec3 V = normalize(-pos); //We are in eye coords //so camera is at (0,0,0); 
    

   //calculate Diffuse Light Intensity making sure it is not negative 
   //and is clamped 0 to 1  
     
   vec3 Id = Light.Intensity * max(dot(norm,L), 0.0);
   Id = clamp(Id, 0.0, 1.0);     
   
   vec3 Ia = Light.Intensity * Material.Ka;

    return Ia + Id;
}

subroutine (RenderPassType)
vec4 pass1()
{
   return vec4(phongModel( Position, N ),1.0);
}

float luminance( vec3 color ) {
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

subroutine( RenderPassType )
vec4 pass2()
{
    const int bins = 20;
    int currentIntensity = 0;
    int radius = 5;
    int oilIntensity = 20;
    int intensityCount[bins];
    int intensityCounter = 0;
    vec3 averageColour[bins];
    int currentMax = 0;
    int maxIndex = 0;
    vec3 colour;
    
    for(int i=0; i < bins; i++)
    {
        intensityCount[i] = 0;
        averageColour[i] = vec3(0.0f, 0.0f, 0.0f);
    }

    for(int i= - radius; i <= 0; ++i)
    {
        for(int k = -radius; k <= 0; ++k)
        {
            colour = vec3(textureOffset(RenderTex, TexCoord, ivec2(i,k)));
            currentIntensity = int(((colour.r+colour.g+colour.b)/3.0)*(bins-1));
            intensityCount[currentIntensity]++;
            averageColour[currentIntensity] +=  vec3(textureOffset(RenderTex, TexCoord, ivec2(i,k)));

        }

    }

    for(int i= 0; i <= radius; ++i)
    {
        for(int k = -radius; k <= 0; ++k)
        {
            colour = vec3(textureOffset(RenderTex, TexCoord, ivec2(i,k)));
            currentIntensity = int(((colour.r+colour.g+colour.b)/3.0)*(bins-1));
            intensityCount[currentIntensity]++;
            averageColour[currentIntensity] +=  vec3(textureOffset(RenderTex, TexCoord, ivec2(i,k)));

        }

    }

    for(int i= - radius; i <= 0; ++i)
    {
        for(int k = 0; k <= radius; ++k)
        {
            colour = vec3(textureOffset(RenderTex, TexCoord, ivec2(i,k)));
            currentIntensity = int(((colour.r+colour.g+colour.b)/3.0)*(bins-1));
            intensityCount[currentIntensity]++;
            averageColour[currentIntensity] +=  vec3(textureOffset(RenderTex, TexCoord, ivec2(i,k)));

        }

    }

    for(int i= 0; i <= radius; ++i)
    {
        for(int k = 0; k <= radius; ++k)
        {
            colour = vec3(textureOffset(RenderTex, TexCoord, ivec2(i,k)));
            currentIntensity = int(((colour.r+colour.g+colour.b)/3.0)*(bins-1));
            intensityCount[currentIntensity]++;
            averageColour[currentIntensity] +=  vec3(textureOffset(RenderTex, TexCoord, ivec2(i,k)));

        }

    }

    for(int i=0; i < bins; i++)
    {
        if(intensityCount[i] > currentMax)
        {
            currentMax = intensityCount[i];
            maxIndex = i;

        }

    }
  
    return vec4(averageColour[maxIndex] / currentMax,1.0);

}

void main()
{
    // This will call either pass1() or pass2()
    FragColor = RenderPass();
}