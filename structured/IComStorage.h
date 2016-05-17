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

#ifndef __ICOMSTORAGE_H_
#define __ICOMSTORAGE_H_
#pragma once

#include "IComPointer.h"				// Include IComPointer declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_NAMESPACE(zuki)
BEGIN_NAMESPACE(storage)
BEGIN_NAMESPACE(structured)

//---------------------------------------------------------------------------
// Interface IComStorage (internal)
//
// IComStorage is an identical interface to the COM IStorage interface.  This
// is used to allow multiple COM interfaces to be exposed from a managed class
//---------------------------------------------------------------------------

interface class IComStorage : public IComPointer
{
	//-----------------------------------------------------------------------
	// Methods

	// Commit
	//
	// Ensures any changes to the storage object are persisted to disk
	HRESULT Commit(DWORD grfCommitFlags);

	// CopyTo
	//
	// Copies the entire contents of an open storage object to another object
	HRESULT CopyTo(DWORD ciidExclude, IID const* rgiidExclude, SNB snbExclude,
	  IStorage* pstgDest);

	// CreateStorage
	//
	// Creates and opens a new storage object nested within this storage object
	HRESULT CreateStorage(const WCHAR* pwcsName, DWORD grfMode, DWORD reserved1,
		DWORD reserved2, IStorage** ppstg);

	// CreateStream
	//
	// Creates and opens a stream object contained in this storage object
	HRESULT CreateStream(const WCHAR* pwcsName, DWORD grfMode, DWORD reserved1,
		DWORD reserved2, IStream** ppstm);

	// DestroyElement
	//
	// Removes the specified storage or stream from this storage object
	HRESULT DestroyElement(const WCHAR* pwcsName);

	// EnumElements
	//
	// Enumerates the storage and stream objects contained within this storage
	HRESULT EnumElements(DWORD reserved1, void* reserved2, DWORD reserved3, IEnumSTATSTG** ppenum);

	// MoveElementTo
	//
	// Copies or moves a substorage or stream from this storage object
	HRESULT MoveElementTo(const WCHAR* pwcsName, IStorage* pstgDest, LPWSTR pwcsNewName,
		DWORD grfFlags);

	// OpenStorage
	//
	// Opens an existing storage object with the specified name
	HRESULT OpenStorage(const WCHAR* pwcsName, IStorage* pstgPriority, DWORD grfMode, 
		SNB snbExclude, DWORD reserved, IStorage** ppstg);

	// OpenStream
	//
	// Opens an existing stream object with the specified name
	HRESULT OpenStream(const WCHAR* pwcsName, void* reserved1, DWORD grfMode, 
		DWORD reserved2, IStream** ppstm);

	// RenameElement
	//
	// Renames the specified substorage or stream in this storage object
	HRESULT RenameElement(const WCHAR* pwcsOldName, const WCHAR* pwcsNewName);

	// Revert
	//
	// Discards all changes that have been made to the storage object
	HRESULT Revert(void);

	// SetClass
	//
	// Assigns the specified class identifier (CLSID) to this storage object.
	HRESULT SetClass(REFCLSID clsid);

	// SetElementTimes
	//
	// Sets the modification, access, and creation times of the specified storage element
	HRESULT SetElementTimes(const WCHAR* pwcsName, ::FILETIME const* pctime, 
		::FILETIME const* patime, ::FILETIME const* pmtime);

	// SetStateBits
	//
	// Stores up to 32 bits of state information in this storage object
	HRESULT SetStateBits(DWORD grfStateBits, DWORD grfMask);

	// Stat
	//
	// Retrieves the STATSTG structure for this open storage object
	HRESULT Stat(::STATSTG* pstatstg, DWORD grfStatFlag);
};

//---------------------------------------------------------------------------

END_NAMESPACE(structured)
END_NAMESPACE(storage)
END_NAMESPACE(zuki)

#pragma warning(pop)

#endif	// __ICOMSTORAGE_H_
