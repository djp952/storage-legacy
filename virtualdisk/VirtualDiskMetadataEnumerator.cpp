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

#include "stdafx.h"
#include "VirtualDiskMetadataEnumerator.h"

#include "GuidUtil.h"
#include "VirtualDiskSafeHandle.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::tools::llvm::clang)

//---------------------------------------------------------------------------
// VirtualDiskMetadataEnumerator Constructor
//
// Arguments:
//
//	handle		- VirtualDiskSafeHandle instance for this enumerator\
//	keys		- Collection of metadata keys to be enumerated

VirtualDiskMetadataEnumerator::VirtualDiskMetadataEnumerator(VirtualDiskSafeHandle^ handle, IList<Guid>^ keys) : m_handle(handle), m_keys(keys)
{
	if(Object::ReferenceEquals(handle, nullptr)) throw gcnew ArgumentNullException("handle");
	if(Object::ReferenceEquals(keys, nullptr)) throw gcnew ArgumentNullException("keys");
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataEnumerator Destructor

VirtualDiskMetadataEnumerator::~VirtualDiskMetadataEnumerator()
{
	if(m_disposed) return;

	delete m_keys;
	m_disposed = true;
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataEnumerator::Current::get
//
// Gets the current element in the collection

KeyValuePair<Guid, array<Byte>^> VirtualDiskMetadataEnumerator::Current::get(void)
{
	ULONG cb = 0;

	CHECK_DISPOSED(m_disposed);

	if((m_index < 0) || (m_index >= m_keys->Count)) throw gcnew InvalidOperationException();

	// Unwrap the safe handle and convert the System::Guid into a GUID
	VirtualDiskSafeHandle::Reference handle(m_handle);
	GUID guid = GuidUtil::SysGuidToUUID(m_keys[m_index]);				

	// Get the size of the metadata blob associated with this GUID, if zero return a zero length array
	DWORD result = GetVirtualDiskMetadata(handle, &guid, &cb, __nullptr);
	if(result == ERROR_SUCCESS) return KeyValuePair<Guid, array<Byte>^>(m_keys[m_index], gcnew array<Byte>(0));
	if(result != ERROR_MORE_DATA) throw gcnew Win32Exception(result);

	// Allocate and pin the managed array to be returned to the caller
	array<Byte>^ blob = gcnew array<Byte>(cb);
	pin_ptr<uint8_t> pinblob = &blob[0];

	// Call the method again, this time loading the blob data into the array
	result = GetVirtualDiskMetadata(handle, &guid, &cb, pinblob);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception();

	return KeyValuePair<Guid, array<Byte>^>(m_keys[m_index], blob);
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataEnumerator::IEnumerator_Current::get
//
// Gets the current element in the collection as an untyped Object^

Object^ VirtualDiskMetadataEnumerator::IEnumerator_Current::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return Current::get();
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataEnumerator::MoveNext
//
// Advances the enumerator to the next element of the collection
//
// Arguments:
//
//	NONE

bool VirtualDiskMetadataEnumerator::MoveNext(void)
{
	CHECK_DISPOSED(m_disposed);
	return (++m_index < m_keys->Count);
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataEnumerator::Reset
//
// Sets the enumerator to its initial position
//
// Arguments:
//
//	NONE

void VirtualDiskMetadataEnumerator::Reset(void)
{
	CHECK_DISPOSED(m_disposed);
	m_index = -1;
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::tools::llvm::clang)

#pragma warning(pop)
