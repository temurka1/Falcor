/***************************************************************************
# Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#pragma once

namespace Falcor
{
    class D3D12DescriptorHeap : public std::enable_shared_from_this<D3D12DescriptorHeap>
    {
    public:
        using SharedPtr = std::shared_ptr<D3D12DescriptorHeap>;
        using SharedConstPtr = std::shared_ptr<const D3D12DescriptorHeap>;
        using ApiHandle = DescriptorHeapHandle;
        using CpuHandle = HeapCpuHandle;
        using GpuHandle = HeapGpuHandle;

        ~D3D12DescriptorHeap();
        static SharedPtr create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorsCount, bool shaderVisible = true);

        class AllocationEntry
        {
        public:
            using SharedPtr = std::shared_ptr<AllocationEntry>;
            SharedPtr create(D3D12DescriptorHeap::SharedPtr pHeap, uint32_t baseIndex, uint32_t descCount);
            ~AllocationEntry();

            CpuHandle getCpuHandle(uint32_t index); // Index is relative to the allocation
            GpuHandle getGpuHandle(uint32_t index);
            
        private:
            AllocationEntry(D3D12DescriptorHeap::SharedPtr pHeap, uint32_t baseIndex, uint32_t descCount);
            D3D12DescriptorHeap::SharedPtr mpHeap;
            uint32_t mBaseIndex;
            uint32_t mDescCount;
        };
        
        AllocationEntry::SharedPtr allocateEntries(uint32_t descCount);
        ApiHandle getApiHandle() const { return mApiHandle; }
        D3D12_DESCRIPTOR_HEAP_TYPE getType() const { return mType; }

        uint32_t getReservedDescCount() const { return mCount; }
        CpuHandle getCpuHandle(uint32_t index) const;
        GpuHandle getGpuHandle(uint32_t index) const;
        uint32_t getDescriptorSize() const { return mDescriptorSize; }
    private:
        D3D12DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorsCount);

        CpuHandle mCpuHeapStart = {};
        GpuHandle mGpuHeapStart = {};
        uint32_t mDescriptorSize;
        uint32_t mCount;
        uint32_t mCurDesc = 0;
        ApiHandle mApiHandle;
        D3D12_DESCRIPTOR_HEAP_TYPE mType;
    };
}
