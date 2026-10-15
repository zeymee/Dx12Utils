#pragma once
#include <cstdint>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl.h>
using namespace Microsoft::WRL;

namespace sg::dxTypes
{
	struct Resource_Desc
	{
		Resource_Desc() = default;

		D3D12_RESOURCE_DESC desc{
			D3D12_RESOURCE_DIMENSION_BUFFER,
			0,
			0,
			0,
			1,
			1,
			DXGI_FORMAT_UNKNOWN,
			{1, 0},
			D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			D3D12_RESOURCE_FLAG_NONE};
		void init(uint64_t width, D3D12_RESOURCE_DIMENSION dimension = D3D12_RESOURCE_DIMENSION_BUFFER)
		{
			desc.Dimension = dimension;
			desc.Alignment = 0;
			desc.Width = width;
			desc.Height = 1;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		}
		Resource_Desc(uint64_t width, D3D12_RESOURCE_DIMENSION dimension = D3D12_RESOURCE_DIMENSION_BUFFER)
		{
			init(width, dimension);
		}

		void init(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
		{
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.Alignment = 0;
			desc.Width = width;
			desc.Height = height;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = format;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			desc.Flags = flags;
		}
		Resource_Desc(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
		{
			init(width, height, format, flags);
		}

		const D3D12_RESOURCE_DESC *operator&() { return &desc; }
	};

	struct Heap_Props
	{
		D3D12_HEAP_PROPERTIES desc;
		void init(D3D12_HEAP_TYPE type)
		{
			desc.Type = type;
			desc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			desc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			desc.CreationNodeMask = 0;
			desc.VisibleNodeMask = 0;
		}
		Heap_Props(D3D12_HEAP_TYPE type)
		{
			init(type);
		}
		Heap_Props() = default;
		const D3D12_HEAP_PROPERTIES *operator&() { return &desc; }
	};

	struct Clear_Value
	{
		D3D12_CLEAR_VALUE desc;
		void init(DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f)
		{
			desc.Format = format;
			desc.Color[0] = r;
			desc.Color[1] = g;
			desc.Color[2] = b;
			desc.Color[3] = a;
		}
		Clear_Value(DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f)
		{
			init(format, r, g, b, a);
		}
		Clear_Value() = default;
		const D3D12_CLEAR_VALUE *operator&() { return &desc; }
	};

	struct Viewport
	{
		D3D12_VIEWPORT desc;
		void init(float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f)
		{
			desc.TopLeftX = 0.0f;
			desc.TopLeftY = 0.0f;
			desc.Width = width;
			desc.Height = height;
			desc.MinDepth = minDepth;
			desc.MaxDepth = maxDepth;
		}
		Viewport(float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f)
		{
			init(width, height, minDepth, maxDepth);
		}
		Viewport() = default;
		const D3D12_VIEWPORT *operator&() { return &desc; }
		const D3D12_VIEWPORT *operator->() { return &desc; }
	};

	struct Rect
	{
		D3D12_RECT desc;
		void init(uint32_t width, uint32_t height)
		{
			desc.left = 0;
			desc.top = 0;
			desc.right = width;
			desc.bottom = height;
		}
		Rect(uint32_t width, uint32_t height)
		{
			init(width, height);
		}
		Rect() = default;

		const D3D12_RECT *operator&() { return &desc; }
		const D3D12_RECT *operator->() { return &desc; }
	};

	struct Resource_Barrier
	{
		D3D12_RESOURCE_BARRIER desc;
		void init(ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
		{
			desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			desc.Transition.pResource = resource.Get();
			desc.Transition.StateBefore = beforeState;
			desc.Transition.StateAfter = afterState;
			desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		}
		Resource_Barrier(ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
		{
			init(resource, beforeState, afterState);
		}
		Resource_Barrier() = default;

		const D3D12_RESOURCE_BARRIER *operator&() { return &desc; }
	};

	struct Descriptor_Heap_Desc
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		void init(uint32_t numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE)
		{
			desc.NumDescriptors = numDescriptors;
			desc.Type = type;
			desc.Flags = flags;
			desc.NodeMask = 0;
		}
		Descriptor_Heap_Desc(uint32_t numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE)
		{
			init(numDescriptors, type, flags);
		}
		Descriptor_Heap_Desc() = default;

		D3D12_DESCRIPTOR_HEAP_DESC *operator&() { return &desc; }
	};

	struct Swapchain_Desc
	{
		DXGI_SWAP_CHAIN_DESC1 desc;

		void init(
			uint32_t width, uint32_t height,
			uint32_t bufferCount,
			DXGI_FORMAT format,
			DXGI_SWAP_EFFECT swapEffect,
			uint32_t flags)
		{
			desc.Width = width;
			desc.Height = height;
			desc.Format = format;
			desc.Stereo = FALSE;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.BufferCount = bufferCount;
			desc.Scaling = DXGI_SCALING_STRETCH;
			desc.SwapEffect = swapEffect;
			desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			desc.Flags = flags;
		}
		Swapchain_Desc(
			uint32_t width, uint32_t height,
			uint32_t bufferCount = 3,
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			uint32_t flags = 0)
		{
			init(width, height, bufferCount, format, swapEffect, flags);
		}
		Swapchain_Desc() = default;

		const DXGI_SWAP_CHAIN_DESC1 *operator&() { return &desc; }
	};

	struct Shader_Resource_View_Desc
	{

		D3D12_SHADER_RESOURCE_VIEW_DESC desc;

		void init(DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
		{
			desc.Format = format;
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.Texture2D.MostDetailedMip = 0;
			desc.Texture2D.MipLevels = 1;
			desc.Texture2D.PlaneSlice = 0;
			desc.Texture2D.ResourceMinLODClamp = 0.0f;
		}

		void init(uint32_t firstElement, uint32_t numElements, uint32_t structureByteStride)
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.Buffer.FirstElement = firstElement;
			desc.Buffer.NumElements = numElements;
			desc.Buffer.StructureByteStride = structureByteStride;
		}

		void init(D3D12_GPU_VIRTUAL_ADDRESS gpuAddress)
		{
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.RaytracingAccelerationStructure.Location = gpuAddress;
		}

		Shader_Resource_View_Desc(DXGI_FORMAT format)
		{
			init(format);
		}

		Shader_Resource_View_Desc(uint32_t firstElement, uint32_t numElements, uint32_t structureByteStride)
		{
			init(firstElement, numElements, structureByteStride);
		}

		Shader_Resource_View_Desc(D3D12_GPU_VIRTUAL_ADDRESS gpuAddress)
		{
			init(gpuAddress);
		}

		Shader_Resource_View_Desc() = default;

		D3D12_SHADER_RESOURCE_VIEW_DESC *operator&() { return &desc; }
	};

	struct Blend_Desc
	{
		D3D12_BLEND_DESC desc;
		void init(
			D3D12_BLEND srcBlend = D3D12_BLEND_SRC_ALPHA,
			D3D12_BLEND dstBlend = D3D12_BLEND_INV_SRC_ALPHA,
			D3D12_COLOR_WRITE_ENABLE renderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL,
			bool alphaToCoverageEnable = false)
		{
			desc.AlphaToCoverageEnable = alphaToCoverageEnable;
			desc.IndependentBlendEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = srcBlend;
			desc.RenderTarget[0].DestBlend = dstBlend;
			desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = renderTargetWriteMask;
		}

		Blend_Desc(
			D3D12_BLEND srcBlend = D3D12_BLEND_SRC_ALPHA,
			D3D12_BLEND dstBlend = D3D12_BLEND_INV_SRC_ALPHA,
			D3D12_COLOR_WRITE_ENABLE renderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL,
			bool alphaToCoverageEnable = false)
		{
			init(srcBlend, dstBlend, renderTargetWriteMask, alphaToCoverageEnable);
		}

		D3D12_BLEND_DESC *operator&() { return &desc; }
	};
	// TODO: Revoir la configuration par défaut du depth stencil
	struct Depth_Stencil_Desc
	{
		D3D12_DEPTH_STENCIL_DESC desc;
		Depth_Stencil_Desc(
			bool enableDepthStencil = true,
			bool reverseDepth = false,
			bool stencilEnable = false)
		{
			desc.DepthEnable = enableDepthStencil;
			desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = reverseDepth ? D3D12_COMPARISON_FUNC_GREATER : D3D12_COMPARISON_FUNC_LESS;
			desc.StencilEnable = stencilEnable;
			desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
			desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			desc.BackFace = desc.FrontFace;
		}
	};

	// TODO: Revoir la configuration par défaut du rasterizer
	struct Rasterizer_Desc
	{
		D3D12_RASTERIZER_DESC desc;

		Rasterizer_Desc()
		{
			desc.FillMode = D3D12_FILL_MODE_SOLID;
			desc.CullMode = D3D12_CULL_MODE_BACK;
			desc.FrontCounterClockwise = false;
			desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
			desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
			desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			desc.DepthClipEnable = true;
			desc.MultisampleEnable = false;
			desc.AntialiasedLineEnable = false;
			desc.ForcedSampleCount = 0;
			desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		}
	};

	struct GenericData
	{
		const void *pData{nullptr};
		size_t byteSize{0};
	};
}