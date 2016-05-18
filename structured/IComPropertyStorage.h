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

#ifndef __ICOMPROPERTYSTORAGE_H_
#define __ICOMPROPERTYSTORAGE_H_
#pragma once

#include "IComPointer.h"				// Include IComPointer declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// Interface IComPropertyStorage (internal)
//
// IComStorage is an identical interface to the COM IPropertyStorage 
// interface.  This is used to allow multiple COM interfaces to be exposed 
// from a managed class
//---------------------------------------------------------------------------

interface class IComPropertyStorage : public IComPointer
{
	//-----------------------------------------------------------------------
	// Methods

	// Commit
	//
	// Saves changes made to a property storage object to the parent storage object
	HRESULT Commit(DWORD grfCommitFlags);

	// DeleteMultiple
	//
	// Deletes as many of the indicated properties as exist in this property set
	HRESULT DeleteMultiple(ULONG cpspec, const PROPSPEC rgpspec[]);

	// DeletePropertyNames
	//
	// Deletes specified string names from the current property set
	HRESULT DeletePropertyNames(ULONG cpropid, const PROPID rgpropid[]);

	// Enum
	//
	// Enumerates the contents of the property set
	HRESULT Enum(IEnumSTATPROPSTG** ppenum);

	// ReadMultiple
	//
	// Reads specified properties from the current property set.
	HRESULT ReadMultiple(ULONG cpspec, const PROPSPEC rgpspec[], PROPVARIANT rgpropvar[]);

	// ReadPropertyNames
	//
	// retrieves any existing string names for the specified property IDs.
	HRESULT ReadPropertyNames(ULONG cpropid, const PROPID rgpropid[], LPWSTR rglpwstrName[]);

	// Revert
	//
	// Discards all changes to the named property set
	HRESULT Revert(void);

	// SetClass
	//
	// Assigns a new CLSID to the current property storage object
	HRESULT SetClass(REFCLSID clsid);

	// SetTimes
	//
	// sets the modification, access, and creation times of this property set
	HRESULT SetTimes(const ::FILETIME* pctime, const ::FILETIME* patime, const ::FILETIME* pmtime);

	// Stat
	//
	// Retrieves information about the current open property set
	HRESULT Stat(STATPROPSETSTG* pstatpsstg);

	// WriteMultiple
	//
	// Writes a specified group of properties to the current property set
	HRESULT WriteMultiple(ULONG cpspec, const PROPSPEC rgpspec[], const PROPVARIANT rgpropvar[], 
		PROPID propidNameFirst);

	// WritePropertyNames
	//
	// Assigns string names to a specified array of property IDs in the current property set
	HRESULT WritePropertyNames(ULONG cpropid, const PROPID rgpropid[], LPWSTR const rglpwstrName[]);
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif	// __ICOMPROPERTYSTORAGE_H_
