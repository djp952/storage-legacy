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
#include "StorageContainerCollection.h"		// Include this class' declarations
#include "StructuredStorage.h"				// Include StructuredStorage decls
#include "StorageContainer.h"				// Include StorageContainer declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// StorageContainer Constructor (internal)
//
// Arguments:
//
//	root			- Reference to the parent Storage object
//	parent			- Reference to parent storage or NULLPTR
//	storage			- Reference to this ComStorage instance

StorageContainer::StorageContainer(StructuredStorage^ root, ComStorage^ parent, ComStorage^ storage) : 
	m_root(root), m_parent(parent), m_storage(storage)
{
	// ROOT STORAGE: If this is the root Storage object, both m_root and m_parent
	// should be NULL.  Otherwise, they should both be non-NULL.

	if(m_storage == nullptr) throw gcnew ArgumentNullException();
	if((m_root == nullptr) && (m_parent != nullptr)) throw gcnew ArgumentException();
	if((m_root != nullptr) && (m_parent == nullptr)) throw gcnew ArgumentException();

	// If we are the root storage object, we have to cast that reference ...

	if(m_root == nullptr) m_root = safe_cast<StructuredStorage^>(this);
	
	m_readOnly = StorageUtil::IsStorageReadOnly(m_storage);

	// Retrieve the unique GUID that is the real name of this container
	// and uniquely identifies it throughout the library.  The mystical 
	// root container always gets the empty guid, though ...

	m_contid = (m_parent != nullptr) ? StorageUtil::GetContainerID(m_storage) : Guid::Empty;

	// Attempt to create the contained collection classes 

	m_containers = gcnew StorageContainerCollection(m_root, m_storage);
	m_objects = gcnew StorageObjectCollection(m_root, m_storage);
	m_propsets = gcnew StoragePropertySetCollection(m_root, m_storage);
}

//---------------------------------------------------------------------------
// StorageContainer::Containers::get
//
// Gets a reference to the contained StorageContainerCollection

StorageContainerCollection^ StorageContainer::Containers::get(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return m_containers;
}

//---------------------------------------------------------------------------
// StorageContainer::Name::get
//
// Retrieves the current name of the container

String^ StorageContainer::Name::get(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());

	// The root storage container has a fixed name that doesn't get looked up

	if(m_parent == nullptr) return CONTAINER_NAME_ROOT;
	else return m_parent->ContainerNameMapper->MapGuidToName(m_contid);
}

//---------------------------------------------------------------------------
// StorageContainer::Name::set
//
// Changes the name of the container

void StorageContainer::Name::set(String^ value)
{
	CHECK_DISPOSED(m_storage->IsDisposed());

	// The root storage container cannot be renamed under any circumstances,
	// and any container that's read-only also obviously cannot be renamed

	if(m_parent == nullptr) throw gcnew InvalidOperationException();
	if(value == nullptr) throw gcnew ArgumentNullException();
	if(m_readOnly) throw gcnew ContainerReadOnlyException();

	m_parent->ContainerNameMapper->RenameMapping(m_contid, value);
}

//---------------------------------------------------------------------------
// StorageContainer::Objects::get
//
// Gets a reference to the contained StorageObjectCollection

StorageObjectCollection^ StorageContainer::Objects::get(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return m_objects;
}

//---------------------------------------------------------------------------
// StorageContainer::Objects::get
//
// Gets a reference to the contained StoragePropertySetCollection

StoragePropertySetCollection^ StorageContainer::PropertySets::get(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return m_propsets;
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
