#ifndef LIGHTINGPASS_H
#define LIGHTINGPASS_H

#include "RenderPass.h"

class LightingPass : public RenderPass {
protected:
    void Setup() override;
    void Execute() override;
    void Cleanup() override;
    const wchar_t* GetName() const override { return L"LightingPass"; }
};

#endif // LIGHTINGPASS_H
