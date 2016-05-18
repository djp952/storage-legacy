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

#ifndef __COMSTORAGE_H_
#define __COMSTORAGE_H_
#pragma once

#include "StorageNameMapper.h"			// Include StorageNameMapper declarations
#include "IComPropertySetStorage.h"		// Include IComPropertySetStorage decls
#include "IComStorage.h"				// Include IComStorage declarations
#include "StorageException.h"			// Include StorageException decls

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// Class ComStorage (internal)
//
// ComStorage implements a safe pointer class that allows a COM pointer
// to be safely shared among managed object instances, and implements thread
// safety for all member functions
//---------------------------------------------------------------------------

ref class ComStorage sealed : public IComStorage, IComPropertySetStorage
{
public:

	//-----------------------------------------------------------------------
	// Constructor
	
	ComStorage(IStorage* pStorage);

	//-----------------------------------------------------------------------
	// Member Functions

	// Commit (IComStorage)
	//
	// Ensures any changes to the storage object are persisted to disk
	virtual HRESULT Commit(DWORD grfCommitFlags);

	// CopyTo (IComStorage)
	//
	// Copies the entire contents of an open storage object to another object
	virtual HRESULT CopyTo(DWORD ciidExclude, IID const* rgiidExclude, SNB snbExclude,
		IStorage* pstgDest);

	// CreatePropertySet (IComPropertySetStorage)
	//
	// Creates and opens a new property set in the property set storage object
	virtual HRESULT CreatePropertySet(REFFMTID fmtid, const CLSID* pclsid, DWORD grfFlags, 
		DWORD grfMode, IPropertyStorage** ppPropStg) sealed = IComPropertySetStorage::Create;

	// CreateStorage (IComStorage)
	//
	// Creates and opens a new storage object nested within this storage object
	virtual HRESULT CreateStorage(const WCHAR* pwcsName, DWORD grfMode, DWORD reserved1,
		DWORD reserved2, IStorage** ppstg);

	// CreateStream (IComStorage)
	//
	// Creates and opens a stream object contained in this storage object
	virtual HRESULT CreateStream(const WCHAR* pwcsName, DWORD grfMode, DWORD reserved1,
		DWORD reserved2, IStream** ppstm);

	// DeletePropertySet (IComPropertySetStorage)
	//
	// Deletes one of the property sets contained in the property set storage object
	virtual HRESULT DeletePropertySet(REFFMTID fmtid) sealed = IComPropertySetStorage::Delete;

	// DestroyElement (IComStorage)
	//
	// Removes the specified storage or stream from this storage object
	virtual HRESULT DestroyElement(const WCHAR* pwcsName);

	// EnumElements (IComStorage)
	//
	// Enumerates the storage and stream objects contained within this storage
	virtual HRESULT EnumElements(DWORD reserved1, void* reserved2, DWORD reserved3, 
		IEnumSTATSTG** ppenum);

	// EnumPropertySets (IComPropertySetStorage)
	//
	// Creates an enumerator object which contains information on the property sets
	virtual HRESULT EnumPropertySets(IEnumSTATPROPSETSTG** ppenum) sealed = 
		IComPropertySetStorage::Enum;

	// IsDisposed (IComPointer)
	//
	// Exposes the object's internal disposed status
	virtual bool IsDisposed(void) { return m_disposed; }

	// MoveElementTo (IComStorage)
	//
	// Copies or moves a substorage or stream from this storage object
	virtual HRESULT MoveElementTo(const WCHAR* pwcsName, IStorage* pstgDest, LPWSTR pwcsNewName,
		DWORD grfFlags);

	// OpenPropertySet (IComPropertySetStorage)
	//
	// Opens a property set contained in the property set storage object
	virtual HRESULT OpenPropertySet(REFFMTID fmtid, DWORD grfMode, IPropertyStorage** ppPropStg) sealed =
		IComPropertySetStorage::Open;

	// OpenStorage (IComStorage)
	//
	// Opens an existing storage object with the specified name
	virtual HRESULT OpenStorage(const WCHAR* pwcsName, IStorage* pstgPriority, DWORD grfMode, 
		SNB snbExclude, DWORD reserved, IStorage** ppstg);

	// OpenStream (IComStorage)
	//
	// Opens an existing stream object with the specified name
	virtual HRESULT OpenStream(const WCHAR* pwcsName, void* reserved1, DWORD grfMode, 
		DWORD reserved2, IStream** ppstm);

	// RenameElement (IComStorage)
	//
	// Renames the specified substorage or stream in this storage object
	virtual HRESULT RenameElement(const WCHAR* pwcsOldName, const WCHAR* pwcsNewName);

	// Revert (IComStorage)
	//
	// Discards all changes that have been made to the storage object
	virtual HRESULT Revert(void);

	// SetClass (IComStorage)
	//
	// Assigns the specified class identifier (CLSID) to this storage object.
	virtual HRESULT SetClass(REFCLSID clsid);

	// SetElementTimes (IComStorage)
	//
	// Sets the modification, access, and creation times of the specified storage element
	virtual HRESULT SetElementTimes(const WCHAR* pwcsName, ::FILETIME const* pctime, 
		::FILETIME const* patime, ::FILETIME const* pmtime);

	// SetStateBits (IComStorage)
	//
	// Stores up to 32 bits of state information in this storage object
	virtual HRESULT SetStateBits(DWORD grfStateBits, DWORD grfMask);

	// Stat (IComStorage)
	//
	// Retrieves the STATSTG structure for this open storage object
	virtual HRESULT Stat(::STATSTG* pstatstg, DWORD grfStatFlag);

	//-----------------------------------------------------------------------
	// Properties

	// ContainerNameMapper
	//
	// Accesses the name mapper instance for sub-storages
	property StorageNameMapper^ ContainerNameMapper
	{
		StorageNameMapper^ get(void);
	}

	// ObjectNameMapper
	//
	// Accesses the name mapper instance for streams
	property StorageNameMapper^ ObjectNameMapper
	{
		StorageNameMapper^ get(void);
	}

	// PropertySetNameMapper
	//
	// Accesses the name mapper instance for property sets
	property StorageNameMapper^ PropertySetNameMapper
	{
		StorageNameMapper^ get(void);
	}

private:

	// DESTRUCTOR / FINALIZER
	~ComStorage();
	!ComStorage();

	//-----------------------------------------------------------------------
	// Member Variables

	bool					m_disposed;			// Object disposal flag
	IStorage*				m_pStorage;			// Contained IStorage
	IPropertySetStorage*	m_pPropStorage;		// Contained IPropertySetStorage

	StorageNameMapper^		m_contMapper;		// Container name mapper
	StorageNameMapper^		m_objMapper;		// Object name mapper
	StorageNameMapper^		m_propSetMapper;	// Property Set name mapper
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif	// __COMSTORAGE_H_
