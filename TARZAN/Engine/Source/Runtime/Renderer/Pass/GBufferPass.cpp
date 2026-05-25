#include "GBufferPass.h"
#include "Renderer/Renderer.h"
#include "D3D11RHI/GraphicDevice.h"
#include "PropertyEditor/ShowFlags.h"
#include "UnrealEd/EditorViewportClient.h"

void GBufferPass::Setup()
{
    Context->OMSetRenderTargets(3, Renderer->Graphics->GBufferRTVs, Renderer->Graphics->DepthStencilView);
}

void GBufferPass::Execute()
{
    // StaticMesh
    if (Renderer->ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_Primitives))
        Renderer->RenderStaticMeshes();

    // Billboard
    if (Renderer->ActiveViewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_BillboardText))
        Renderer->RenderBillboards();
}
