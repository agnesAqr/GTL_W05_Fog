// MeshPixelShader.hlsl

struct FMaterial
{
    float3 DiffuseColor;
    float TransparencyScalar;
    float3 AmbientColor;
    float DensityScalar;
    float3 SpecularColor;
    float SpecularScalar;
    float3 EmissiveColor;
    float MaterialPad0;
};

cbuffer MaterialConstants : register(b0)
{
    FMaterial Material;
}

//cbuffer TextureConstants : register(b2)
//{
//    float2 UVOffset;
//    float2 Padding;
//};

Texture2D g_DiffuseMap : register(t0);
SamplerState g_sampler0 : register(s0);

struct PS_INPUT
{
    float4 Position : SV_POSITION; // 변환된 화면 좌표
    float4 Color : COLOR; // 버텍스 컬러
    float3 Normal : NORMAL; // 정규화된 노멀 벡터
    float2 TexCoord : TEXCOORD1;
    float4 WorldPosition : POSITION; // 버텍스 위치
    float SceneDepth : TEXCOORD2;
};

struct PS_OUTPUT
{
    float4 Normal : SV_Target0;
    float4 Albedo : SV_Target1; // Color(rgb) + Object Validation(0.5: Valid)
    float4 Ambient : SV_Target2;
};

PS_OUTPUT main(PS_INPUT input)
{   
    PS_OUTPUT output;
    
    float3 normal = normalize(input.Normal);
    float normalLen = length(normal);
    output.Normal = (normalLen < 0.001f) ? float4(0.f, 0.f, 0.f, 0.f) : float4(normal * 0.5f + 0.5f, 1.0f);
    
    float2 uv = input.TexCoord;
    float4 textureColor = g_DiffuseMap.Sample(g_sampler0, uv);
    //if (textureColor.w == 0.0f)
    //{
    //    clip(-1);
    //}
    float4 diffuseColor = float4(Material.DiffuseColor, 1.0f);
    
    bool isValidTexture = dot(textureColor, float4(1, 1, 1, 1)) > 1e-5f;
    bool isValidDiffuse = dot(Material.DiffuseColor, float3(1, 1, 1)) > 1e-5f;
    
    if (isValidTexture && isValidDiffuse)
    {
        output.Albedo = diffuseColor * textureColor;
    }
    else if (isValidTexture)
    {
        output.Albedo = textureColor;
        //output.Albedo = float4(1.f, 0.f, 1.f, 1.f);
    }
    else if (isValidDiffuse)
    {
        output.Albedo = diffuseColor;
    }
    else
    {
        output.Albedo = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    output.Ambient = (Material.AmbientColor.xyz, 1.0f);
    output.Albedo.w = 0.5f;

    return output;
}


