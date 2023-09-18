#pragma once

#include "runtime/function/render/render_pass.h"

namespace Piccolo
{
    struct VignettePassInitInfo : RenderPassInitInfo
    {
        VkRenderPass render_pass;
        VkImageView  input_attachment;
    };

    class VignettePass : public RenderPass
    {
    public:
        void initialize(const RenderPassInitInfo* init_info) override final;
        void draw() override final;

        void updateAfterFramebufferRecreate(VkImageView input_attachment);

    private:
        void setupDescriptorSetLayout();
        void setupPipelines();
        void setupDescriptorSet();
    };
} // namespace Piccolo
