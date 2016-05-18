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
#include "VirtualDiskStorageType.h"		// Include VirtualDiskStorageType declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//-----------------------------------------------------------------------------
// VirtualDiskStorageType::FromVIRTUAL_STORAGE_TYPE (internal, static)
//
// Converts a VIRTUAL_STORAGE_TYPE into one of the valid VirtualStorageType(s)
//
// Arguments:
//
//	ptype		- Pointer to a VIRTUAL_STORAGE_TYPE structure

VirtualDiskStorageType VirtualDiskStorageType::FromVIRTUAL_STORAGE_TYPE(PVIRTUAL_STORAGE_TYPE ptype)
{
	if(!ptype) return VirtualDiskStorageType::Auto;

	// Use the DEVICEID to determine a VirtualDiskStorageType to return
	switch(ptype->DeviceId) {

		case VIRTUAL_STORAGE_TYPE_DEVICE_ISO: return VirtualDiskStorageType::ISO;
		case VIRTUAL_STORAGE_TYPE_DEVICE_VHD: return VirtualDiskStorageType::VHD;
		case VIRTUAL_STORAGE_TYPE_DEVICE_VHDX: return VirtualDiskStorageType::VHDX;
	}
	
	// Anything unrecognized --> AUTO
	return VirtualDiskStorageType::Auto;
}

//-----------------------------------------------------------------------------
// VirtualDiskStorageType::ToVIRTUAL_STORAGE_TYPE (internal)
//
// Converts the value of this class into an unmanaged VIRTUAL_STORAGE_TYPE
//
// Arguments:
//
//	ptype		- Pointer to a VIRTUAL_STORAGE_TYPE structure

void VirtualDiskStorageType::ToVIRTUAL_STORAGE_TYPE(PVIRTUAL_STORAGE_TYPE ptype)
{
	if(ptype == NULL) throw gcnew ArgumentNullException("ptype");

	ptype->DeviceId = static_cast<ULONG>(m_deviceId);
	
	// Use VENDOR_UNKNOWN along with DEVICE_UNKNOWN, otherwise VENDOR_MICROSOFT
	if(m_deviceId == VIRTUAL_STORAGE_TYPE_DEVICE_UNKNOWN) ptype->VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_UNKNOWN;
	else ptype->VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT;
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
