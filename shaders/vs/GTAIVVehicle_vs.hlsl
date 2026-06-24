// GTA IV Vehicle Vertex Shader
// Adapted from RAGE vehicle_common.fxh (GTA V source)

struct VS_INPUT {
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float2 texcoord1 : TEXCOORD1;
    float4 color : COLOR0;
};

struct VS_OUTPUT {
    float4 pos : POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
    float3 worldNormal : TEXCOORD1;
    float3 viewDir : TEXCOORD2;
    float2 envTexcoord : TEXCOORD3;
    float fresnel : TEXCOORD4;
};

float4x4 worldViewProj : register(c0);
float4 ambientColor : register(c4);
float4 directCol[7] : register(c5);
float4 directDir[7] : register(c12);
float4 matCol : register(c19);
float4 surfProps : register(c20);
float4 fxParams : register(c30);
float4 envXform : register(c31);
float4x3 envMat : register(c32);

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(input.pos, worldViewProj);
    output.texcoord = input.texcoord;
    output.worldNormal = normalize(input.normal);
    output.viewDir = normalize(-input.pos.xyz);

    float3 envNormal = mul(input.normal, (float3x3)envMat);
    output.envTexcoord.x = envNormal.x * envXform.z + envXform.x + 0.5;
    output.envTexcoord.y = envNormal.y * envXform.w + envXform.y + 0.5;

    float NdotV = saturate(dot(output.worldNormal, output.viewDir));
    output.fresnel = pow(1.0 - NdotV, fxParams.y * 2.0);

    float3 litColor = ambientColor.rgb * surfProps.x;
    float3 N = normalize(input.normal);
    for(int i = 0; i < 7; i++){
        float NdotL = saturate(dot(N, directDir[i].xyz));
        litColor += directCol[i].rgb * NdotL;
    }
    litColor *= matCol.rgb * surfProps.z;
    output.color = float4(litColor, matCol.a);
    return output;
}

technique GTAIVVehicle {
    pass P0 {
        VertexShader = compile vs_3_0 vs_main();
    }
}
