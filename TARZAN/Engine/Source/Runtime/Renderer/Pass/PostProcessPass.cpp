#include "PostProcessPass.h"
#include "Renderer/Renderer.h"
#include "D3D11RHI/GraphicDevice.h"
#include "PropertyEditor/ShowFlags.h"
#include "UnrealEd/EditorViewportClient.h"

void PostProcessPass::Setup()
{
    Renderer->PreparePostProcessShader();

    ID3D11RenderTargetView* FrameBufferRTV = Renderer->Graphics->FrameBufferRTV;
    Context->OMSetRenderTargets(1, &FrameBufferRTV, nullptr);

    ID3D11ShaderResourceView* PPSRVs[] = {
        Renderer->Graphics->LightPassSRV_Color,
        Renderer->Graphics->LightPassSRV_Position
    };
    Context->PSSetShaderResources(0, 2, PPSRVs);

    Context->PSSetSamplers(0, 1, &Renderer->SamplerState);
}

void PostProcessPass::Execute()
{
    bool bFogEnabled = Renderer->ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Fog);
    if (!bFogEnabled)
    {
        Renderer->FogData.DisableFog = 1.0f;
    }

    Renderer->ConstantBufferUpdater.UpdateFogConstant(Renderer->FogConstantBuffer, Renderer->FogData);
    Renderer->ConstantBufferUpdater.UpdateScreenConstant(Renderer->ScreenConstantBuffer, Renderer->ActiveViewport);

    Renderer->RenderFullScreenQuad();
}

void PostProcessPass::Cleanup()
{
    ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };
    Context->PSSetShaderResources(0, 2, nullSRVs);
}
