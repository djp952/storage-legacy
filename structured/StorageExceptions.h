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

#ifndef __STORAGEEXCEPTIONS_H_
#define __STORAGEEXCEPTIONS_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_NAMESPACE(zuki)
BEGIN_NAMESPACE(storage)
BEGIN_NAMESPACE(structured)

//---------------------------------------------------------------------------
// CACHE EXCEPTIONS
//---------------------------------------------------------------------------

// CachedItemExistsException
//
// Thrown when the code attempts to add an object to cache that already
// exists in the cache.  Ideally, the user would never see one of these
STRUCTURED_STORAGE_PUBLIC ref class CachedItemExistsException sealed : public Exception
{
internal:

	CachedItemExistsException(String^ name) :
		Exception(String::Format("Internal Error: The specified "
			"item [{0}] has already been cached and cannot be added to the "
			"collection.", name)) {}
};

//---------------------------------------------------------------------------
// CONTAINER EXCEPTIONS
//---------------------------------------------------------------------------

// ContainerDisposedException
//
// Thrown when the caller attempts to access a disposed container
STRUCTURED_STORAGE_PUBLIC ref class ContainerDisposedException sealed : public Exception
{
internal:

	ContainerDisposedException() :
		Exception("The specified container has been disposed of.") {}
};

// ContainerExistsException
//
// Thrown when the caller attempts to create a container that already exists
STRUCTURED_STORAGE_PUBLIC ref class ContainerExistsException sealed : public Exception
{
internal:

	ContainerExistsException(String^ name) :
		Exception(String::Format("The specified name [{0}] already exists "
			"as a subcontainer within this container.", name)) {}
};

// ContainerNotFoundException
//
// Thrown when the caller attempts to access a container that doesn't exist
STRUCTURED_STORAGE_PUBLIC ref class ContainerNotFoundException sealed : public Exception
{
internal:

	ContainerNotFoundException(String^ name) :
		Exception(String::Format("The specified subcontainer [{0}] does "
			"not exist.", name)) {}
};

// ContainerReadOnlyException
//
// Thrown when an attempt to modify a read-only container is made
STRUCTURED_STORAGE_PUBLIC ref class ContainerReadOnlyException sealed : public Exception
{
internal:

	ContainerReadOnlyException() : 
		Exception("The container is read-only and cannot be modified.") {}
};

//---------------------------------------------------------------------------
// NAME MAPPER EXCEPTIONS
//---------------------------------------------------------------------------

// MappingExistsException
//
// Thrown when a specific mapping name already exists in the property set
STRUCTURED_STORAGE_PUBLIC ref class MappingExistsException sealed : public Exception
{
internal:

	MappingExistsException(String^ name) :
		Exception(String::Format("The specified name [{0}] is already "
			"registered as an existing mapped name", name)) {}
};

// MappingNotFoundException
//
// Thrown when a specific mapping name cannot be located in the property set
STRUCTURED_STORAGE_PUBLIC ref class MappingNotFoundException sealed : public Exception
{
internal:

	MappingNotFoundException(String^ name) 
		: Exception(String::Format("The specified name [{0}] is not "
			"registered as a valid mapped name in this collection", name)) {}
};

//---------------------------------------------------------------------------
// OBJECT EXCEPTIONS
//---------------------------------------------------------------------------

// ObjectExistsException
//
// Thrown when the caller attempts to create an object that already exists
STRUCTURED_STORAGE_PUBLIC ref class ObjectExistsException sealed : public Exception
{
internal:

	ObjectExistsException(String^ name) :
		Exception(String::Format("The specified name [{0}] already exists "
			"as an object within this container.", name)) {}
};

// ObjectNotFoundException
//
// Thrown when the caller attempts to access an object that doesn't exist
STRUCTURED_STORAGE_PUBLIC ref class ObjectNotFoundException sealed : public Exception
{
internal:

	ObjectNotFoundException(String^ name) :
		Exception(String::Format("The specified object [{0}] does "
			"not exist in this container.", name)) {}
};

// ObjectReadOnlyException
//
// Thrown when an attempt to modify a read-only object is made
STRUCTURED_STORAGE_PUBLIC ref class ObjectReadOnlyException sealed : public Exception
{
internal:

	ObjectReadOnlyException() : Exception("This object is read-only.") {}
};

// ObjectStreamOpenException
//
// Thrown when the caller attempts to do something that cannot be done while
// a StorageObjectStream is open against a StorageObject
STRUCTURED_STORAGE_PUBLIC ref class ObjectStreamOpenException sealed : public Exception
{
internal:

	ObjectStreamOpenException() :
		Exception("This operation cannot be performed while there is an "
			"open StorageObjectStream against this StorageObject.") {}
};

// ObjectTooLargeException
//
// Thrown when the caller attempts to access an absurdly large object stream
// by converting it into a byte array.  If anyone EVER sees this, I'll be
// quite surprised.  2GB is quite a bit to ask of Structured Storage
STRUCTURED_STORAGE_PUBLIC ref class ObjectTooLargeException sealed : public Exception
{
internal:

	ObjectTooLargeException() :
		Exception("The contents of this StorageObject exceed the maximum capacity "
			"of a byte array, and must be accessed with a StorageObjectReader.") {}
};

//---------------------------------------------------------------------------
// PROPERTY SET EXCEPTIONS
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Class PropertySetsReadOnlyException
//
// Thrown when an attempt is made to modify a read-only property set
//---------------------------------------------------------------------------

// InvalidPropertyDataTypeException
//
// Thrown when the caller attempts to write an invalid data type into the
// .Item setter of a PropertySet collection
STRUCTURED_STORAGE_PUBLIC ref class InvalidPropertyDataTypeException : public Exception
{
internal:

	InvalidPropertyDataTypeException(Type^ type) :
		Exception(String::Format("The data type [{0}] cannot be used as the "
		"value of a StoragePropertySet property.", type->ToString())) {}
};

// PropertyExistsException
//
// Thrown when the caller attempts to create a property that already exists
STRUCTURED_STORAGE_PUBLIC ref class PropertyExistsException sealed : public Exception
{
internal:

	PropertyExistsException(String^ name) :
		Exception(String::Format("The specified property [{0}] already exists "
			"as an object within this property set.", name)) {}
};

// PropertySetExistsException
//
// Thrown when the caller attempts to create a property set that already exists
STRUCTURED_STORAGE_PUBLIC ref class PropertySetExistsException sealed : public Exception
{
internal:

	PropertySetExistsException(String^ name) :
		Exception(String::Format("The specified property set [{0}] already exists "
			"within this container.", name)) {}
};

// PropertySetNotFoundException
//
// Thrown when the caller attempts to access a property set that doesn't exist
STRUCTURED_STORAGE_PUBLIC ref class PropertySetNotFoundException sealed : public Exception
{
internal:

	PropertySetNotFoundException(String^ name) :
		Exception(String::Format("The specified property set [{0}] does "
			"not exist.", name)) {}
};
// PropertyNotFoundException
//
// Thrown when an attempt is made to access a non-existant property
STRUCTURED_STORAGE_PUBLIC ref class PropertyNotFoundException sealed : public Exception
{
internal:

	PropertyNotFoundException(String^ name) :
		Exception(String::Format("The specified property [{0}] does "
			"not exist in this property set.", name)) {}
};

// PropertySetReadOnlyException
//
// Thrown when an attempt to modify a read-only property set is made
STRUCTURED_STORAGE_PUBLIC ref class PropertySetReadOnlyException sealed : public Exception
{
internal:

	PropertySetReadOnlyException() : Exception("This property set is read-only") {}
};

//---------------------------------------------------------------------------

END_NAMESPACE(structured)
END_NAMESPACE(storage)
END_NAMESPACE(zuki)

#pragma warning(pop)

#endif	// __STORAGEEXCEPTIONS_H_
