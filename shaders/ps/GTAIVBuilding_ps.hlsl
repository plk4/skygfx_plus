// GTA IV Building Pixel Shader
// Adapted from RAGE megashader.fxh

struct VS_OUTPUT {
    float4 pos : POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
    float3 worldNormal : TEXCOORD1;
    float2 envTexcoord : TEXCOORD2;
};

sampler2D diffuseSampler : register(s0);
sampler2D envSampler : register(s1);

float4 psParams : register(c0); // {colorScale, detailTile, wetAmount, unused}

float4 main(VS_OUTPUT input) : COLOR0
{
    float4 diffuse = tex2D(diffuseSampler, input.texcoord);

    float3 color = input.color.rgb * diffuse.rgb;

    float wetness = psParams.z;
    if(wetness > 0.01){
        float4 envColor = tex2D(envSampler, input.envTexcoord);
        float NdotV = saturate(dot(input.worldNormal, float3(0,0,1)));
        float fresnel = pow(1.0 - NdotV, 3.0);
        color = lerp(color, envColor.rgb * 0.5, fresnel * wetness * 0.3);
    }

    return float4(color, diffuse.a);
}

technique GTAIVBuilding {
    pass P0 {
        PixelShader = compile ps_3_0 main();
    }
}
