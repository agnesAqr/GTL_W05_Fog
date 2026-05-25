#include "LightingPass.h"
#include "Renderer/Renderer.h"
#include "D3D11RHI/GraphicDevice.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Components/FireballComp.h"
#include "SpotLightComp.h"
#include "UObject/Casts.h"

void LightingPass::Setup()
{
    Renderer->PrepareLightShader();

    // RenderTarget 설정 (Color, Position)
    Context->OMSetRenderTargets(2, Renderer->Graphics->LightPassRTVs, nullptr);

    // GBuffer SRV (Normal, Albedo, Ambient, Position)
    ID3D11ShaderResourceView* SRVs[] = {
        Renderer->Graphics->GBufferSRV_Normal,
        Renderer->Graphics->GBufferSRV_Albedo,
        Renderer->Graphics->GBufferSRV_Ambient,
        Renderer->Graphics->GBufferSRV_Position
    };
    Context->PSSetShaderResources(0, 4, SRVs);

    Context->PSSetSamplers(0, 1, &Renderer->SamplerState);
}

void LightingPass::Execute()
{
    // Directional Light
    FLightConstant GlobalLight = {
        .Ambient = FVector4(0.1f, 0.1f, 0.1f, 1.f),
        .Diffuse = FVector4(1.0f, 1.0f, 1.0f, 1.0f),
        .Specular = FVector4(1.0f, 1.0f, 1.0f, 1.0f),
        .Emissive = FVector(1.0f, 1.0f, 1.0f),
        .Padding1 = 0,
        .Direction = FVector(1.0f, -1.0f, -1.0f),
        .Padding2 = 0,
        .CameraPosition = Renderer->ActiveViewport->GetCameraLocation(),
        .Padding = (float)Renderer->ActiveViewport->GetViewMode(),
        .InverseView = FMatrix::Inverse(Renderer->ActiveViewport->GetViewMatrix()),
        .InverseProjection = FMatrix::Inverse(Renderer->ActiveViewport->GetProjectionMatrix()),
    };
    Renderer->ConstantBufferUpdater.UpdateGlobalLightConstant(Renderer->LPLightConstantBuffer, GlobalLight);

    Renderer->RenderLight();

    // Point Light / Spot Light
    std::unique_ptr<FFireballArrayInfo> fireballArrayInfo = std::make_unique<FFireballArrayInfo>();

    if (Renderer->FireballObjs.Num() > 0)
    {
        fireballArrayInfo->FireballCount = 0;

        for (int i = 0; i < Renderer->FireballObjs.Num(); i++)
        {
            if (Renderer->FireballObjs[i] != nullptr)
            {
                const FFireballInfo& fireballInfo = Renderer->FireballObjs[i]->GetFireballInfo();
                fireballArrayInfo->FireballConstants[i].Intensity = fireballInfo.Intensity;
                fireballArrayInfo->FireballConstants[i].Radius = fireballInfo.Radius;
                fireballArrayInfo->FireballConstants[i].Color = fireballInfo.Color;
                fireballArrayInfo->FireballConstants[i].RadiusFallOff = fireballInfo.RadiusFallOff;
                fireballArrayInfo->FireballConstants[i].Position = Renderer->FireballObjs[i]->GetWorldLocation();
                fireballArrayInfo->FireballConstants[i].LightType = fireballInfo.Type;
                if (USpotLightComponent* spotLight = Cast<USpotLightComponent>(Renderer->FireballObjs[i]))
                {
                    fireballArrayInfo->FireballConstants[i].InnerAngle = spotLight->GetInnerSpotAngle();
                    fireballArrayInfo->FireballConstants[i].OuterAngle = spotLight->GetOuterSpotAngle();
                    fireballArrayInfo->FireballConstants[i].Direction = spotLight->GetForwardVector();
                }
                fireballArrayInfo->FireballCount++;
            }
        }
    }
    Renderer->ConstantBufferUpdater.UpdateFireballConstant(Renderer->FireballConstantBuffer, *fireballArrayInfo);
    Renderer->ConstantBufferUpdater.UpdateScreenConstant(Renderer->ScreenConstantBuffer, Renderer->ActiveViewport);

    // Fullscreen Quad 렌더링
    Renderer->RenderFullScreenQuad();
}

void LightingPass::Cleanup()
{
    // SRV 언바인딩 (다음 Pass에서 RTV/SRV 충돌 방지)
    ID3D11ShaderResourceView* nullSRVs[4] = { nullptr, nullptr, nullptr, nullptr };
    Context->PSSetShaderResources(0, 4, nullSRVs);
}
