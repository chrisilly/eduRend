
Texture2D texDiffuse : register(t0);

cbuffer LightCamBuffer : register(b0)
{
    float4 lightPosition;
    float4 cameraPosition;
};

cbuffer MaterialBuffer : register(b1)
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
};

struct PSIn
{
	float4 Pos  : SV_Position;
	float3 Normal : NORMAL;
	float2 TexCoord : TEX;
    float3 WorldNormal : WORLD_NORMAL;
    float4 WorldPos : WORLD_POSITION;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
	//return float4(input.Normal*0.5+0.5, 1);
	
	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
//	return float4(input.TexCoord, 0, 1);
	
    // Old calculations
    //float3 modelToLight = (lightPosition.xyz - input.WorldPos.xyz);
    //// If the dot product is negative, it means that point is supposed to be black (without light), hence why we use max()
    //float4 lambertDiffuse = max(diffuse * dot(modelToLight.xyz, input.Normal), 0);
    //
    //float4 reflectedRay = float4(reflect(-modelToLight, input.WorldNormal), 0);
    //float4 modelToCamera = cameraPosition - input.WorldPos;
    //
    //float4 specularHighlight = max(specular * pow(abs(dot(reflectedRay.xyz, modelToCamera.xyz)), specular.w), 0);
    //
    //return ambient + lambertDiffuse + specularHighlight;
    
    // new calculations
    float3 modelToLight = (lightPosition.xyz - input.WorldPos.xyz).xyz;
    // If the dot product is negative, it means that point is supposed to be black (without light), hence why we use max()
    float3 diffuseComponent = max(diffuse * dot(modelToLight, input.Normal), 0).xyz;
    
    float3 reflectedRay = reflect(-modelToLight, input.WorldNormal);
    float3 calculatedRelfectedRay = 2 * (modelToLight * input.WorldNormal) * input.WorldNormal - modelToLight;
    float3 modelToCamera = (cameraPosition - input.WorldPos).xyz;
    
    float3 specularComponent = max(specular * pow(max(dot(calculatedRelfectedRay, modelToCamera), 0), specular.w), 0).xyz;
    //float3 color = { 1, 1, 1 };
    
    return float4((ambient.xyz + diffuseComponent + specularComponent), 1);
}