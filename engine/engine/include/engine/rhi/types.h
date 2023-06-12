#pragma once

#include <cstdint>
#include "engine/core/base.h"

namespace light::rhi
{
	enum class GraphicsApi
	{
		kNone,
		kD3D12,
		kVulkan
	};

	enum class ResourceStates
	{
		kCommon = 0,
		kVertexAndConstantBuffer = 0x1,
		kIndexBuffer = 0x2,
		kRenderTarget = 0x4,
		kUnorderedAccess = 0x8,
		kDepthWrite = 0x10,
		kDepthRead = 0x20,
		kNonPixelShaderResource = 0x40,
		kPixelShaderResource = 0x80,
		kStreamOut = 0x100,
		kIndirectArgument = 0x200,
		kCopyDest = 0x400,
		kCopySource = 0x800,
		kResolveDest = 0x1000,
		kResolveSource = 0x2000,
		kRaytracingAccelerationStructure = 0x400000,
		kShadingRateSource = 0x1000000,
		kGenericRead = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
		kPresent = 0,
		kPredication = 0x200,
		kVideoDecodeRead = 0x10000,
		kVideoDecodeWrite = 0x20000,
		kVideoProcessRead = 0x40000,
		kVideoProcessWrite = 0x80000,
		kVideoEncodeRead = 0x200000,
		kVideoEncodeWrite = 0x800000
	};

	ENUM_CLASS_FLAG_OPERATORS(ResourceStates)

	enum class CpuAccess : uint8_t
	{
		kNone,		// 不在CPU端访问
		kRead,		// 在CPU端读取
		kWrite		// 在CPU端写入
	};

	enum class BufferType : uint8_t
	{
		kUnknown,
		kVertex,
		kIndex,
		kConstant,
	};

	struct DepthStencilValue
	{
		float depth;
		uint8_t stencil;
	};

	struct ClearValue
	{
		union
		{
			float color[4];
			DepthStencilValue depth_stencil;
		};
	};

	enum class TextureDimension : uint8_t
	{
		kUnknown,
		kTexture1D,
		kTexture1DArray,
		kTexture2D,
		kTexture2DArray,
		kTextureCube,
		kTextureCubeArray,
		kTexture2DMS,
		kTexture2DMSArray,
		kTexture3D
	};

	enum class ShaderType : uint8_t
	{
		kNone = 0,
		kVertex = 1,
		kHull = 2,
		kDomain = 3,
		kGeometry = 4,
		kPixel = 5,
		kAmplification = 6,
		kMesh = 7
	};

	enum class CommandListType : uint8_t
	{
		kDirect = 0,
		kCompute = 1,
		kCopy = 2,
	};

	enum class Format : uint8_t
	{
		UNKNOWN,

		R8_UINT,
		R8_SINT,
		R8_UNORM,
		R8_SNORM,
		RG8_UINT,
		RG8_SINT,
		RG8_UNORM,
		RG8_SNORM,
		R16_UINT,
		R16_SINT,
		R16_UNORM,
		R16_SNORM,
		R16_FLOAT,
		BGRA4_UNORM,
		B5G6R5_UNORM,
		B5G5R5A1_UNORM,
		RGBA8_UINT,
		RGBA8_SINT,
		RGBA8_UNORM,
		RGBA8_SNORM,
		BGRA8_UNORM,
		SRGBA8_UNORM,
		SBGRA8_UNORM,
		R10G10B10A2_UNORM,
		R11G11B10_FLOAT,
		RG16_UINT,
		RG16_SINT,
		RG16_UNORM,
		RG16_SNORM,
		RG16_FLOAT,
		R32_UINT,
		R32_SINT,
		R32_FLOAT,
		RGBA16_UINT,
		RGBA16_SINT,
		RGBA16_FLOAT,
		RGBA16_UNORM,
		RGBA16_SNORM,
		RG32_UINT,
		RG32_SINT,
		RG32_FLOAT,
		RGB32_UINT,
		RGB32_SINT,
		RGB32_FLOAT,
		RGBA32_UINT,
		RGBA32_SINT,
		RGBA32_FLOAT,

		D16,
		D24S8,
		X24G8_UINT,
		D32,
		D32S8,
		X32G8_UINT,

		BC1_UNORM,
		BC1_UNORM_SRGB,
		BC2_UNORM,
		BC2_UNORM_SRGB,
		BC3_UNORM,
		BC3_UNORM_SRGB,
		BC4_UNORM,
		BC4_SNORM,
		BC5_UNORM,
		BC5_SNORM,
		BC6H_UFLOAT,
		BC6H_SFLOAT,
		BC7_UNORM,
		BC7_UNORM_SRGB,

		COUNT,
	};
	
	enum class FormatKind
	{
		kInteger,
		kNormalized,
		kFloat,
		kDepthStencil,
	};

	struct FormatInfo
	{
		Format format;
		const char* name;
		uint8_t bytes_per_pixel;
		uint8_t block_size;
		FormatKind kind;
		bool has_red : 1;
		bool has_green : 1;
		bool has_blue : 1;
		bool has_alpha : 1;
		bool has_depth : 1;
		bool has_stencil : 1;
		bool is_signed : 1;
		bool is_srgb : 1;
	};

	inline const FormatInfo& GetFormatInfo(Format format)
	{
		// Format mapping table. The rows must be in the exactly same order as Format enum members are defined.
		static const FormatInfo s_kFormatInfo[] = {
			//        format                   name             bytes blk         kind               red   green   blue  alpha  depth  stencl signed  srgb
				{ Format::UNKNOWN,           "UNKNOWN",           0,   0, FormatKind::kInteger,      false, false, false, false, false, false, false, false },
				{ Format::R8_UINT,           "R8_UINT",           1,   1, FormatKind::kInteger,      true,  false, false, false, false, false, false, false },
				{ Format::R8_SINT,           "R8_SINT",           1,   1, FormatKind::kInteger,      true,  false, false, false, false, false, true,  false },
				{ Format::R8_UNORM,          "R8_UNORM",          1,   1, FormatKind::kNormalized,   true,  false, false, false, false, false, false, false },
				{ Format::R8_SNORM,          "R8_SNORM",          1,   1, FormatKind::kNormalized,   true,  false, false, false, false, false, false, false },
				{ Format::RG8_UINT,          "RG8_UINT",          2,   1, FormatKind::kInteger,      true,  true,  false, false, false, false, false, false },
				{ Format::RG8_SINT,          "RG8_SINT",          2,   1, FormatKind::kInteger,      true,  true,  false, false, false, false, true,  false },
				{ Format::RG8_UNORM,         "RG8_UNORM",         2,   1, FormatKind::kNormalized,   true,  true,  false, false, false, false, false, false },
				{ Format::RG8_SNORM,         "RG8_SNORM",         2,   1, FormatKind::kNormalized,   true,  true,  false, false, false, false, false, false },
				{ Format::R16_UINT,          "R16_UINT",          2,   1, FormatKind::kInteger,      true,  false, false, false, false, false, false, false },
				{ Format::R16_SINT,          "R16_SINT",          2,   1, FormatKind::kInteger,      true,  false, false, false, false, false, true,  false },
				{ Format::R16_UNORM,         "R16_UNORM",         2,   1, FormatKind::kNormalized,   true,  false, false, false, false, false, false, false },
				{ Format::R16_SNORM,         "R16_SNORM",         2,   1, FormatKind::kNormalized,   true,  false, false, false, false, false, false, false },
				{ Format::R16_FLOAT,         "R16_FLOAT",         2,   1, FormatKind::kFloat,        true,  false, false, false, false, false, true,  false },
				{ Format::BGRA4_UNORM,       "BGRA4_UNORM",       2,   1, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::B5G6R5_UNORM,      "B5G6R5_UNORM",      2,   1, FormatKind::kNormalized,   true,  true,  true,  false, false, false, false, false },
				{ Format::B5G5R5A1_UNORM,    "B5G5R5A1_UNORM",    2,   1, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::RGBA8_UINT,        "RGBA8_UINT",        4,   1, FormatKind::kInteger,      true,  true,  true,  true,  false, false, false, false },
				{ Format::RGBA8_SINT,        "RGBA8_SINT",        4,   1, FormatKind::kInteger,      true,  true,  true,  true,  false, false, true,  false },
				{ Format::RGBA8_UNORM,       "RGBA8_UNORM",       4,   1, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::RGBA8_SNORM,       "RGBA8_SNORM",       4,   1, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::BGRA8_UNORM,       "BGRA8_UNORM",       4,   1, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::SRGBA8_UNORM,      "SRGBA8_UNORM",      4,   1, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, true  },
				{ Format::SBGRA8_UNORM,      "SBGRA8_UNORM",      4,   1, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::R10G10B10A2_UNORM, "R10G10B10A2_UNORM", 4,   1, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::R11G11B10_FLOAT,   "R11G11B10_FLOAT",   4,   1, FormatKind::kFloat,        true,  true,  true,  false, false, false, false, false },
				{ Format::RG16_UINT,         "RG16_UINT",         4,   1, FormatKind::kInteger,      true,  true,  false, false, false, false, false, false },
				{ Format::RG16_SINT,         "RG16_SINT",         4,   1, FormatKind::kInteger,      true,  true,  false, false, false, false, true,  false },
				{ Format::RG16_UNORM,        "RG16_UNORM",        4,   1, FormatKind::kNormalized,   true,  true,  false, false, false, false, false, false },
				{ Format::RG16_SNORM,        "RG16_SNORM",        4,   1, FormatKind::kNormalized,   true,  true,  false, false, false, false, false, false },
				{ Format::RG16_FLOAT,        "RG16_FLOAT",        4,   1, FormatKind::kFloat,        true,  true,  false, false, false, false, true,  false },
				{ Format::R32_UINT,          "R32_UINT",          4,   1, FormatKind::kInteger,      true,  false, false, false, false, false, false, false },
				{ Format::R32_SINT,          "R32_SINT",          4,   1, FormatKind::kInteger,      true,  false, false, false, false, false, true,  false },
				{ Format::R32_FLOAT,         "R32_FLOAT",         4,   1, FormatKind::kFloat,        true,  false, false, false, false, false, true,  false },
				{ Format::RGBA16_UINT,       "RGBA16_UINT",       8,   1, FormatKind::kInteger,      true,  true,  true,  true,  false, false, false, false },
				{ Format::RGBA16_SINT,       "RGBA16_SINT",       8,   1, FormatKind::kInteger,      true,  true,  true,  true,  false, false, true,  false },
				{ Format::RGBA16_FLOAT,      "RGBA16_FLOAT",      8,   1, FormatKind::kFloat,        true,  true,  true,  true,  false, false, true,  false },
				{ Format::RGBA16_UNORM,      "RGBA16_UNORM",      8,   1, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::RGBA16_SNORM,      "RGBA16_SNORM",      8,   1, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::RG32_UINT,         "RG32_UINT",         8,   1, FormatKind::kInteger,      true,  true,  false, false, false, false, false, false },
				{ Format::RG32_SINT,         "RG32_SINT",         8,   1, FormatKind::kInteger,      true,  true,  false, false, false, false, true,  false },
				{ Format::RG32_FLOAT,        "RG32_FLOAT",        8,   1, FormatKind::kFloat,        true,  true,  false, false, false, false, true,  false },
				{ Format::RGB32_UINT,        "RGB32_UINT",        12,  1, FormatKind::kInteger,      true,  true,  true,  false, false, false, false, false },
				{ Format::RGB32_SINT,        "RGB32_SINT",        12,  1, FormatKind::kInteger,      true,  true,  true,  false, false, false, true,  false },
				{ Format::RGB32_FLOAT,       "RGB32_FLOAT",       12,  1, FormatKind::kFloat,        true,  true,  true,  false, false, false, true,  false },
				{ Format::RGBA32_UINT,       "RGBA32_UINT",       16,  1, FormatKind::kInteger,      true,  true,  true,  true,  false, false, false, false },
				{ Format::RGBA32_SINT,       "RGBA32_SINT",       16,  1, FormatKind::kInteger,      true,  true,  true,  true,  false, false, true,  false },
				{ Format::RGBA32_FLOAT,      "RGBA32_FLOAT",      16,  1, FormatKind::kFloat,        true,  true,  true,  true,  false, false, true,  false },
				{ Format::D16,               "D16",               2,   1, FormatKind::kDepthStencil, false, false, false, false, true,  false, false, false },
				{ Format::D24S8,             "D24S8",             4,   1, FormatKind::kDepthStencil, false, false, false, false, true,  true,  false, false },
				{ Format::X24G8_UINT,        "X24G8_UINT",        4,   1, FormatKind::kInteger,      false, false, false, false, false, true,  false, false },
				{ Format::D32,               "D32",               4,   1, FormatKind::kDepthStencil, false, false, false, false, true,  false, false, false },
				{ Format::D32S8,             "D32S8",             8,   1, FormatKind::kDepthStencil, false, false, false, false, true,  true,  false, false },
				{ Format::X32G8_UINT,        "X32G8_UINT",        8,   1, FormatKind::kInteger,      false, false, false, false, false, true,  false, false },
				{ Format::BC1_UNORM,         "BC1_UNORM",         8,   4, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::BC1_UNORM_SRGB,    "BC1_UNORM_SRGB",    8,   4, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, true  },
				{ Format::BC2_UNORM,         "BC2_UNORM",         16,  4, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::BC2_UNORM_SRGB,    "BC2_UNORM_SRGB",    16,  4, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, true  },
				{ Format::BC3_UNORM,         "BC3_UNORM",         16,  4, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::BC3_UNORM_SRGB,    "BC3_UNORM_SRGB",    16,  4, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, true  },
				{ Format::BC4_UNORM,         "BC4_UNORM",         8,   4, FormatKind::kNormalized,   true,  false, false, false, false, false, false, false },
				{ Format::BC4_SNORM,         "BC4_SNORM",         8,   4, FormatKind::kNormalized,   true,  false, false, false, false, false, false, false },
				{ Format::BC5_UNORM,         "BC5_UNORM",         16,  4, FormatKind::kNormalized,   true,  true,  false, false, false, false, false, false },
				{ Format::BC5_SNORM,         "BC5_SNORM",         16,  4, FormatKind::kNormalized,   true,  true,  false, false, false, false, false, false },
				{ Format::BC6H_UFLOAT,       "BC6H_UFLOAT",       16,  4, FormatKind::kFloat,        true,  true,  true,  false, false, false, false, false },
				{ Format::BC6H_SFLOAT,       "BC6H_SFLOAT",       16,  4, FormatKind::kFloat,        true,  true,  true,  false, false, false, true,  false },
				{ Format::BC7_UNORM,         "BC7_UNORM",         16,  4, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, false },
				{ Format::BC7_UNORM_SRGB,    "BC7_UNORM_SRGB",    16,  4, FormatKind::kNormalized,   true,  true,  true,  true,  false, false, false, true  },
		};

		static_assert(sizeof(s_kFormatInfo) / sizeof(FormatInfo) == size_t(Format::COUNT),
			"The format info table doesn't have the right number of elements");

		if (uint32_t(format) >= uint32_t(Format::COUNT))
			return s_kFormatInfo[0]; // UNKNOWN

		const FormatInfo& info = s_kFormatInfo[uint32_t(format)];
		assert(info.format == format);
		return info;
	}

	enum class ShaderBindResourceType : uint8_t
	{
		kConstantBuffer = 0,
		kTextureBuffer,
		kTexture,
		kSampler
	};

	enum class BindingParameterType : uint8_t
	{
		kDescriptorTable,
		kConstants,
		kConstantBufferView,
		kShaderResourceView,
		kUnorderAccessView
	};

	enum class ShaderVisibility : uint8_t
	{
		kAll = 0,
		kVertex = 1,
		kHull = 2,
		kDomain = 3,
		kGeometry = 4,
		kPixel = 5,
		kAmplification = 6,
		kMesh = 7
	};

	enum class DescriptorRangeType : uint8_t
	{
		kShaderResourceView,
		kUnorderAccessView,
		kConstantsBufferView,
		kSampler
	};

	enum class ClearFlags : uint8_t
	{
		kClearFlagDepth		= 0x1,
		kClearFlagStencil	= 0x2
	};

	ENUM_CLASS_FLAG_OPERATORS(ClearFlags);

	enum class PrimitiveTopology : uint8_t
	{
		kPointList,
		kLineList,
		kTriangleList,
		kTriangleStrip,
		kTriangleFan,
		kTriangleListWithAdjacency,
		kTriangleStripWithAdjacency,
		kPatchList
	};
	
	enum class FillMode : uint8_t
	{
		kWireframe = 2,
		kSolid = 3
	};

	enum class CullMode : uint8_t
	{
		kNone = 1,
		kFront = 2,
		kBack = 3
	};

	enum class ConservativeRasterizationMode : uint8_t
	{
		kOff = 0,
		kOn = 1
	};

	struct RasterizerDesc
	{
		FillMode fill_mode = FillMode::kSolid;
		CullMode cull_mode = CullMode::kBack; //todo
		bool front_counter_clockwise = false;
		int32_t depth_bias = 0;
		float depth_bias_clamp = 0.0;
		float slope_scaled_depth_bias = 0.0f;
		bool depth_clip_enable = true;
		bool multi_sample_enable = false;
		bool anti_aliased_line_enable = false;
		uint32_t forced_sample_count = 0;
		ConservativeRasterizationMode conservative_raster = ConservativeRasterizationMode::kOff;
	};

	enum class BlendFactor : uint8_t
	{
		kZero = 1,
		kOne = 2,
		kSrcColor = 3,
		kInvSrcColor = 4,
		kSrcAlpha = 5,
		kInvSrcAlpha = 6,
		kDestAlpha = 7,
		kInvDestAlpha = 8,
		kDestColor = 9,
		kInvDestColor = 10,
		kSrcAlphaSat = 11,
		kBlendFactor = 14,
		kInvBlendFactor = 15,
		kSrc1Color = 16,
		kInvSrc1Color = 17,
		kSrc1Alpha = 18,
		kInvSrc1Alpha = 19
	};

	enum class BlendOp : uint8_t
	{
		kAdd = 1,
		kSubtract = 2,
		kRevSubtract = 3,
		kMin = 4,
		kMax = 5
	};

	enum class ColorMask : uint8_t
	{
		kRed = 1,
		kGreen = 2,
		kBlue = 4,
		kAlpha = 8,
		kAll = 0xF
	};	

	struct BlendDesc
	{
		struct RenderTargetBlendDesc
		{
			bool blend_enable = false;
			BlendFactor src_blend = BlendFactor::kOne;
			BlendFactor dest_blend = BlendFactor::kZero;
			BlendOp blend_op = BlendOp::kAdd;
			BlendFactor src_blend_alpha = BlendFactor::kOne;
			BlendFactor dest_blend_alpha = BlendFactor::kZero;
			BlendOp blend_op_alpha = BlendOp::kAdd;
			ColorMask render_target_write_mask = ColorMask::kAll;
		};

		bool alpha_to_coverage_enable = false;
		bool independent_blend_enable = false;

		RenderTargetBlendDesc render_target[8] = {};
	};

	enum class DepthWriteMask
	{
		kZero = 0,
		kAll = 1
	};

	
	enum class ComparisonFunc
	{
		kNever = 1,
		kLess = 2,
		kEqual = 3,
		kLessEqual = 4,
		kGreater = 5,
		kNotEqual = 6,
		kGreaterEqual = 7,
		kAlways = 8
	};

	enum class StencilOp : uint8_t
	{
		kKeep = 1,
		kZero = 2,
		kReplace = 3,
		kIncrSat = 4,
		kDecrSat = 5,
		kInvert = 6,
		kIncr = 7,
		kDecr = 8
	};

	struct DepthStencilDesc
	{
		struct DepthStencilOpDesc
		{
			StencilOp stencil_fail_op = StencilOp::kKeep;
			StencilOp stencil_depth_fail_op = StencilOp::kKeep;
			StencilOp stencil_pass_op = StencilOp::kKeep;
			ComparisonFunc stencil_func = ComparisonFunc::kAlways;
		};

		bool depth_enable = true;
		DepthWriteMask depth_write_mask = DepthWriteMask::kAll;
		ComparisonFunc depth_func = ComparisonFunc::kLess;
		bool stencil_enable = false;
		uint8_t stencil_read_mask = 0xff;
		uint8_t stencil_write_mask = 0xff;
		DepthStencilOpDesc front_face;
		DepthStencilOpDesc back_face;
	};

	struct Size
	{
		float width = 0.0;
		float height = 0.0;
	};

	struct Viewport
	{
		float top_left_x;
		float top_left_y;
		float width;
		float height;
		float min_depth;
		float max_depth;
	};

	struct Rect
	{
		int32_t left;
		int32_t top;
		int32_t right;
		int32_t bottom;
	};

	struct SampleDesc
	{
		uint32_t count;
		uint32_t quality;
	};

	enum class SamplerFilter
	{
		kMIN_MAG_MIP_POINT = 0,
		kMIN_MAG_POINT_MIP_LINEAR = 0x1,
		kMIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
		kMIN_POINT_MAG_MIP_LINEAR = 0x5,
		kMIN_LINEAR_MAG_MIP_POINT = 0x10,
		kMIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
		kMIN_MAG_LINEAR_MIP_POINT = 0x14,
		kMIN_MAG_MIP_LINEAR = 0x15,
		kANISOTROPIC = 0x55,
		kCOMPARISON_MIN_MAG_MIP_POINT = 0x80,
		kCOMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
		kCOMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
		kCOMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
		kCOMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
		kCOMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
		kCOMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
		kCOMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
		kCOMPARISON_ANISOTROPIC = 0xd5,
		kMINIMUM_MIN_MAG_MIP_POINT = 0x100,
		kMINIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x101,
		kMINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x104,
		kMINIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x105,
		kMINIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x110,
		kMINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x111,
		kMINIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x114,
		kMINIMUM_MIN_MAG_MIP_LINEAR = 0x115,
		kMINIMUM_ANISOTROPIC = 0x155,
		kMAXIMUM_MIN_MAG_MIP_POINT = 0x180,
		kMAXIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x181,
		kMAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x184,
		kMAXIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x185,
		kMAXIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x190,
		kMAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x191,
		kMAXIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x194,
		kMAXIMUM_MIN_MAG_MIP_LINEAR = 0x195,
		kMAXIMUM_ANISOTROPIC = 0x1d5
	};

	enum class SamplerMode
	{
		kWRAP = 1,
		kMIRROR = 2,
		kCLAMP = 3,
		kBORDER = 4,
		kMIRROR_ONCE = 5
	};

	struct TextureSubresourceData
	{
		char* data = nullptr;
		//每行纹理的字节数
		uint32_t row_pitch = 0;
		//每个纹理数组元素的字节数
		uint32_t depth_pitch = 0;
		uint32_t data_size = 0;
	};
}
