#pragma once
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#define _TCHAR_DEFINED
#include <d3d11.h>

#include "EngineBaseTypes.h"

#include "Core/HAL/PlatformType.h"
#include "Core/Math/Vector4.h"

class FGraphicsDevice {
public:
    ID3D11Device* Device = nullptr;
    ID3D11DeviceContext* DeviceContext = nullptr;
    IDXGISwapChain* SwapChain = nullptr;
    
    ID3D11Texture2D* FrameBuffer = nullptr;
    ID3D11Texture2D* UUIDFrameBuffer = nullptr;
    
    ID3D11RenderTargetView* RTVs[2];
    ID3D11RenderTargetView* FrameBufferRTV = nullptr;
    ID3D11RenderTargetView* UUIDFrameBufferRTV = nullptr;

    ID3D11RasterizerState* RasterizerStateSOLID = nullptr;
    ID3D11RasterizerState* RasterizerStateWIREFRAME = nullptr;
    DXGI_SWAP_CHAIN_DESC SwapchainDesc;

    // GBuffer(Normal, Albedo, Ambient)
    ID3D11Texture2D* GBufferTexture_Normal = nullptr;
    ID3D11Texture2D* GBufferTexture_Albedo = nullptr;
    ID3D11Texture2D* GBufferTexture_Ambient = nullptr;

    ID3D11RenderTargetView* GBufferRTV_Normal = nullptr;
    ID3D11RenderTargetView* GBufferRTV_Albedo = nullptr;
    ID3D11RenderTargetView* GBufferRTV_Ambient = nullptr;

    ID3D11ShaderResourceView* GBufferSRV_Normal = nullptr;
    ID3D11ShaderResourceView* GBufferSRV_Albedo = nullptr;
    ID3D11ShaderResourceView* GBufferSRV_Ambient = nullptr;

    ID3D11RenderTargetView* GBufferRTVs[3];
    // -

    // LightPass(Color)
    ID3D11Texture2D* LightPassTexture_Color = nullptr;

    ID3D11RenderTargetView* LightPassRTV_Color = nullptr;

    ID3D11ShaderResourceView* LightPassSRV_Color = nullptr;
    // -

    // PostProcessPass
    // -

    UINT screenWidth = 0;
    UINT screenHeight = 0;
    // Depth-Stencil 관련 변수
    ID3D11Texture2D* DepthStencilBuffer = nullptr;  
    ID3D11DepthStencilView* DepthStencilView = nullptr; 
    ID3D11ShaderResourceView* DepthStencilSRV = nullptr;
    ID3D11DepthStencilState* DepthStencilState = nullptr;
    FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f }; // 화면을 초기화(clear) 할 때 사용할 색상(RGBA)

    ID3D11DepthStencilState* DepthStateDisable = nullptr;

    ID3D11BlendState* LineBlendState = nullptr;

    void Initialize(HWND hWindow);

    void CreateDeviceAndSwapChain(HWND hWindow);
    void CreateDepthStencilBuffer(HWND hWindow);
    void CreateDepthStencilState();
    void CreateRasterizerState();
    void CreateGBuffer();
    void CreateFrameBuffer();
    void CreateLightPassBuffer();
    void CreateBlendState();
    
    void ReleaseDeviceAndSwapChain();
    void ReleaseGBuffer();
    void ReleaseFrameBuffer();
    void ReleaseLightPassBuffer();
    void ReleaseRasterizerState();
    void ReleaseDepthStencilResources();
    void ReleaseBlendState();
    void Release();
    
    void SwapBuffer();
    void Prepare();
    void OnResize(HWND hWindow);
    ID3D11RasterizerState* GetCurrentRasterizer() { return CurrentRasterizer; }
    void ChangeRasterizer(EViewModeIndex evi);
    void ChangeDepthStencilState(ID3D11DepthStencilState* newDetptStencil);

    uint32 GetPixelUUID(POINT pt);
    uint32 DecodeUUIDColor(FVector4 UUIDColor);
private:
    ID3D11RasterizerState* CurrentRasterizer = nullptr;
};

