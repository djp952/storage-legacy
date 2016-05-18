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
#include "ComPropertyStorage.h"				// Include ComPropertyStorage declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// ComPropertyStorage Constructor
//
// Arguments:
//
//	pStorage	- IStorage pointer to be wrapped up

ComPropertyStorage::ComPropertyStorage(IPropertyStorage* pPropStorage) : 
	m_pPropStorage(pPropStorage)
{
	if(!m_pPropStorage) throw gcnew ArgumentNullException();
	m_pPropStorage->AddRef();
}

//---------------------------------------------------------------------------
// ComPropertyStorage Finalizer

ComPropertyStorage::!ComPropertyStorage()
{
	if(m_pPropStorage) m_pPropStorage->Release();
	m_pPropStorage = NULL;
}

//---------------------------------------------------------------------------
// Commit
//
// Saves changes made to a property storage object to the parent storage object

HRESULT ComPropertyStorage::Commit(DWORD grfCommitFlags)
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->Commit(grfCommitFlags);
}

//---------------------------------------------------------------------------
// DeleteMultiple
//
// Deletes as many of the indicated properties as exist in this property set

HRESULT ComPropertyStorage::DeleteMultiple(ULONG cpspec, const PROPSPEC rgpspec[])
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->DeleteMultiple(cpspec, rgpspec);
}

//---------------------------------------------------------------------------
// DeletePropertyNames
//
// Deletes specified string names from the current property set

HRESULT ComPropertyStorage::DeletePropertyNames(ULONG cpropid, const PROPID rgpropid[])
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->DeletePropertyNames(cpropid, rgpropid);
}

//---------------------------------------------------------------------------
// Enum
//
// Enumerates the contents of the property set

HRESULT ComPropertyStorage::Enum(IEnumSTATPROPSTG** ppenum)
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->Enum(ppenum);
}

//---------------------------------------------------------------------------
// ReadMultiple
//
// Reads specified properties from the current property set.

HRESULT ComPropertyStorage::ReadMultiple(ULONG cpspec, const PROPSPEC rgpspec[], 
	PROPVARIANT rgpropvar[])
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->ReadMultiple(cpspec, rgpspec, rgpropvar);
}

//---------------------------------------------------------------------------
// ReadPropertyNames
//
// retrieves any existing string names for the specified property IDs.

HRESULT ComPropertyStorage::ReadPropertyNames(ULONG cpropid, const PROPID rgpropid[], 
	LPWSTR rglpwstrName[])
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->ReadPropertyNames(cpropid, rgpropid, rglpwstrName);
}

//---------------------------------------------------------------------------
// Revert
//
// Discards all changes to the named property set

HRESULT ComPropertyStorage::Revert(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->Revert();
}

//---------------------------------------------------------------------------
// SetClass
//
// Assigns a new CLSID to the current property storage object

HRESULT ComPropertyStorage::SetClass(REFCLSID clsid)
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->SetClass(clsid);
}

//---------------------------------------------------------------------------
// SetTimes
//
// sets the modification, access, and creation times of this property set

HRESULT ComPropertyStorage::SetTimes(const ::FILETIME* pctime, const ::FILETIME* patime, 
	const ::FILETIME* pmtime)
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->SetTimes(pctime, patime, pmtime);
}

//---------------------------------------------------------------------------
// Stat
//
// Retrieves information about the current open property set

HRESULT ComPropertyStorage::Stat(STATPROPSETSTG* pstatpsstg)
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->Stat(pstatpsstg);
}

//---------------------------------------------------------------------------
// WriteMultiple
//
// Writes a specified group of properties to the current property set

HRESULT ComPropertyStorage::WriteMultiple(ULONG cpspec, const PROPSPEC rgpspec[], 
	const PROPVARIANT rgpropvar[], PROPID propidNameFirst)
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->WriteMultiple(cpspec, rgpspec, rgpropvar, propidNameFirst);
}

//---------------------------------------------------------------------------
// WritePropertyNames
//
// Assigns string names to a specified array of property IDs in the current property set

HRESULT ComPropertyStorage::WritePropertyNames(ULONG cpropid, const PROPID rgpropid[], 
	LPWSTR const rglpwstrName[])
{
	CHECK_DISPOSED(m_disposed);
	return m_pPropStorage->WritePropertyNames(cpropid, rgpropid, rglpwstrName);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
