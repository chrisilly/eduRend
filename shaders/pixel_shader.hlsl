
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
    float shininess;
    float3 padding;
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
    
    // float4 textureColor = texDiffuse.Sample(texSampler, input.TextCoord);
	
    float3 N = normalize(input.Normal);
    float3 L = normalize(lightPosition.xyz - input.WorldPos.xyz);       // should input.WorldPos be a float3?? Reference other people's code
    float3 V = normalize(cameraPosition.xyz - input.WorldPos.xyz);      // should input.WorldPos be a float3?? Reference other people's code
    float3 R = reflect(-L, N);
    
    float3 ambientTerm = ambient.xyz; // * terxtureColor
    float diff = max(dot(L, N), 0.0f);
    float3 diffuseTerm = diffuse.xyz * diff;
    float spec = pow(max(dot(R, V), 0.0f), shininess); // 0.05f = shininess
    float3 specularTerm = specular.xyz * spec;
    
    //float3 color = ambientTerm + diffuseTerm;
    float3 color = ambientTerm + diffuseTerm + specularTerm;
    return float4(color, 1.0f);
}