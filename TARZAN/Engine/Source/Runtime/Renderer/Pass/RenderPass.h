#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <d3d11.h>
#include <d3d11_1.h> // ID3DUserDefinedAnnotation

class FRenderer;

class RenderPass {
public:
    virtual ~RenderPass()
    {
        if (Annotation)
        {
            Annotation->Release();
            Annotation = nullptr;
        }
    }

    void Initialize(FRenderer* InRenderer, ID3D11DeviceContext* InContext)
    {
        Renderer = InRenderer;
        Context = InContext;
    }

    void Render()
    {
        BeginEvent();
        Setup();
        Execute();
        Cleanup();
        EndEvent();
    }

protected:
    // 패스 실행 전 GPU 상태(셰이더/RTV/SRV/샘플러) 바인딩. 기본 no-op.
    virtual void Setup() {}

    // 패스의 핵심 드로우 및 프레임 데이터 갱신. 유일한 필수 구현.
    virtual void Execute() = 0;

    // 패스 종료 후 상태 복원(SRV 언바인딩 등). 기본 no-op.
    virtual void Cleanup() {}

    // GPU 디버그 마커 및 프로파일링 구간 이름.
    virtual const wchar_t* GetName() const = 0;

    FRenderer* Renderer = nullptr;
    ID3D11DeviceContext* Context = nullptr;

private:
    void BeginEvent()
    {
        if (!Annotation && Context)
        {
            Context->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), reinterpret_cast<void**>(&Annotation));
        }
        if (Annotation)
        {
            Annotation->BeginEvent(GetName());
        }
    }

    void EndEvent()
    {
        if (Annotation)
        {
            Annotation->EndEvent();
        }
    }

    ID3DUserDefinedAnnotation* Annotation = nullptr;
};

#endif // RENDERPASS_H
