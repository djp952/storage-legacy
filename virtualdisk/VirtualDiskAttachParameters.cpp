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
#include "VirtualDiskAttachParameters.h"	// Include VirtualDiskAttachParameters

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// VirtualDiskAttachParameters::Construct (private)
//
// Class pseudo-constructor; initializes all member variables to default values

void VirtualDiskAttachParameters::Construct(void)
{
	m_readOnly = false;
	m_noDriveLetter = false;
	m_autoDetach = true;
}

//---------------------------------------------------------------------------
// VirtualDiskAttachParameters::FreeATTACH_VIRTUAL_DISK_PARAMETERS (static)
//
// Releases structure created by ToOPEN_VIRTUAL_DISK_PARAMETERS
//
// Arguments:
//
//	pParams			- Parameter structure with embedded pointers

void VirtualDiskAttachParameters::FreeATTACH_VIRTUAL_DISK_PARAMETERS(PATTACH_VIRTUAL_DISK_PARAMETERS pParams)
{
	// There are currently no embedded pointers to chase in this structure
	delete pParams;
}

//---------------------------------------------------------------------------
// VirtualDiskAttachParameters::ToATTACH_VIRTUAL_DISK_FLAG
//
// Converts the state of this object into an unmanaged ATTACH_VIRTUAL_DISK_FLAG
//
// Arguments:
//
//	NONE

ATTACH_VIRTUAL_DISK_FLAG VirtualDiskAttachParameters::ToATTACH_VIRTUAL_DISK_FLAG(void)
{
	ATTACH_VIRTUAL_DISK_FLAG result = ATTACH_VIRTUAL_DISK_FLAG_NONE;

	if(m_readOnly) result |= ATTACH_VIRTUAL_DISK_FLAG_READ_ONLY;
	if(m_noDriveLetter) result |= ATTACH_VIRTUAL_DISK_FLAG_NO_DRIVE_LETTER;
	if(!m_autoDetach) result |= ATTACH_VIRTUAL_DISK_FLAG_PERMANENT_LIFETIME;

	return result;
}

//---------------------------------------------------------------------------
// VirtualDiskAttachParameters::ToATTACH_VIRTUAL_DISK_PARAMETERS (internal)
//
// Converts the state of this object into an unmanaged ATTACH_VIRTUAL_DISK_PARAMETERS
//
// Arguments:
//
//	ppParams		- Pointer to receive an unmanaged OPEN_VIRTUAL_DISK_PARAMETERS structure

ATTACH_VIRTUAL_DISK_VERSION VirtualDiskAttachParameters::ToATTACH_VIRTUAL_DISK_PARAMETERS(PATTACH_VIRTUAL_DISK_PARAMETERS* ppParams)
{
	PATTACH_VIRTUAL_DISK_PARAMETERS		pParams;					// Structure pointer

	if(!ppParams) throw gcnew ArgumentNullException("pParams");

	*ppParams = new ATTACH_VIRTUAL_DISK_PARAMETERS;
	if(!(*ppParams)) throw gcnew OutOfMemoryException();
	pParams = (*ppParams);

	// Initialize the ATTACH_VIRTUAL_DISK_PARAMETERS
	ZeroMemory(pParams, sizeof(ATTACH_VIRTUAL_DISK_PARAMETERS));
	pParams->Version = ATTACH_VIRTUAL_DISK_VERSION_1;

	// There are currently no fields to set in ATTACH_VIRTUAL_DISK_PARAMETERS

	return pParams->Version;
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
