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

#ifndef __COMPROPERTYSTORAGE_H_
#define __COMPROPERTYSTORAGE_H_
#pragma once

#include "IComPropertyStorage.h"		// Include IComPropertyStorage decls
#include "StorageException.h"			// Include StorageException decls

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// Class ComPropertyStorage (internal)
//
// ComPropertyStorage implements a safe pointer class that allows a COM pointer
// to be safely shared among managed object instances, and implements thread
// safety for all member functions
//---------------------------------------------------------------------------

ref class ComPropertyStorage sealed : public IComPropertyStorage
{
public:

	//-----------------------------------------------------------------------
	// Constructor
	
	ComPropertyStorage(IPropertyStorage* pPropStorage);

	//-----------------------------------------------------------------------
	// Member Functions

	// Commit (IComPropertyStorage)
	//
	// Saves changes made to a property storage object to the parent storage object
	virtual HRESULT Commit(DWORD grfCommitFlags);

	// DeleteMultiple (IComPropertyStorage)
	//
	// Deletes as many of the indicated properties as exist in this property set
	virtual HRESULT DeleteMultiple(ULONG cpspec, const PROPSPEC rgpspec[]);

	// DeletePropertyNames (IComPropertyStorage)
	//
	// Deletes specified string names from the current property set
	virtual HRESULT DeletePropertyNames(ULONG cpropid, const PROPID rgpropid[]);

	// Enum (IComPropertyStorage)
	//
	// Enumerates the contents of the property set
	virtual HRESULT Enum(IEnumSTATPROPSTG** ppenum);

	// IsDisposed (IComPointer)
	//
	// Exposes the object's internal disposed status
	virtual bool IsDisposed(void) { return m_disposed; }

	// ReadMultiple (IComPropertyStorage)
	//
	// Reads specified properties from the current property set.
	virtual HRESULT ReadMultiple(ULONG cpspec, const PROPSPEC rgpspec[], PROPVARIANT rgpropvar[]);

	// ReadPropertyNames (IComPropertyStorage)
	//
	// retrieves any existing string names for the specified property IDs.
	virtual HRESULT ReadPropertyNames(ULONG cpropid, const PROPID rgpropid[], LPWSTR rglpwstrName[]);

	// Revert (IComPropertyStorage)
	//
	// Discards all changes to the named property set
	virtual HRESULT Revert(void);

	// SetClass (IComPropertyStorage)
	//
	// Assigns a new CLSID to the current property storage object
	virtual HRESULT SetClass(REFCLSID clsid);

	// SetTimes (IComPropertyStorage)
	//
	// sets the modification, access, and creation times of this property set
	virtual HRESULT SetTimes(const ::FILETIME* pctime, const ::FILETIME* patime, const ::FILETIME* pmtime);

	// Stat (IComPropertyStorage)
	//
	// Retrieves information about the current open property set
	virtual HRESULT Stat(STATPROPSETSTG* pstatpsstg);

	// WriteMultiple (IComPropertyStorage)
	//
	// Writes a specified group of properties to the current property set
	virtual HRESULT WriteMultiple(ULONG cpspec, const PROPSPEC rgpspec[], const PROPVARIANT rgpropvar[], 
		PROPID propidNameFirst);

	// WritePropertyNames (IComPropertyStorage)
	//
	// Assigns string names to a specified array of property IDs in the current property set
	virtual HRESULT WritePropertyNames(ULONG cpropid, const PROPID rgpropid[], LPWSTR const rglpwstrName[]);

private:

	// DESTRUCTOR / FINALIZER
	~ComPropertyStorage() { this->!ComPropertyStorage(); m_disposed = true; }
	!ComPropertyStorage();

	//-----------------------------------------------------------------------
	// Member Variables

	bool					m_disposed;			// Object disposal flag
	IPropertyStorage*		m_pPropStorage;		// Contained IPropertyStorage
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif	// __COMPROPERTYSTORAGE_H_
