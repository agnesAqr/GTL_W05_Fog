#ifndef OVERLAYPASS_H
#define OVERLAYPASS_H

#include "RenderPass.h"

class OverlayPass : public RenderPass {
protected:
    void Setup() override;
    void Execute() override;
    const wchar_t* GetName() const override { return L"OverlayPass"; }
};

#endif // OVERLAYPASS_H
