// GTA IV Vehicle Pixel Shader
// Adapted from RAGE vehicle_common.fxh

struct VS_OUTPUT {
    float4 pos : POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
    float3 worldNormal : TEXCOORD1;
    float3 viewDir : TEXCOORD2;
    float2 envTexcoord : TEXCOORD3;
    float fresnel : TEXCOORD4;
};

sampler2D diffuseSampler : register(s0);
sampler2D envSampler : register(s1);

float4 fxParams : register(c0);  // {fxSwitch, shininess, specularity, lightmult}
float4 psParams : register(c1);  // {colorScale, unused, unused, unused}

float4 main(VS_OUTPUT input) : COLOR0
{
    float4 diffuse = tex2D(diffuseSampler, input.texcoord);
    float4 envColor = tex2D(envSampler, input.envTexcoord);

    float specularity = fxParams.z;
    float3 color = input.color.rgb * diffuse.rgb;
    color += envColor.rgb * input.fresnel * specularity * fxParams.w;

    color *= psParams.x;

    return float4(color, diffuse.a * input.color.a);
}

technique GTAIVVehicle {
    pass P0 {
        PixelShader = compile ps_3_0 main();
    }
}
