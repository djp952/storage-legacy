//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------

#include "stdafx.h"					// Include project pre-compiled headers
#include "StorageUtil.h"			// Include StorageUtil declarations
#include "StorageContainer.h"		// Include StorageContainer decls
#include "StorageObject.h"			// Include StorageObject declarations

using namespace System;

#pragma warning(push, 4)			// Enable maximum compiler warnings

BEGIN_NAMESPACE(zuki)
BEGIN_NAMESPACE(storage)
BEGIN_NAMESPACE(structured)

//---------------------------------------------------------------------------
// StorageUtil::Base64ToSysGuid
//
// Converts a System::Guid that has been converted into a base64 encoded
// string back into a System::Guid again.  CAUTION: All base64 encoded
// strings are CASE SENSITIVE, since they use both upper and lower case
//
// Arguments:
//
//	base64		- The base64 encoded string to be converted

Guid StorageUtil::Base64ToSysGuid(String^ base64)
{
	base64 = base64->Replace('_', '/');		// Underbar back to slash

	// Conversion errors here could be really bad in other places.  Safest
	// thing to do is catch them and return an empty GUID instead

	try { return Guid(Convert::FromBase64String(base64)); }
	catch(Exception^) { return Guid::Empty; }
}

//---------------------------------------------------------------------------
// StorageUtil::CompareUUIDs
//
// Compares two GUIDs to determine their equality or inequality
//
// Arguments :
//
//	lhs			- Left hand side GUID argument
//	rhs			- Right hand side GUID argument

int StorageUtil::CompareUUIDs(const UUID &lhs, const UUID &rhs)
{
	// Ths first three parts of the GUIDs can be compared directly

	if(lhs.Data1 != rhs.Data1) return (lhs.Data1 < rhs.Data1) ? -1 : 1;
	if(lhs.Data2 != rhs.Data2) return (lhs.Data2 < rhs.Data2) ? -1 : 1;
	if(lhs.Data3 != rhs.Data3) return (lhs.Data3 < rhs.Data3) ? -1 : 1;

	// The first three parts are equal.  In order to check the final part
	// of the GUIDs, we need to iterate through all 8 bytes in order

	int i;
	for(i = 0; (i < 8) && (lhs.Data4[i] == rhs.Data4[i]); i++);

	if(i == 8) return 0;
	else return (lhs.Data4[i] < rhs.Data4[i]) ? -1 : 1;
}

//---------------------------------------------------------------------------
// StorageUtil::GetContainerID (static)
//
// Retrieves the GUID that uniquely defines a specific container.  Every
// container's name is actually a BASE64 encoded Guid
//
// Arguments:
//
//	storage		- IComStorage reference to get the GUID for

Guid StorageUtil::GetContainerID(IComStorage^ storage)
{
	::STATSTG		stats;				// Structure returned from Stat()
	HRESULT			hResult;			// Result from function call

	if(storage == nullptr) throw gcnew ArgumentNullException();

	// Retrieve the STASTG information about this IStorage object,
	// including the name, since that's what we're really after here

	hResult = storage->Stat(&stats, STATFLAG_DEFAULT);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Convert the BASE64 encoded string back into a GUID and return it

	try { return Base64ToSysGuid(gcnew String(stats.pwcsName)); }
	finally { CoTaskMemFree(stats.pwcsName); }
}

//---------------------------------------------------------------------------
// StorageUtil::GetObjectID
//
// Retrieves the GUID associated with a stream
//
// Arguments:
//
//	stream			- IComStream reference to get the GUID from

Guid StorageUtil::GetObjectID(IComStream^ stream)
{
	::STATSTG		stats;				// Structure returned from Stat()
	HRESULT			hResult;			// Result from function call

	// Retrieve the STASTG information about this Stream object,
	// including the name, since that's what we're really after here

	hResult = stream->Stat(&stats, STATFLAG_DEFAULT);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Convert the BASE64 encoded string back into a GUID and return it

	try { return Base64ToSysGuid(gcnew String(stats.pwcsName)); }
	finally { CoTaskMemFree(stats.pwcsName); }
}

//---------------------------------------------------------------------------
// StorageUtil::GetPropertySetID
//
// Retrieves the FMTID associated with a property set
//
// Arguments:
//
//	propStorage		- IComPropertyStorage object reference

Guid StorageUtil::GetPropertySetID(IComPropertyStorage^ propStorage)
{
	STATPROPSETSTG		stats;			// Structure returned from Stat()
	HRESULT				hResult;		// Result from function call

	// Retrieve the STATPROPSETSTG information about this property set

	hResult = propStorage->Stat(&stats);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	return UUIDToSysGuid(stats.fmtid);		// Return the FMTID
}

//---------------------------------------------------------------------------
// StorageUtil::GetStorageChildOpenMode
//
// Generates a set of "open mode" flags based on an existing IStorage object.
// When opening up sub storages or streams, you always have to include the
// STGM_SHARE_EXCLUSIVE flag for whatever genius reason they came up with
//
// Arguments:
//
//	storage			- IComStorage instance to be accessed

DWORD StorageUtil::GetStorageChildOpenMode(IComStorage^ storage)
{
	::STATSTG		stats;				// Structure returned from Stat()
	HRESULT			hResult;			// Result from function call

	// Retrieve the STASTG information about this Storage object, omitting
	// the pcwsName field, so we don't have to bother releasing the memory

	hResult = storage->Stat(&stats, STATFLAG_NONAME);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	return ((stats.grfMode & 0xF) | STGM_SHARE_EXCLUSIVE);
}

//---------------------------------------------------------------------------
// StorageUtil::GetStorageMode
//
// Retrieves the mode flags associated with an IStorage interface
//
// Arguments:
//
//	pStorage		- Pointer to the IStorage interface

DWORD StorageUtil::GetStorageMode(IComStorage^ storage)
{
	::STATSTG		stats;				// Structure returned from Stat()
	HRESULT			hResult;			// Result from function call

	// Retrieve the STASTG information about this Storage object, omitting
	// the pcwsName field, so we don't have to bother releasing the memory

	hResult = storage->Stat(&stats, STATFLAG_NONAME);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	return stats.grfMode;				// Copy back the mode flags
}

//---------------------------------------------------------------------------
// StorageUtil::GetStreamMode
//
// Retrieves the mode flags associated with an IStream interface
//
// Arguments:
//
//	stream			- IComStream reference to get the flag from

DWORD StorageUtil::GetStreamMode(IComStream^ stream)
{
	::STATSTG		stats;				// Structure returned from Stat()
	HRESULT			hResult;			// Result from function call

	// Retrieve the STASTG information about this Stream object, omitting
	// the pcwsName field, so we don't have to bother releasing the memory

	hResult = stream->Stat(&stats, STATFLAG_NONAME);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	return stats.grfMode;				// Copy back the mode flags
}

//---------------------------------------------------------------------------
// StorageUtil::IsStorageReadOnly
//
// Determines if an IStorage-based object is set to read only or not
//
// Arguments:
//
//	storage		- IComStorage reference to get flags from

bool StorageUtil::IsStorageReadOnly(IComStorage^ storage)
{
	return ((GetStorageMode(storage) & 0xF) == STGM_READ);
}

//---------------------------------------------------------------------------
// StorageUtil::IsStreamReadOnly
//
// Determines if an IStream-based object is set to read only or not
//
// Arguments:
//
//	stream		- IComStream reference to check status of

bool StorageUtil::IsStreamReadOnly(IComStream^ stream)
{
	return ((GetStreamMode(stream) & 0xF) == STGM_READ);
}

//---------------------------------------------------------------------------
// StorageUtil::SysGuidToBase64
//
// Converts a System::Guid structure into a base64 encoded string in order
// to minimize the storage space necessary.  CAUTION: All base64 encoded
// strings are CASE SENSITIVE, since they use both upper and lower case
//
// Arguments:
//
//	guid		- The System::Guid structure to be converted

String^ StorageUtil::SysGuidToBase64(System::Guid guid)
{
	String^ base64 = Convert::ToBase64String(guid.ToByteArray());
	return base64->Replace('/', '_');
}

//---------------------------------------------------------------------------
// StorageUtil::SysGuidToUUID
//
// Converts a managed System::Guid structure into a standard UUID structure
// (Found this somewhere on MSDN)
//
// Arguments:
//
//	guid		- The managed System::Guid to be converted

UUID StorageUtil::SysGuidToUUID(Guid guid)
{
	array<Byte>^ guidData = guid.ToByteArray();
	PinnedBytePtr data = &(guidData[0]);
	return *reinterpret_cast<UUID*>(data);
}

//---------------------------------------------------------------------------
// StorageUtil::UUIDToSysGuid
//
// Converts an unmanaged UUID structure into a managed System::Guid structure
// (Found this on MSDN as well, surprisingly enough)
//
// Arguments:
//
//	guid		- The unmanaged UUID structure to be converted

Guid StorageUtil::UUIDToSysGuid(const UUID& guid)
{
   return Guid(guid.Data1, guid.Data2, guid.Data3, guid.Data4[0],
	   guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5],
	   guid.Data4[6], guid.Data4[7]);
}

//---------------------------------------------------------------------------

END_NAMESPACE(structured)
END_NAMESPACE(storage)
END_NAMESPACE(zuki)

#pragma warning(pop)
