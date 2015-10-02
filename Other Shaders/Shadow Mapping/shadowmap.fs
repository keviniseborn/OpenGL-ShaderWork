#version 430

uniform struct MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shininess;
} Material;

uniform sampler2DShadow ShadowMap;
uniform  vec3 LightIntensity;

in vec3 Position;
in vec3 Normal;
in vec4 ShadowCoord;
in vec3 LightPos;


layout (location = 0) out vec4 FragColor;

vec3 phongModelDiffAndSpec()
{
    vec3 n = Normal;
   if( !gl_FrontFacing ) n = -n;
    vec3 s = normalize(vec3(LightPos) - Position);
    vec3 v = normalize(-Position.xyz);
    vec3 r = reflect( -s, n );
    float sDotN = max( dot(s,n), 0.0 );
    vec3 diffuse = LightIntensity * Material.Kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = LightIntensity * Material.Ks *
            pow( max( dot(r,v), 0.0 ), Material.Shininess );

    return diffuse + spec;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{
   
   //////////////////////////////////////////////////////////////////////////////////
   // TO DO
   //
   // Calculate the ambient light using LightIntensity and the material's Ka value
   vec3 Ia = LightIntensity * Material.Ka;
   // Calculate the diffuse and specular lighting using phongModelDiffAndSpec() function
   vec3 IdIs = phongModelDiffAndSpec();
   // Find a value for shadow (float {0,1}) by sampling ShadowMap using the function textureProj
   float shadowSum = 0;

   shadowSum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(0,0));
   shadowSum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(0,1));
   shadowSum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1,0));
   shadowSum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1,1));

   shadowSum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1,-1));
   shadowSum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1,1));
   shadowSum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1,-1));
   shadowSum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(1,1));

   shadowSum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(0,-1));
   shadowSum += textureProjOffset(ShadowMap, ShadowCoord, ivec2(-1,0));
   
   float shadow = shadowSum * 0.1;

   //shadowSum = textureOffset(ShadowMap, ShadowCoord, ivec2(0,0));
   // Set the FragColor incorporating ambient, diffuseAndSpecular and shadow
   FragColor = vec4(Ia, 1.0) + vec4(shadow * phongModelDiffAndSpec(), 1.0);// + vec4(phongModelDiffAndSpec(), 1.0);
   // END TO DO
    //////////////////////////////////////////////////////////////////////////////////
}



subroutine (RenderPassType)
void recordDepth()
{
    // Outputs gl_FragCoord.z but no code needed as depth is written automatically by Frag Shaders

}

void main() {
    // This will call either shadeWithShadow or recordDepth
    RenderPass();
}
