#include "OverlayPass.h"
#include "Renderer/Renderer.h"
#include "D3D11RHI/GraphicDevice.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/PrimitiveBatch.h"

void OverlayPass::Setup()
{
    // FrameBuffer RTV + DSV 바인딩 (Depth Test 활성)
    Context->OMSetRenderTargets(1, &Renderer->Graphics->FrameBufferRTV, Renderer->Graphics->DepthStencilView);
}

void OverlayPass::Execute()
{
    // Line
    FVector CamPos = Renderer->ActiveViewport->GetCameraLocation();
    FVector4 CamPos4 = FVector4(CamPos.x, CamPos.y, CamPos.z, 1.f);
    float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
    Context->OMSetBlendState(Renderer->Graphics->LineBlendState, blendFactor, 0xffffffff);
    UPrimitiveBatch::GetInstance().RenderBatch(Renderer->ConstantBuffer, Renderer->ActiveViewport->GetViewMatrix(), Renderer->ActiveViewport->GetProjectionMatrix(), CamPos4);
    Context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

    // Gizmo
    Renderer->RenderGizmos();
}
