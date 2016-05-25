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

#include "stdafx.h"
#include "VirtualDiskType.h"

#include "GuidUtil.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// VirtualDiskType Constructor
//
// Arguments:
//
//	deviceid	- Virtual disk device identifier
//	vendorid	- Virtual disk vendor identifier

VirtualDiskType::VirtualDiskType(unsigned int deviceid, Guid vendorid) : DeviceId(deviceid), VendorId(vendorid)
{
}

//---------------------------------------------------------------------------
// VirtualDiskType Constructor (internal)
//
// Arguments:
//
//	deviceid	- Virtual disk device identifier
//	vendorid	- Virtual disk vendor identifier

VirtualDiskType::VirtualDiskType(ULONG deviceid, const GUID& vendorid) : DeviceId(deviceid), VendorId(GuidUtil::UUIDToSysGuid(vendorid))
{
}

//---------------------------------------------------------------------------
// VirtualDiskType Constructor (internal)
//
// Arguments:
//
//	type		- VIRTUAL_STORAGE_TYPE instance reference

VirtualDiskType::VirtualDiskType(const VIRTUAL_STORAGE_TYPE& type) : DeviceId(type.DeviceId), VendorId(GuidUtil::UUIDToSysGuid(type.VendorId))
{
}

//---------------------------------------------------------------------------
// VirtualDiskType::operator == (static)

bool VirtualDiskType::operator==(VirtualDiskType lhs, VirtualDiskType rhs)
{
	return ((lhs.DeviceId == rhs.DeviceId) && (lhs.VendorId == rhs.VendorId));
}

//---------------------------------------------------------------------------
// VirtualDiskType::operator != (static)

bool VirtualDiskType::operator!=(VirtualDiskType lhs, VirtualDiskType rhs)
{
	return ((lhs.DeviceId != rhs.DeviceId) || (lhs.VendorId != rhs.VendorId));
}

//---------------------------------------------------------------------------
// VirtualDiskType::Equals
//
// Compares this VirtualDiskType to another VirtualDiskType
//
// Arguments:
//
//	rhs		- Right-hand VirtualDiskType to compare against

bool VirtualDiskType::Equals(VirtualDiskType rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// VirtualDiskType::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool VirtualDiskType::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a VirtualDiskType instance
	VirtualDiskType^ rhsref = dynamic_cast<VirtualDiskType^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// VirtualDiskType::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int VirtualDiskType::GetHashCode(void)
{
	// VendorId is initonly and has to be copied for this operation (C4395)
	return DeviceId.GetHashCode() ^ Guid(VendorId).GetHashCode();
}

//---------------------------------------------------------------------------
// VirtualDiskType::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ VirtualDiskType::ToString(void)
{
	switch(DeviceId) {

		case VIRTUAL_STORAGE_TYPE_DEVICE_ISO:		return gcnew String("ISO");
		case VIRTUAL_STORAGE_TYPE_DEVICE_VHD:		return gcnew String("VHD");
		case VIRTUAL_STORAGE_TYPE_DEVICE_VHDX:		return gcnew String("VHDX");
		case VIRTUAL_STORAGE_TYPE_DEVICE_VHDSET:	return gcnew String("VHDSet");

		default: return gcnew String("Unknown");
	}
}

//-----------------------------------------------------------------------------
// VirtualDiskType::ToVIRTUAL_STORAGE_TYPE (internal)
//
// Converts the value of this class into an unmanaged VIRTUAL_STORAGE_TYPE
//
// Arguments:
//
//	type		- Pointer to a VIRTUAL_STORAGE_TYPE structure

void VirtualDiskType::ToVIRTUAL_STORAGE_TYPE(PVIRTUAL_STORAGE_TYPE type)
{
	if(type == __nullptr) throw gcnew ArgumentNullException("type");

	type->DeviceId = static_cast<ULONG>(DeviceId);
	type->VendorId = GuidUtil::SysGuidToUUID(VendorId);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
