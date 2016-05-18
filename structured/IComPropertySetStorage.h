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

#ifndef __ICOMPROPERTYSETSTORAGE_H_
#define __ICOMPROPERTYSETSTORAGE_H_
#pragma once

#include "IComPointer.h"				// Include IComPointer declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Interface IComPropertySetStorage (internal)
//
// IComStorage is an identical interface to the COM IPropertySetStorage 
// interface.  This is used to allow multiple COM interfaces to be exposed 
// from a managed class
//---------------------------------------------------------------------------

interface class IComPropertySetStorage : public IComPointer
{
	//-----------------------------------------------------------------------
	// Methods

	// Create
	//
	// Creates and opens a new property set in the property set storage object
	HRESULT Create(REFFMTID fmtid, const CLSID* pclsid, DWORD grfFlags, DWORD grfMode,
		IPropertyStorage** ppPropStg);

	// Delete
	//
	// Deletes one of the property sets contained in the property set storage object
	HRESULT Delete(REFFMTID fmtid);

	// Enum
	//
	// Creates an enumerator object which contains information on the property sets
	HRESULT Enum(IEnumSTATPROPSETSTG** ppenum);

	// Open
	//
	// Opens a property set contained in the property set storage object
	HRESULT Open(REFFMTID fmtid, DWORD grfMode, IPropertyStorage** ppPropStg);
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __ICOMPROPERTYSETSTORAGE_H_
