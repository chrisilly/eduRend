
Texture2D texDiffuse : register(t0);

SamplerState texSampler : register(s0);

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
    float3 ambientColour = ambient.xyz;
    float3 diffuseColour = diffuse.xyz;
    float3 specularColour = specular.xyz;
    float shininess = specular.w;
    
    bool normalMap = diffuse.w;
    //bool skyBox = ambient.w;
    float ambientStrength = 0.3f;
    
    float4 colour = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    diffuseColour = texDiffuse.Sample(texSampler, input.TexCoord).xyz;
	
    float3 normal = normalize(input.Normal);
    float3 lightVector = lightPosition.xyz - input.WorldPos.xyz;       // should input.WorldPos be a float3?? Reference other people's code
    float3 lightNormal = normalize(lightVector);
    float3 cameraVector = cameraPosition.xyz - input.WorldPos.xyz;
    float3 cameraNormal = normalize(cameraVector);      // should input.WorldPos be a float3?? Reference other people's code
    float3 reflectionVector = reflect(-lightNormal, normal);
    //reflectionVector = reflect(lightvector, normal);
    //float3 reflectionNormal = normalize(reflectionVector);
    float reflectionAngle = max(dot(reflectionVector, cameraNormal), 0.0f);
    //reflectionAngle = max(0, -dot(reflectionNormal, cameraNormal));

    float lightStrength = max(dot(lightNormal, normal), 0.0f);
    float specularStrength = pow(reflectionAngle, shininess);
    
    // Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
	//return float4(input.Normal*0.5+0.5, 1);
	
	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
    // return float4(input.TexCoord, 0, 1);
    
    colour.xyz += ambientColour * diffuseColour * ambientStrength;
    colour.xyz += diffuseColour * lightStrength;
    colour.xyz += specularColour * specularStrength; // * cube map color
    //colour = float4((ambientColour + (diffuse.xyz * diff)) * diffuseColour.xyz + specularColour * spec, 1.0f);
    
    return colour;
}