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
#include "StorageContainerEnumerator.h"		// Include this class' declarations
#include "StructuredStorage.h"				// Include StructuredStorage declarations
#include "StorageContainer.h"				// Include StorageContainer decls

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// StorageContainerEnumerator Constructor (internal)
//
// Arguments:
//
//	root			- Reference to the root Storage object
//	storage			- Reference to ComStorage instance

StorageContainerEnumerator::StorageContainerEnumerator(StructuredStorage^ root, 
	ComStorage^ storage) : m_root(root), m_storage(storage), m_current(-1)
{
	Generic::ICollection<Guid>^		guids;		// Enumerated storage GUIDs

	if(m_root == nullptr) throw gcnew ArgumentNullException();
	if(m_storage == nullptr) throw gcnew ArgumentNullException();

	// This used to have all kinds of code that was quite frankly stupid, since it
	// physically enumerated the storage looking for things, and then checked with
	// the name mapper to see if that thing is valid.  NOW it just asks the name mapper
	// for everything it knows about, saving both execution time and resources

	guids = m_storage->ContainerNameMapper->ToDictionary()->Values;
	m_items = gcnew array<Guid>(guids->Count);
	if(m_items->Length > 0) guids->CopyTo(m_items, 0);
}

//---------------------------------------------------------------------------
// StorageContainerEnumerator::Current::get
//
// Retrieves the object at the current position in the enumerator.  In this
// implementation, we construct a new StorageContainer object each time the
// user happens to ask for it
//
// Arguments:
//
//	NONE

StorageContainer^ StorageContainerEnumerator::Current::get(void)
{
	Guid					contid;				// Container ID code
	ComStorage^				subStorage;			// Child container instance
	IStorage*				pSubStorage;		// Child container pointer
	String^					name;				// BASE64 container name
	PinnedStringPtr			pinName;			// Pinned container name
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());
	
	// The enumerator must be positioned somewhere between the first and last elements

	if((m_current < 0) || (m_current >= m_items->Length)) throw gcnew InvalidOperationException();

	contid = m_items[m_current];				// Grab the current container GUID

	lock cacheLock(m_root->ComStorageCache->SyncRoot);		// <--- THREAD SAFETY

	// If an instance of this container already exists somewhere, we can get
	// a copy of that pointer and just wrap with a new class instance

	if(m_root->ComStorageCache->TryGetValue(contid, subStorage))
		return gcnew StorageContainer(m_root, m_storage, subStorage);

	// The container hasn't been cached (or is dead), so we need to make a new one

	name = StorageUtil::SysGuidToBase64(contid);	// Convert GUID to BASE64
	pinName = PtrToStringChars(name);				// Pin it down

	// Attempt to open up the sub container's IStorage interface 

	hResult = m_storage->OpenStorage(pinName, NULL, 
		StorageUtil::GetStorageChildOpenMode(m_storage), NULL, 0, &pSubStorage);
	if(FAILED(hResult)) throw gcnew StorageException(hResult, name);

	// Create a new ComStorage wrapper for the container, and cache it off
	// in case anyone else tries to access it later on ...

	subStorage = gcnew ComStorage(pSubStorage);
	pSubStorage->Release();

	m_root->ComStorageCache->Add(contid, subStorage);
	return gcnew StorageContainer(m_root, m_storage, subStorage);
}

//---------------------------------------------------------------------------
// StorageContainerEnumerator::MoveNext
//
// Advances the enumerator to the next item in the collection
//
// Arguments:
//
//	NONE

bool StorageContainerEnumerator::MoveNext(void)
{
	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	m_current++;								// Move to the next element
	return (m_current < m_items->Length);		// True/False based on position
}

//---------------------------------------------------------------------------
// StorageContainerEnumerator::Reset
//
// Resets the enumerator to right before the first collection element
//
// Arguments:
//
//	NONE

void StorageContainerEnumerator::Reset(void)
{
	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());
	m_current = -1;
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
