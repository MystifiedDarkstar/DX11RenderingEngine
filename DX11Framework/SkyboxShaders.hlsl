TextureCube skybox : register(t0);
SamplerState bilinearSampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
    float4x4 Projection;
    float4x4 View;
    float4x4 World;
    float4 DiffuseLight;
    float4 DiffuseMaterial;
    float4 AmbientLight;
    float4 AmbientMaterial;
    float4 SpecularLight;
    float4 SpecularMaterial;
    float3 CameraPosition;
    float SpecPower;
    float3 LightDir;
    uint hasTexture;
    uint hasSpecularMap;
}

struct SkyboxVS_OUT
{
    float4 position : SV_Position;
    float3 texCoord : TEXCOORD;
};

SkyboxVS_OUT VS_main(float3 Position : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD)
{
    SkyboxVS_OUT output = (SkyboxVS_OUT) 0;
    float4 Pos4 = float4(Position, 1.0f);
    
    output.position = mul(Pos4, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    output.position = output.position.xyww;
    output.texCoord = Position;
        
    return output;
}

float4 PS_main(SkyboxVS_OUT input) : SV_TARGET
{
    float4 TexColor = skybox.Sample(bilinearSampler, input.texCoord);
    return TexColor;
}