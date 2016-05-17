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

#include "stdafx.h"							// Include project pre-compiled headers
#include "ComStorage.h"						// Include ComStorage declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_NAMESPACE(zuki)
BEGIN_NAMESPACE(storage)
BEGIN_NAMESPACE(structured)

//---------------------------------------------------------------------------
// ComStorage Constructor
//
// Arguments:
//
//	pStorage	- IStorage pointer to be wrapped up

ComStorage::ComStorage(IStorage* pStorage) : m_pStorage(pStorage), m_pPropStorage(NULL)
{
	IPropertySetStorage*	pPropStorage;		// IPropertySetStorage
	HRESULT					hResult;			// Result from function call

	if(!m_pStorage) throw gcnew ArgumentNullException();

	// Since the IStorage and IPropertySetStorage interfaces are bound to the
	// same underlying COM object instance, we have to maintain them both here
	// and expose them appropriately via the IComXXXX interfaces to the callers.

	hResult = m_pStorage->QueryInterface(__uuidof(IPropertySetStorage), 
		reinterpret_cast<void**>(&pPropStorage));
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	m_pStorage->AddRef();				// AddRef() main IStorage to keep it
	m_pPropStorage = pPropStorage;		// IPropertySetStorage is already ours

	// Create the contained COM name mapper instances.  The constructors for
	// ComNameMapper do not throw exceptions, so we can safely avoid a try

	m_contMapper = gcnew StorageNameMapper(FMTID_ContainerNameMapper, this);
	m_objMapper = gcnew StorageNameMapper(FMTID_ObjectNameMapper, this);
	m_propSetMapper = gcnew StorageNameMapper(FMTID_PropertySetNameMapper, this);
}

//---------------------------------------------------------------------------
// ComStorage Destructor

ComStorage::~ComStorage()
{
	delete m_propSetMapper;				// Dispose of property set mapper
	delete m_objMapper;					// Dispose of object mapper
	delete m_contMapper;				// Dispose of container mapper
	
	this->!ComStorage();				// Invoke the finalizer
	m_disposed = true;					// Object is now disposed of
}

//---------------------------------------------------------------------------
// ComStorage Finalizer

ComStorage::!ComStorage()
{
	if(m_pPropStorage) m_pPropStorage->Release();
	m_pPropStorage = NULL;

	if(m_pStorage) m_pStorage->Release();
	m_pStorage = NULL;
}

//---------------------------------------------------------------------------
// ComStorage::Commit
//
// Ensures any changes made to the storage object are persisted to disk

HRESULT ComStorage::Commit(DWORD grfCommitFlags)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->Commit(grfCommitFlags);
}

//---------------------------------------------------------------------------
// ComStorage::ContainerNameMapper
//
// Accesses the contained StorageNameMapper instance

StorageNameMapper^ ComStorage::ContainerNameMapper::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_contMapper;
}

//---------------------------------------------------------------------------
// ComStorage::CopyTo
//
// Copies the entire contents of an open storage object to another object

HRESULT ComStorage::CopyTo(DWORD ciidExclude, IID const* rgiidExclude, 
	SNB snbExclude, IStorage* pstgDest)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->CopyTo(ciidExclude, rgiidExclude, snbExclude, pstgDest);	
}

//---------------------------------------------------------------------------
// ComStorage::CreatePropertySet
//
// Creates and opens a new property set in the property set storage object

HRESULT ComStorage::CreatePropertySet(REFFMTID fmtid, const CLSID* pclsid, DWORD grfFlags, 
	DWORD grfMode, IPropertyStorage** ppPropStg)
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->Create(fmtid, pclsid, grfFlags, grfMode, ppPropStg);
}

//---------------------------------------------------------------------------
// ComStorage::CreateStorage
//
// Creates and opens a new storage object nested within this storage object

HRESULT ComStorage::CreateStorage(const WCHAR* pwcsName, DWORD grfMode, 
	DWORD reserved1, DWORD reserved2, IStorage** ppstg)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->CreateStorage(pwcsName, grfMode, reserved1, reserved2, ppstg);
}

//---------------------------------------------------------------------------
// ComStorage::CreateStream
//
// Creates and opens a stream object contained in this storage object

HRESULT ComStorage::CreateStream(const WCHAR* pwcsName, DWORD grfMode, DWORD reserved1,
	DWORD reserved2, IStream** ppstm)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->CreateStream(pwcsName, grfMode, reserved1, reserved2, ppstm);
}

//---------------------------------------------------------------------------
// ComStorage::DeletePropertySet
//
// Deletes one of the property sets contained in the property set storage object

HRESULT ComStorage::DeletePropertySet(REFFMTID fmtid)
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->Delete(fmtid);
}

//---------------------------------------------------------------------------
// ComStorage::DestroyElement
//
// Removes the specified storage or stream from this storage object

HRESULT ComStorage::DestroyElement(const WCHAR* pwcsName)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->DestroyElement(pwcsName);
}

//---------------------------------------------------------------------------
// ComStorage::EnumElements
//
// Enumerates the storage and stream objects contained within this storage

HRESULT ComStorage::EnumElements(DWORD reserved1, void* reserved2, DWORD reserved3, 
	IEnumSTATSTG** ppenum)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->EnumElements(reserved1, reserved2, reserved3, ppenum);
}

//---------------------------------------------------------------------------
// ComStorage::EnumPropertySets
//
// Creates an enumerator object which contains information on the property sets

HRESULT ComStorage::EnumPropertySets(IEnumSTATPROPSETSTG** ppenum)
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->Enum(ppenum);
}

//---------------------------------------------------------------------------
// ComStorage::MoveElementTo
//
// Copies or moves a substorage or stream from this storage object

HRESULT ComStorage::MoveElementTo(const WCHAR* pwcsName, IStorage* pstgDest, 
	LPWSTR pwcsNewName, DWORD grfFlags)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->MoveElementTo(pwcsName, pstgDest, pwcsNewName, grfFlags);
}

//---------------------------------------------------------------------------
// ComStorage::ObjectNameMapper
//
// Accesses the contained StorageNameMapper instance

StorageNameMapper^ ComStorage::ObjectNameMapper::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_objMapper;
}

//---------------------------------------------------------------------------
// ComStorage::OpenPropertySet
//
// Opens a property set contained in the property set storage object

HRESULT ComStorage::OpenPropertySet(REFFMTID fmtid, DWORD grfMode, IPropertyStorage** ppPropStg)
{
	HRESULT				hResult;		// Result from function call

	CHECK_DISPOSED(m_disposed);

	// No matter how hard I tried, there are still finalization timing issues
	// with storage interfaces being open yet uncached.  If a call to open an
	// existing storage object fails with STG_E_ACCESSDENIED, sit here until 
	// .NET gets around to finalizing everything.  If it still fails afterwards
	// there is an actual problem, so don't do it more than once

	hResult = m_pPropStorage->Open(fmtid, grfMode, ppPropStg);
	if(hResult != STG_E_ACCESSDENIED) return hResult;

	GC::WaitForPendingFinalizers();
	return m_pPropStorage->Open(fmtid, grfMode, ppPropStg);
}

//---------------------------------------------------------------------------
// ComStorage::OpenStorage
//
// Opens an existing storage object with the specified name

HRESULT ComStorage::OpenStorage(const WCHAR* pwcsName, IStorage* pstgPriority, 
	DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage** ppstg)
{
	HRESULT				hResult;		// Result from function call

	CHECK_DISPOSED(m_disposed);

	// No matter how hard I tried, there are still finalization timing issues
	// with storage interfaces being open yet uncached.  If a call to open an
	// existing storage object fails with STG_E_ACCESSDENIED, sit here until 
	// .NET gets around to finalizing everything.  If it still fails afterwards
	// there is an actual problem, so don't do it more than once

	hResult = m_pStorage->OpenStorage(pwcsName, pstgPriority, grfMode, snbExclude, reserved, ppstg);
	if(hResult != STG_E_ACCESSDENIED) return hResult;

	GC::WaitForPendingFinalizers();
	return m_pStorage->OpenStorage(pwcsName, pstgPriority, grfMode, snbExclude, reserved, ppstg);
}

//---------------------------------------------------------------------------
// ComStorage::OpenStream
//
// Opens an existing stream object with the specified name

HRESULT ComStorage::OpenStream(const WCHAR* pwcsName, void* reserved1, DWORD grfMode, 
	DWORD reserved2, IStream** ppstm)
{
	HRESULT				hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed);

	// No matter how hard I tried, there are still finalization timing issues
	// with storage interfaces being open yet uncached.  If a call to open an
	// existing storage object fails with STG_E_ACCESSDENIED, sit here until 
	// .NET gets around to finalizing everything.  If it still fails afterwards
	// there is an actual problem, so don't do it more than once

	hResult = m_pStorage->OpenStream(pwcsName, reserved1, grfMode, reserved2, ppstm);
	if(hResult != STG_E_ACCESSDENIED) return hResult;

	GC::WaitForPendingFinalizers();
	return m_pStorage->OpenStream(pwcsName, reserved1, grfMode, reserved2, ppstm);
}

//---------------------------------------------------------------------------
// ComStorage::PropertySetNameMapper
//
// Accesses the contained StorageNameMapper instance

StorageNameMapper^ ComStorage::PropertySetNameMapper::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_propSetMapper;
}

//---------------------------------------------------------------------------
// ComStorage::RenameElement
//
// Renames the specified substorage or stream in this storage object

HRESULT ComStorage::RenameElement(const WCHAR* pwcsOldName, const WCHAR* pwcsNewName)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->RenameElement(pwcsOldName, pwcsNewName);
}

//---------------------------------------------------------------------------
// ComStorage::Revert
//
// Discards all changes that have been made to the storage object

HRESULT ComStorage::Revert(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->Revert();
}

//---------------------------------------------------------------------------
// ComStorage::SetClass
//
// Assigns the specified class identifier (CLSID) to this storage object.

HRESULT ComStorage::SetClass(REFCLSID clsid)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->SetClass(clsid);
}

//---------------------------------------------------------------------------
// ComStorage::SetElementTimes
//
// Sets the modification, access, and creation times of the specified storage element

HRESULT ComStorage::SetElementTimes(const WCHAR* pwcsName, ::FILETIME const* pctime, 
	::FILETIME const* patime, ::FILETIME const* pmtime)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->SetElementTimes(pwcsName, pctime, patime, pmtime);
}

//---------------------------------------------------------------------------
// ComStorage::SetStateBits
//
// Stores up to 32 bits of state information in this storage object

HRESULT ComStorage::SetStateBits(DWORD grfStateBits, DWORD grfMask)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->SetStateBits(grfStateBits, grfMask);
}

//---------------------------------------------------------------------------
// ComStorage::Stat
//
// Retrieves the STATSTG structure for this open storage object

HRESULT ComStorage::Stat(::STATSTG* pstatstg, DWORD grfStatFlag)
{
	CHECK_DISPOSED(m_disposed);
	return m_pStorage->Stat(pstatstg, grfStatFlag);
}


//---------------------------------------------------------------------------

END_NAMESPACE(structured)
END_NAMESPACE(storage)
END_NAMESPACE(zuki)

#pragma warning(pop)
