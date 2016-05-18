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

#ifndef __ICOMSTREAM_H_
#define __ICOMSTREAM_H_
#pragma once

#include "IComPointer.h"				// Include IComPointer declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// Interface IComStream (internal)
//
// IComStream is an identical interface to the COM IStream interface.  This
// is used to allow multiple COM interfaces to be exposed from a managed class
//---------------------------------------------------------------------------

interface class IComStream : public IComPointer
{
	//-----------------------------------------------------------------------
	// Methods

	// Clone
	//
	// Creates a new stream object with its own seek pointer
	HRESULT Clone(IStream** ppStm);

	// Commit
	//
	// Ensures that any changes made to a stream object are persisted
	HRESULT Commit(DWORD grfCommitFlags);

	// CopyTo
	//
	// Copies a specified number of bytes into another stream
	HRESULT CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead,
		ULARGE_INTEGER* pcbWritten);

	// LockRegion
	//
	// Restricts access to a specified range of bytes in the stream
	HRESULT LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);

	// Read
	//
	// Reads a specified number of bytes from the stream 
	HRESULT Read(void* pv, ULONG cb, ULONG* pcbRead);

	// Revert
	//
	// Discards all changes that have been made to a transacted stream 
	HRESULT Revert(void);

	// Seek
	//
	// Changes the seek pointer to a new location
	HRESULT Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition);

	// SetSize
	//
	// Changes the size of the stream object
	HRESULT SetSize(ULARGE_INTEGER libNewSize);

	// Stat
	//
	// Retrieves the STATSTG structure for this stream
	HRESULT Stat(::STATSTG* pstatstg, DWORD grfStatFlag);

	// UnlockRegion
	//
	// Removes the access restriction on a range of bytes previously restricted
	HRESULT UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);

	// Write
	//
	// Writes a specified number of bytes into the stream object
	HRESULT Write(void const* pv, ULONG cb, ULONG* pcbWritten);
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif	// __ICOMSTREAM_H_
