// GTA IV Building Vertex Shader
// Adapted from RAGE megashader.fxh (GTA V source)

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
    float2 envTexcoord : TEXCOORD2;
};

float4x4 worldViewProj : register(c0);
float4 ambientColor : register(c4);
float4 directCol[7] : register(c5);
float4 directDir[7] : register(c12);
float4 matCol : register(c19);
float4 surfProps : register(c20);
float colorScale : register(c29);
float4 dayParam : register(c30);
float4 nightParam : register(c31);
float4x4 texMat : register(c32);
float4 fxParams : register(c36);
float4 envXform : register(c37);
float4x3 envMat : register(c38);

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(input.pos, worldViewProj);
    output.texcoord = mul(float4(input.texcoord, 0, 1), texMat).xy;
    output.worldNormal = normalize(input.normal);

    float3 envNormal = mul(input.normal, (float3x3)envMat);
    output.envTexcoord.x = envNormal.x * envXform.z + envXform.x + 0.5;
    output.envTexcoord.y = envNormal.y * envXform.w + envXform.y + 0.5;

    float3 N = normalize(input.normal);
    float3 litColor = ambientColor.rgb * dayParam.rgb * surfProps.x;
    for(int i = 0; i < 7; i++){
        float NdotL = saturate(dot(N, directDir[i].xyz));
        litColor += directCol[i].rgb * NdotL * dayParam.rgb;
    }
    litColor *= matCol.rgb * colorScale;
    output.color = float4(litColor, 1.0);
    return output;
}

technique GTAIVBuilding {
    pass P0 {
        VertexShader = compile vs_3_0 vs_main();
    }
}
