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

#include "stdafx.h"						// Include project pre-compiled headers
#include "VirtualDiskOpenParameters.h"	// Include VirtualDiskOpenParameters declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// VirtualDiskOpenParameters::Construct (private)
//
// Class pseudo-constructor; initializes all member variables to default values

void VirtualDiskOpenParameters::Construct(String^ path, VirtualDiskStorageType type)
{
	m_path = path;
	m_storageType = type;
	m_getInfoOnly = false;
	m_readOnly = false;
	m_noParents = false;
	m_customDiffChain = false;
	m_resiliencyGuid = Guid::Empty;
}

//---------------------------------------------------------------------------
// VirtualDiskOpenParameters::FreeOPEN_VIRTUAL_DISK_PARAMETERS (static)
//
// Releases structure created by ToOPEN_VIRTUAL_DISK_PARAMETERS
//
// Arguments:
//
//	pParams			- Parameter structure with embedded pointers

void VirtualDiskOpenParameters::FreeOPEN_VIRTUAL_DISK_PARAMETERS(POPEN_VIRTUAL_DISK_PARAMETERS pParams)
{
	// There are currently no embedded pointers to chase in this structure
	delete pParams;
}

//---------------------------------------------------------------------------
// VirtualDiskOpenParameters::ToOPEN_VIRTUAL_DISK_FLAG
//
// Converts the state of this object into an unmanaged OPEN_VIRTUAL_DISK_FLAG
//
// Arguments:
//
//	NONE

OPEN_VIRTUAL_DISK_FLAG VirtualDiskOpenParameters::ToOPEN_VIRTUAL_DISK_FLAG(void)
{
	OPEN_VIRTUAL_DISK_FLAG result = OPEN_VIRTUAL_DISK_FLAG_NONE;

	if(m_noParents) result |= OPEN_VIRTUAL_DISK_FLAG_NO_PARENTS;
	if(m_cachedIO) result |= OPEN_VIRTUAL_DISK_FLAG_CACHED_IO;
	if(m_customDiffChain) result |= OPEN_VIRTUAL_DISK_FLAG_CUSTOM_DIFF_CHAIN;

	return result;
}

//---------------------------------------------------------------------------
// VirtualDiskOpenParameters::ToOPEN_VIRTUAL_DISK_PARAMETERS (internal)
//
// Converts the state of this object into an unmanaged OPEN_VIRTUAL_DISK_PARAMETERS
//
// Arguments:
//
//	ppParams		- Pointer to receive an unmanaged OPEN_VIRTUAL_DISK_PARAMETERS structure

OPEN_VIRTUAL_DISK_VERSION VirtualDiskOpenParameters::ToOPEN_VIRTUAL_DISK_PARAMETERS(POPEN_VIRTUAL_DISK_PARAMETERS* ppParams)
{
	POPEN_VIRTUAL_DISK_PARAMETERS		pParams;					// Structure pointer
	PinnedBytePtr						pinResiliencyGuid;			// Pinned GUID

	if(!ppParams) throw gcnew ArgumentNullException("pParams");

	*ppParams = new OPEN_VIRTUAL_DISK_PARAMETERS;
	if(!(*ppParams)) throw gcnew OutOfMemoryException();
	pParams = (*ppParams);

	// Pin the resiliency GUID structure
	array<Byte>^ resiliencyGuid = m_resiliencyGuid.ToByteArray();
	pinResiliencyGuid = &(resiliencyGuid[0]);

	// Initialize the OPEN_VIRTUAL_DISK_PARAMETERS
	ZeroMemory(pParams, sizeof(OPEN_VIRTUAL_DISK_PARAMETERS));
	pParams->Version = OPEN_VIRTUAL_DISK_VERSION_2;
	pParams->Version2.GetInfoOnly = (m_getInfoOnly) ? TRUE : FALSE;
	pParams->Version2.ReadOnly = (m_readOnly) ? TRUE : FALSE;
	memcpy(&pParams->Version2.ResiliencyGuid, pinResiliencyGuid, sizeof(_GUID));

	return pParams->Version;
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
