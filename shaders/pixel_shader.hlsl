
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
    float3 PosWorld : POSITION_WORLD;
    float3 NormalWorld : NORMAL_WORLD;
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
	//return float4(input.TexCoord, 0, 1);
	
	//Debug ??? #3
    float3 light = lightPosition.xyz - input.PosWorld;
    float3 camera = normalize(cameraPosition.xyz - input.PosWorld);
    float3 reflectedRay = reflect(-light, camera);
    
    float diffuseIntensity = max(0, dot(input.NormalWorld, light));
    float specularIntensity = pow(max(0, dot(reflectedRay, camera)), specular.w);
    
    normalize(light);
    normalize(camera);
    normalize(reflectedRay);
    normalize(input.NormalWorld);

    //return float4(cameraPosition);
    return float4(ambient + diffuse * diffuseIntensity + specular * specularIntensity);
    //return float4(ambient + diffuse * diffuseIntensity + specular * specularIntensity);
    //return float4(ambient.xyz + diffuse.xyz * (lightPosition.xyz * -cameraNormal) + specular.xyz * pow(reflectedRay * cameraNormal, 5), 1);
    //return float4(ambient + diffuse * dot(light, input.NormalWorld) + specular * dot(reflectedRay, camera));
    //return float4(ambient + max(diffuse * dot(light, input.NormalWorld), 0) + max(specular * pow(dot(reflectedRay, camera), 5), 0));
    //return float4(diffuse);
}