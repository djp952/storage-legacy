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
#include "VirtualDiskSafeHandle.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// VirtualDiskSafeHandle Constructor
//
// Arguments:
//
//	handle			- Virtual disk HANDLE instance

VirtualDiskSafeHandle::VirtualDiskSafeHandle(HANDLE handle) : SafeHandleZeroOrMinusOneIsInvalid(true)
{
	if(handle == __nullptr) throw gcnew ArgumentNullException("handle");
	SetHandle(IntPtr(handle));
}

//---------------------------------------------------------------------------
// VirtualDiskSafeHandle::ReleaseHandle
//
// Releases the unmanaged handle instance

bool VirtualDiskSafeHandle::ReleaseHandle(void)
{
	return (CloseHandle(handle.ToPointer()) != FALSE);
}

//---------------------------------------------------------------------------
// VirtualDiskSafeHandle::Reference Constructor
//
// Arguments:
//
//	handle		- VirtualDiskSafeHandle to reference

VirtualDiskSafeHandle::Reference::Reference(VirtualDiskSafeHandle^ handle) : m_handle(handle)
{
	if(Object::ReferenceEquals(handle, nullptr)) throw gcnew ArgumentNullException("handle");
			
	m_handle->DangerousAddRef(m_release);
	if(!m_release) throw gcnew ObjectDisposedException(VirtualDiskSafeHandle::typeid->Name);
}

//---------------------------------------------------------------------------
// VirtualDiskSafeHandle::Reference Destructor

VirtualDiskSafeHandle::Reference::~Reference()
{
	if(m_release) m_handle->DangerousRelease();
}

//---------------------------------------------------------------------------
// VirtualDiskSafeHandle::Reference HANDLE conversion operator

VirtualDiskSafeHandle::Reference::operator HANDLE()
{
	return m_handle->DangerousGetHandle().ToPointer();
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
