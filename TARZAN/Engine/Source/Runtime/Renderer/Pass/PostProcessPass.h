#ifndef POSTPROCESSPASS_H
#define POSTPROCESSPASS_H

#include "RenderPass.h"

class PostProcessPass : public RenderPass
{
protected:
    void Setup() override;
    void Execute() override;
    void Cleanup() override;
    const wchar_t* GetName() const override { return L"PostProcessPass"; }
};

#endif // POSTPROCESSPASS_H
