#include "Dx12FncUtils.hpp"

ComPtr<ID3D12Resource> createResource(
    uint64_t sizeInBytes,
    D3D12_HEAP_TYPE heapType,
    D3D12_RESOURCE_STATES initialState
)
{
    sg::dxTypes::Resource_Desc resDesc(sizeInBytes, D3D12_RESOURCE_DIMENSION_BUFFER);
    sg::dxTypes::Heap_Props heapProps(heapType);

    ComPtr<ID3D12Device14> device = Dx12Core::getDevice();
    ComPtr<ID3D12Resource> resource;
    throwIfFailed(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        initialState,
        nullptr,
        IID_PPV_ARGS(&resource)
    ));
    return resource;
}

ComPtr<ID3D12Resource> createResource2D(uint32_t width, uint32_t height, D3D12_HEAP_TYPE heapType, DXGI_FORMAT format, D3D12_RESOURCE_STATES initialState)
{
    sg::dxTypes::Resource_Desc resDesc(width, height, format);
    sg::dxTypes::Heap_Props heapProps(heapType);

    ComPtr<ID3D12Device14> device = Dx12Core::getDevice();
    ComPtr<ID3D12Resource> resource;
    throwIfFailed(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        initialState,
        nullptr,
        IID_PPV_ARGS(&resource)
    ));
    return resource;
}

ComPtr<ID3D12Resource> createContiguousResource2D(uint32_t width, uint32_t height, D3D12_HEAP_TYPE heapType, DXGI_FORMAT format, D3D12_RESOURCE_STATES initialState)
{
    sg::dxTypes::Resource_Desc resDesc(width, height, format);
    uint64_t totalBytes{ 0 };

    ComPtr<ID3D12Device14> device = Dx12Core::getDevice();
    device->GetCopyableFootprints(
        &resDesc,
        0, 1, 0,
        nullptr, nullptr, nullptr,
        &totalBytes
    );
    return createResource(totalBytes, heapType, initialState);
}

ComPtr<ID3D12PipelineState> createPipelineState(
    const sg::dxTypes::Blend_Desc& blendDesc,
    const sg::dxTypes::Rasterizer_Desc& rasterizerDesc,
    const sg::dxTypes::Depth_Stencil_Desc& depthStencilDesc,
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutDescs,
    const ComPtr<ID3D12RootSignature>& rootSignature,
    const sg::dxTypes::GenericData& vShader,
    const sg::dxTypes::GenericData& pShader,
    DXGI_FORMAT renderTargetFormat)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.VS = {  vShader.pData, vShader.byteSize };
    psoDesc.PS = {  pShader.pData, pShader.byteSize };
    psoDesc.StreamOutput = {};
	psoDesc.BlendState = blendDesc.desc;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState = rasterizerDesc.desc;
	psoDesc.DepthStencilState = depthStencilDesc.desc;
	psoDesc.InputLayout.pInputElementDescs = inputLayoutDescs.data();
	psoDesc.InputLayout.NumElements = inputLayoutDescs.size();
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = renderTargetFormat;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.NodeMask = 0;
	psoDesc.CachedPSO.pCachedBlob = nullptr;
	psoDesc.CachedPSO.CachedBlobSizeInBytes = 0;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    ComPtr<ID3D12Device14> device = Dx12Core::getDevice();
    ComPtr<ID3D12PipelineState> pipelineState;
    throwIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
    return pipelineState;
}

void uploadCpuDataToResource(
    const void *pData,
    ComPtr<ID3D12Resource> &dstResource,
    size_t byteSize, size_t dstOffset)
{
    D3D12_RANGE range{};
    range.Begin = dstOffset;
    range.End = dstOffset + byteSize;

    void* mappedData{ nullptr };
    throwIfFailed(dstResource->Map(0, &range, &mappedData));
    memcpy(mappedData, pData, byteSize);
    dstResource->Unmap(0, &range);

}

void uploadCpuDataToResource2D(
    uint32_t width, uint32_t height,
    DXGI_FORMAT format,
    const uint8_t* pData,
    ComPtr<ID3D12Resource> &dstResource,
    size_t byteSize, size_t dstOffset)
{
    sg::dxTypes::Resource_Desc resDesc(width, height, format);
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};

    ComPtr<ID3D12Device14> device = Dx12Core::getDevice();
    device->GetCopyableFootprints(
        &resDesc,
        0, 1, 0,
        &footprint,
        nullptr, nullptr, nullptr
    );

    void* mappedData{ nullptr };
    throwIfFailed(dstResource->Map(0, nullptr, &mappedData));
    uint8_t* dst = reinterpret_cast<uint8_t*>(mappedData);
    const uint8_t* src = pData;
    for(uint32_t i = 0; i < height; i++)
        memcpy(
            dst + i * footprint.Footprint.RowPitch,
            src + i * width * 4,
            width * 4
        );
    dstResource->Unmap(0, nullptr);
}

void recordUploadToDefaultResource(
    ID3D12GraphicsCommandList *pCmdList,
    const ComPtr<ID3D12Resource> &srcResource,
    const ComPtr<ID3D12Resource> &dstResource,
    size_t byteSize,
    size_t srcOffset, size_t dstOffset)
{
    pCmdList->CopyBufferRegion(
        dstResource.Get(), dstOffset,
        srcResource.Get(), srcOffset,
        byteSize
    );
}

void recordUploadToDefaultResource2D(
    ID3D12GraphicsCommandList *pCmdList,
    const ComPtr<ID3D12Resource> &srcResource,
    const ComPtr<ID3D12Resource> &dstResource,
    size_t byteSize,
    size_t srcOffset, size_t dstOffset)
{
    D3D12_RESOURCE_DESC dstDesc = dstResource->GetDesc();
    dstDesc.Alignment = 0;

    uint64_t uploadBufferSize;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
    uint32_t numRows;
    uint64_t rowSizeInBytes;
    uint64_t totalBytes;

    ComPtr<ID3D12Device14> device = Dx12Core::getDevice();
    device->GetCopyableFootprints(
        &dstDesc,
        0, 1, 0,
        &footprint, &numRows,
        &rowSizeInBytes, &totalBytes
    );

    D3D12_TEXTURE_COPY_LOCATION locationSrc;
    locationSrc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    locationSrc.pResource = srcResource.Get();
    locationSrc.PlacedFootprint = footprint;
    locationSrc.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION locationDst;
    locationDst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    locationDst.pResource = dstResource.Get();
    locationDst.SubresourceIndex = 0;

    pCmdList->CopyTextureRegion(
        &locationDst, 0, 0, 0,
        &locationSrc,
        nullptr
    );
}

void recordTransitionResourceState(
    ID3D12GraphicsCommandList *pCmdList,
    const ComPtr<ID3D12Resource> &resource,
    D3D12_RESOURCE_STATES stateBefore,
    D3D12_RESOURCE_STATES stateAfter)
{
    sg::dxTypes::Resource_Barrier barrier(
        resource.Get(),
        stateBefore,
        stateAfter
    );
    pCmdList->ResourceBarrier(1, &barrier);
}
