
Texture2D texDiffuse : register(t0);
Texture2D normalMap : register(t1);

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
    float4 Pos : SV_Position;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
	float2 TexCoord : TEX;
    float4 WorldPos : WORLD_POSITION;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

// Only use this for rendering normal mapped materials
float3 GetNormal(float3 tangent, float3 binormal, float3 normal, float2 texCoord)
{
    float3 encodedNormal = normalMap.Sample(texSampler, texCoord).xyz;
    float3 localNormal = normalize(float3(encodedNormal.x * 2 - 1, encodedNormal.y * 2 - 1, encodedNormal.z * 2 - 1));
    float3x3 tbnMatrix = transpose(float3x3(tangent, binormal, normal));
    
    return normalize(mul(tbnMatrix, localNormal));
}

float4 PS_main(PSIn input) : SV_Target
{
    bool toggleAmbient = true;
    bool toggleDiffuse = true;
    bool toggleSpecular = true;
    
    float3 ambientColour = ambient.xyz;
    float3 diffuseColour = diffuse.xyz;
    float3 specularColour = specular.xyz;
    float shininess = specular.w;
    
    bool normalMap = diffuse.w;
    //bool skyBox = ambient.w;
    float ambientStrength = 0.3f;
    
    float4 colour = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    diffuseColour = texDiffuse.Sample(texSampler, input.TexCoord).xyz;
	
    float3 normal = normalMap ? GetNormal(input.Tangent, input.Binormal, input.Normal, input.TexCoord) : normalize(input.Normal);
    float3 lightVector = lightPosition.xyz - input.WorldPos.xyz;
    float3 lightNormal = normalize(lightVector);
    float3 cameraVector = cameraPosition.xyz - input.WorldPos.xyz;
    float3 cameraNormal = normalize(cameraVector);
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
    
    if (toggleAmbient)
        colour.xyz += ambientColour * diffuseColour * ambientStrength;
    if (toggleDiffuse)
        colour.xyz += diffuseColour * lightStrength;
    if (toggleSpecular)
        colour.xyz += specularColour * specularStrength; // * cube map color
    
    //return float4(input.Tangent * 0.5f + 0.5f, 1);
    //reutrn float4(input.Binormal * 0.5f + 0.5f, 1);
    //return float4((ambientColour + (diffuse.xyz * diff)) * diffuseColour.xyz + specularColour * spec, 1.0f);
    
    return colour;
}