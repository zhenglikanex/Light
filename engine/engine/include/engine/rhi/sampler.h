#pragma once

#include <numeric>

#include "types.h"
#include "engine/core/base.h"

#undef max
#undef min

namespace light::rhi
{
	struct SamplerDesc
	{
        SamplerFilter filter = SamplerFilter::kMIN_MAG_MIP_LINEAR;
        SamplerMode u = SamplerMode::kWRAP;
        SamplerMode v = SamplerMode::kWRAP;
        SamplerMode w = SamplerMode::kWRAP;
        float mip_lod_bias = 0.0f;
        uint32_t max_anisotropy = 1;
        ComparisonFunc comparison_func = ComparisonFunc::kAlways;
        float border_color[4] = { 0.0f };
        float min_lod = 0;
        float max_lod = std::numeric_limits<float>::max();
	};

    class Sampler : public RefCounter
    {
    public:
        explicit Sampler(const SamplerDesc& desc)
            : desc_(desc)
        {

        }

        const SamplerDesc& GetDesc() const { return desc_; }
    private:
        SamplerDesc desc_;
    };

    using SamplerHandle = Ref<Sampler>;
}