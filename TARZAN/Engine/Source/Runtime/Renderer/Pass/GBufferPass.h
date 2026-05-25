#ifndef GBUFFERPASS_H
#define GBUFFERPASS_H

#include "RenderPass.h"

class GBufferPass : public RenderPass {
protected:
    void Setup() override;
    void Execute() override;
    const wchar_t* GetName() const override { return L"GBufferPass"; }
};

#endif // GBUFFERPASS_H
