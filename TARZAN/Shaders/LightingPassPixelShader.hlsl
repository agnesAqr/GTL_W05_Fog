#define Max_Fireball 300

Texture2D g_GBufferNormal : register(t0);
Texture2D g_GBufferAlbedo : register(t1);
Texture2D g_GBufferAmbient : register(t2);
Texture2D g_DepthBuffer : register(t3);

SamplerState g_sampler : register(s0);

struct FLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Emissive;
    float Padding1;
    float3 Direction;
    float Padding2;
};

struct FireballConstants
{
    float3 FireballPosition;
    float FireballIndensity;
    float radius;
    float RadiusFallOff;
    float InnerAngle;
    float OuterAngle;
    float4 FireballColor;
    float3 Direction;
    int LightType; // 0: Point, 1: Spot
};

cbuffer GlobalLightConstants : register(b0)
{
    FLight GlobalLight;
    float3 CameraPosition;
    float ViewMode;
    row_major float4x4 InverseView;
    row_major float4x4 InverseProjection;
};

cbuffer FireballBuffer : register(b1)
{
    FireballConstants Fireball[Max_Fireball];
    int FierballCount;
    float3 padding;
}

cbuffer ScreenInfo : register(b2)
{
    float2 ViewPortRatio;
    float2 ViewPortPosition;
}

struct PS_Input
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

float3 ReconstructWorldPos(float2 UV, float Depth)
{
    float4 NDC;
    NDC.xy = UV * 2.0 - 1.0;
    NDC.y *= -1;
    NDC.z = Depth;
    NDC.w = 1.0;

    float4 WorldPos = mul(NDC, InverseProjection);
    WorldPos /= WorldPos.w;

    WorldPos = mul(WorldPos, InverseView);

    return WorldPos.xyz;
}

float4 ComputeDirectionalLight(float3 normal, float3 worldPosition, float3 albedo, float3 ambient)
{
    float4 ambientColor = 0;
    float4 diffuseColor = 0;

    float3 lightDir = normalize(-GlobalLight.Direction);
    normal = normalize(normal);

    // === Ambient ===
    ambientColor = float4(albedo, 1.0f) * GlobalLight.Ambient * float4(ambient, 1.0f);

    // === Diffuse ===
    float NdotL = saturate(dot(normal, lightDir));
    diffuseColor = float4(albedo, 1.0f) * GlobalLight.Diffuse * NdotL;

    return ambientColor + diffuseColor;
}

float3 ComputeFireballLighting(float4 worldPos, float3 normal)
{
    float3 N = normalize(normal);
    float3 V = normalize(CameraPosition - worldPos.xyz);
    float3 totalLighting = float3(0, 0, 0);

    for (int i = 0; i < FierballCount; ++i)
    {
        float3 L = normalize(Fireball[i].FireballPosition - worldPos.xyz);
        float3 H = normalize(L + V);

        float dist = length(Fireball[i].FireballPosition - worldPos.xyz);
        float attenuation = 1.0 / (1.0 + Fireball[i].RadiusFallOff * (dist / Fireball[i].radius) * (dist / Fireball[i].radius));
        attenuation *= Fireball[i].FireballIndensity;

        float spotAttenuation = 1.0f;
        if (Fireball[i].LightType == 1) // Spot light
        {
            float3 spotDir = normalize(Fireball[i].Direction);
            float spotCos = dot(-L, spotDir);
            float innerCos = cos(radians(Fireball[i].InnerAngle/2));
            float outerCos = cos(radians(Fireball[i].OuterAngle/2));
            spotAttenuation = saturate((spotCos - outerCos) / (innerCos - outerCos));
        }

        float finalAttenuation = attenuation * spotAttenuation;

        float diffuse = saturate(dot(N, L));
        float3 diffuseColor = diffuse * Fireball[i].FireballColor.rgb * finalAttenuation;

        totalLighting += diffuseColor;
    }

    return totalLighting;
}

float LinearNormalizeDepth(float z_ndc, float nearZ, float farZ)
{
    float LinearValue = nearZ * farZ / (farZ - z_ndc * (farZ - nearZ));
    return saturate((LinearValue - nearZ) / (farZ - nearZ));
}

PS_OUTPUT main(PS_Input input)
{
    PS_OUTPUT output;

    float2 uv = input.TexCoord * ViewPortRatio + ViewPortPosition;

    float4 normalTex = g_GBufferNormal.Sample(g_sampler, uv);
    float4 albedoTex = g_GBufferAlbedo.Sample(g_sampler, uv);
    float3 ambient = g_GBufferAmbient.Sample(g_sampler, uv).rgb;
    float depth = g_DepthBuffer.Sample(g_sampler, uv).r;

    float3 albedo = albedoTex.rgb;
    float isValidObject = albedoTex.a;

    float3 worldPos = ReconstructWorldPos(input.TexCoord, depth);

    float3 normal = (normalTex.xyz - 0.5f) * 2.0f;

    float4 DirectionLightColor;
    float3 PointLightColor;

    if (isValidObject == 0.5f)
    {
        switch (ViewMode)
        {
            case 0 : // Lit
                DirectionLightColor = ComputeDirectionalLight(normal, worldPos, albedo, ambient);
                PointLightColor = ComputeFireballLighting(float4(worldPos, 1), normal);
                output.Color = DirectionLightColor + float4(PointLightColor.xyz, 1);
                break;
            case 1:
            case 2: // Unlit, WireFrame
                output.Color = float4(albedo.xyz, 1.f);
                break;
            case 3: // Base Color
                output.Color = float4(albedo.xyz, 1.f);
                break;
            case 4: // Normal
                output.Color = float4(normal, 1.f);
                break;
            case 5: // Depth
            {
                float linearDepth = LinearNormalizeDepth(depth, 0.1, 1000);
                output.Color = float4(linearDepth, linearDepth, linearDepth, 1.f);
                break;
            }
            case 6: // World Position
                output.Color = float4(normalize(worldPos), 1.f);
                break;
            default:
                output.Color = float4(1, 0, 1, 1);
                break;
        }
    }
    else if (isValidObject == 0.7f)
    {
        output.Color = float4(albedo.xyz, 1.f);
    }
    else
    {
        output.Color = float4(albedo.xyz, 1.f);
    }

    return output;
}
