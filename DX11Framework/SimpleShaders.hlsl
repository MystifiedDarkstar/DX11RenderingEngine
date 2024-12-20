struct DirectionalLight
{
    float4 AmbientLight; // 16
    float4 DiffuseLight; // 16
    float4 SpecularLight; // 16
    float3 LightDir; // 12
    float SpecPower; // 4
};
 
struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float3 Attenuation;
    float Range;
    float3 Position;
    float Padding;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 Projection;
    float4x4 View;
    float4x4 World;
    DirectionalLight DLights[4];
    PointLight PLights[4];
    float3 CameraPosition;
    int DLightCount;
    int PLightCount;
}

Texture2D diffuseTex : register(t0);
Texture2D specularTex : register(t1);

SamplerState bilinearSampler : register(s0);

struct VS_Out
{
    float4 position : SV_POSITION;
    float4 NormalW : NORMAL;
    float3 PosW : POSITION0;
    float2 TexCoord : TEXCOORD;
};

VS_Out VS_main(float3 Position : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD)
{   
    VS_Out output = (VS_Out)0;
    float4 Pos4 = float4(Position, 1.0f);
    
    float4 Normal4 = float4(Normal, 0.0f);
    float4 NormalW = mul(Normal4, World);
    
    output.NormalW = NormalW;
    output.PosW = mul(Pos4, World);
    output.position = mul(Pos4, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    output.TexCoord = TexCoord;
        
    return output;
}

float4 PS_main(VS_Out input) : SV_TARGET
{
    float4 TexColor = diffuseTex.Sample(bilinearSampler, input.TexCoord);
    
    float4 NormalizedDir = normalize(input.NormalW);
    
    float4 PotentialDiffuse = { 0, 0, 0, 0 };
    float4 TotalColour = { 0, 0, 0, 0 };
    
    for (int j = 0; j < PLightCount; j++)
    {
        float4 FinalColour = { 0, 0, 0, 1.0 };
        
        float3 LightToPixelVec = PLights[j].Position - input.PosW.xyz;
        float Distance = length(LightToPixelVec);
        
        float3 finalAmbient = TexColor * PLights[j].Ambient; // Calculate Ambient
        
        //If pixel is too far, return pixel color with ambient light
        if (Distance > PLights[j].Range)
            return float4(finalAmbient, TexColor.a);
            
        //Turn lightToPixelVec into a unit length vector describing
        //the pixels direction from the lights position
        LightToPixelVec /= Distance;
    
        //Calculate how much light the pixel gets by the angle
        //in which the light strikes the pixels surface
        float howMuchLight = saturate(dot(LightToPixelVec, input.NormalW.xyz));

        //If light is striking the front side of the pixel
        if (howMuchLight > 0.0f)
        {
        //Add light to the finalColor of the pixel
            FinalColour += howMuchLight * TexColor * PLights[j].Diffuse;
        
        //Calculate Light's Falloff factor
            FinalColour /= PLights[j].Attenuation[0] + (PLights[j].Attenuation[1] * Distance) + (PLights[j].Attenuation[2] * (Distance * Distance));
            
        }
        //FinalColour += float4(finalAmbient,1.0f);
        TotalColour = saturate(FinalColour + TotalColour);

    }
    
    for (int i = 0; i < DLightCount; i++)
    {
        float3 ReflectedDir = -DLights[i].LightDir;
        float DiffuseAmount = saturate(dot(float4(ReflectedDir, 0.0f), NormalizedDir));
        
        PotentialDiffuse = TexColor * DLights[i].DiffuseLight; // Calculate Diffuse
        TotalColour += TexColor * DLights[i].AmbientLight; // Calculate Ambient
        TotalColour += PotentialDiffuse * DiffuseAmount;
        
        float3 ToViewer = normalize(CameraPosition - input.PosW);
        float3 ReflectDir = normalize(reflect(DLights[i].LightDir, NormalizedDir.xyz));
        float specAmount = pow(saturate(dot(ReflectDir, ToViewer)), DLights[i].SpecPower);
        float4 Specular = { 0, 0, 0, 0 };

        float4 SpecularMap = specularTex.Sample(bilinearSampler, input.TexCoord);
        Specular = specAmount * (DLights[i].SpecularLight * SpecularMap);
        TotalColour += Specular;

    }
    
    
    
    return TotalColour;

}