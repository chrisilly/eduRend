
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
	
    float3 modelToLight = (lightPosition.xyz - input.WorldPos.xyz);
    // If the dot product is negative, it means that point is supposed to be black (without light), hence why we use max()
    float4 lambertDiffuse = max(diffuse * dot(modelToLight.xyz, input.Normal), 0);
    
    float4 reflectedRay = float4(reflect(-modelToLight, input.WorldNormal), 0);
    float4 modelToCamera = cameraPosition - input.WorldPos;
    
    float4 specularHighlight = max(specular * pow(abs(dot(reflectedRay.xyz, modelToCamera.xyz)), specular.w), 0);
    
    return ambient + lambertDiffuse + specularHighlight;
}