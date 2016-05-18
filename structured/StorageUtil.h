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

#ifndef __STORAGEUTIL_H_
#define __STORAGEUTIL_H_
#pragma once

#include "IComPropertyStorage.h"		// Include IComPropertyStorage decls
#include "IComStorage.h"				// Include IComStorage declarations
#include "IComStream.h"					// Include IComStream declarations
#include "StorageException.h"			// Include StorageException declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Forward Class Declarations
//---------------------------------------------------------------------------

ref class StorageContainer;				// StorageContainer.h
ref class StorageObject;				// StorageObject.h

//---------------------------------------------------------------------------
// Function Prototypes
//---------------------------------------------------------------------------

Guid GetStreamGuid(IStream* pStream);

//---------------------------------------------------------------------------
// Class StorageUtil (internal)
//
// StorageUtil contains some handy-dandy functions used throughout the
// class library for general "stuff".
//---------------------------------------------------------------------------

ref class StorageUtil
{
public:

	//-----------------------------------------------------------------------
	// Member Functions

	static Guid		Base64ToSysGuid(String^ base64);
	static int		CompareUUIDs(const UUID &lhs, const UUID &rhs);
	static Guid		GetContainerID(IComStorage^ storage);
	static Guid		GetObjectID(IComStream^ stream);
	static Guid		GetPropertySetID(IComPropertyStorage^ propStorage);
	static DWORD	GetStorageChildOpenMode(IComStorage^ storage);
	static DWORD	GetStorageMode(IComStorage^ storage);
	static DWORD	GetStreamMode(IComStream^ stream);
	static bool		IsStorageReadOnly(IComStorage^ storage);
	static bool		IsStreamReadOnly(IComStream^ stream);
	static String^	SysGuidToBase64(Guid guid);
	static UUID		SysGuidToUUID(Guid guid);
	static Guid		UUIDToSysGuid(const UUID& guid);
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __STORAGEUTIL_H_
