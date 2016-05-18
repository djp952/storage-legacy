//-----------------------------------------------------------------------------
// Copyright (c) 2016 Michael G. Brehm
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//-----------------------------------------------------------------------------

#include "stdafx.h"							// Include project pre-compiled headers
#include "VirtualDiskCreateParameters.h"	// Include VirtualDiskCreateParameters decls

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// VirtualDiskCreateParameters::Construct (private)
//
// Class pseudo-constructor; initializes all member variables to default values

void VirtualDiskCreateParameters::Construct(String^ path, VirtualDiskStorageType type, __int64 maximumSize)
{
	if(maximumSize < 0) throw gcnew ArgumentOutOfRangeException("maximumSize");
	
	m_path = path;
	m_storageType = type;
	m_uniqueIdentifier = Guid::NewGuid();
	m_maximumSize = maximumSize;
	m_blockSize = CREATE_VIRTUAL_DISK_PARAMETERS_DEFAULT_BLOCK_SIZE;
	m_sectorSize = CREATE_VIRTUAL_DISK_PARAMETERS_DEFAULT_SECTOR_SIZE;
	m_preAllocate = false;
	m_cachedIO = false;
	m_resiliencyGuid = Guid::Empty;

	m_parentParams = gcnew VirtualDiskParentDiskParameters();
	m_sourceParams = gcnew VirtualDiskSourceDiskParameters();
}

//---------------------------------------------------------------------------
// VirtualDiskCreateParameters::FreeCREATE_VIRTUAL_DISK_PARAMETERS (static)
//
// Releases structure created by ToCREATE_VIRTUAL_DISK_PARAMETERS
//
// Arguments:
//
//	pParams			- Parameter structure with embedded pointers

void VirtualDiskCreateParameters::FreeCREATE_VIRTUAL_DISK_PARAMETERS(PCREATE_VIRTUAL_DISK_PARAMETERS pParams)
{
	VirtualDiskUtil::FreePWSTR(pParams->Version2.ParentPath);
	VirtualDiskUtil::FreePWSTR(pParams->Version2.SourcePath);
	delete pParams;
}

//---------------------------------------------------------------------------
// VirtualDiskCreateParameters::ToCREATE_VIRTUAL_DISK_FLAG
//
// Converts the state of this object into an unmanaged CREATE_VIRTUAL_DISK_FLAG
//
// Arguments:
//
//	NONE

CREATE_VIRTUAL_DISK_FLAG VirtualDiskCreateParameters::ToCREATE_VIRTUAL_DISK_FLAG(void)
{
	CREATE_VIRTUAL_DISK_FLAG result = CREATE_VIRTUAL_DISK_FLAG_NONE;

	if(m_preAllocate) result |= CREATE_VIRTUAL_DISK_FLAG_FULL_PHYSICAL_ALLOCATION;
	if(!m_parentParams->CopyMetadata) result |= CREATE_VIRTUAL_DISK_FLAG_DO_NOT_COPY_METADATA_FROM_PARENT;
	if(m_sourceParams->PreventWrites) result |= CREATE_VIRTUAL_DISK_FLAG_PREVENT_WRITES_TO_SOURCE_DISK;

	return result;
}

//---------------------------------------------------------------------------
// VirtualDiskCreateParameters::ToCREATE_VIRTUAL_DISK_PARAMETERS (internal)
//
// Converts the state of this object into an unmanaged CREATE_VIRTUAL_DISK_PARAMETERS
// structure.  Embedded string pointers must be released with Marshal::FreeHGlobal()
//
// Arguments:
//
//	ppParams		- Pointer to receive an unmanaged CREATE_VIRTUAL_DISK_PARAMETERS structure

CREATE_VIRTUAL_DISK_VERSION VirtualDiskCreateParameters::ToCREATE_VIRTUAL_DISK_PARAMETERS(PCREATE_VIRTUAL_DISK_PARAMETERS* ppParams)
{
	PCREATE_VIRTUAL_DISK_PARAMETERS		pParams;					// Structure pointer
	PinnedBytePtr						pinUniqueIdentifier;		// Pinned GUID
	PinnedBytePtr						pinResiliencyGuid;			// Pinned GUID

	if(!ppParams) throw gcnew ArgumentNullException("pParams");

	*ppParams = new CREATE_VIRTUAL_DISK_PARAMETERS;
	if(!(*ppParams)) throw gcnew OutOfMemoryException();
	pParams = (*ppParams);

	// Pin the unique identifier GUID structure
	array<Byte>^ uniqueIdentifier = m_uniqueIdentifier.ToByteArray();
	pinUniqueIdentifier = &(uniqueIdentifier[0]);
	
	// Pin the resiliency GUID structure
	array<Byte>^ resiliencyGuid = m_resiliencyGuid.ToByteArray();
	pinResiliencyGuid = &(resiliencyGuid[0]);

	// Initialize the CREATE_VIRTUAL_DISK_PARAMETERS
	ZeroMemory(pParams, sizeof(CREATE_VIRTUAL_DISK_PARAMETERS));
	pParams->Version = CREATE_VIRTUAL_DISK_VERSION_2;
	memcpy(&pParams->Version2.UniqueId, pinUniqueIdentifier, sizeof(_GUID));
	pParams->Version2.MaximumSize = m_maximumSize;
	pParams->Version2.BlockSizeInBytes = m_blockSize;
	pParams->Version2.SectorSizeInBytes = m_sectorSize;
	if(!String::IsNullOrEmpty(m_parentParams->Path)) pParams->Version2.ParentPath = VirtualDiskUtil::StringToPWSTR(m_parentParams->Path);
	if(!String::IsNullOrEmpty(m_sourceParams->Path)) pParams->Version2.SourcePath = VirtualDiskUtil::StringToPWSTR(m_sourceParams->Path);
	pParams->Version2.OpenFlags = (m_cachedIO) ? OPEN_VIRTUAL_DISK_FLAG_CACHED_IO : OPEN_VIRTUAL_DISK_FLAG_NONE;
	m_parentParams->Type.ToVIRTUAL_STORAGE_TYPE(&pParams->Version2.ParentVirtualStorageType);
	m_sourceParams->Type.ToVIRTUAL_STORAGE_TYPE(&pParams->Version2.SourceVirtualStorageType);
	memcpy(&pParams->Version2.ResiliencyGuid, pinResiliencyGuid, sizeof(_GUID));

	return pParams->Version;
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
