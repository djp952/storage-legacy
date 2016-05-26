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

using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace zuki.storage.virtualdisk.test
{
	[TestClass()]
	public class VirtualDiskMetadata
	{
		static string s_vdiskpath;
		static VirtualDisk s_vdisk;

		[ClassInitialize()]
		public static void ClassInit(TestContext context)
		{
			s_vdiskpath = Path.Combine(Environment.CurrentDirectory, "testmetadata.vhdx");
			if (File.Exists(s_vdiskpath)) File.Delete(s_vdiskpath);
			s_vdisk = VirtualDisk.Create(s_vdiskpath, VirtualDiskType.VHDX, 4 * 1024 * 1024);
			Assert.IsNotNull(s_vdisk);
		}

		[ClassCleanup()]
		public static void ClassCleanup()
		{
			if (s_vdisk != null) s_vdisk.Dispose();
			if (File.Exists(s_vdiskpath)) File.Delete(s_vdiskpath);
		}

		[TestMethod(), TestCategory("Virtual Disk Metadata")]
		public void VirtualDiskMetadata_Collection()
		{
			// Create some metadata
			KeyValuePair<Guid, byte[]> metadata1 = new KeyValuePair<Guid, byte[]>(Guid.NewGuid(), Enumerable.Repeat((byte)0x10, 100).ToArray());
			KeyValuePair<Guid, byte[]> metadata2 = new KeyValuePair<Guid, byte[]>(Guid.NewGuid(), Enumerable.Repeat((byte)0x20, 200).ToArray());
			KeyValuePair<Guid, byte[]> metadata3 = new KeyValuePair<Guid, byte[]>(Guid.NewGuid(), Enumerable.Repeat((byte)0x30, 300).ToArray());

			// Check the object reference
			Assert.IsNotNull(s_vdisk.Metadata);
			Assert.AreSame(s_vdisk.Metadata, s_vdisk.Metadata);

			// IsReadOnly
			Assert.IsFalse(s_vdisk.Metadata.IsReadOnly);

			// Add via KeyValuePair and key/value elements
			s_vdisk.Metadata.Add(metadata1);
			s_vdisk.Metadata.Add(metadata2);
			s_vdisk.Metadata.Add(metadata3.Key, metadata3.Value);
			Assert.AreEqual(3, s_vdisk.Metadata.Count);

			// Contains
			Assert.IsTrue(s_vdisk.Metadata.Contains(metadata1));
			Assert.IsTrue(s_vdisk.Metadata.Contains(metadata2));
			Assert.IsFalse(s_vdisk.Metadata.Contains(new KeyValuePair<Guid, byte[]>(Guid.NewGuid(), new byte[] { })));

			// ContainsKey
			Assert.IsTrue(s_vdisk.Metadata.ContainsKey(metadata1.Key));
			Assert.IsTrue(s_vdisk.Metadata.ContainsKey(metadata2.Key));
			Assert.IsFalse(s_vdisk.Metadata.ContainsKey(Guid.NewGuid()));

			// Enumerate with generic enumerator
			foreach (KeyValuePair<Guid, byte[]> item in s_vdisk.Metadata)
			{
				Assert.AreNotEqual(Guid.Empty, item.Key);
				Assert.AreNotEqual(0, item.Value.Length);
			}

			// Enumerate with non-generic enumerator
			foreach (Object obj in ((IEnumerable)s_vdisk.Metadata))
			{
				Assert.IsNotNull(obj);
			}

			// Keys and Values
			Assert.AreEqual(3, s_vdisk.Metadata.Keys.Count);
			Assert.AreEqual(3, s_vdisk.Metadata.Values.Count);

			// TryGetValue
			byte[] value;
			Assert.IsTrue(s_vdisk.Metadata.TryGetValue(metadata1.Key, out value));
			Assert.IsFalse(s_vdisk.Metadata.TryGetValue(Guid.NewGuid(), out value));

			// Indexer::get
			value = s_vdisk.Metadata[metadata1.Key];
			Assert.AreEqual(100, value.Length);
			value = s_vdisk.Metadata[metadata2.Key];
			Assert.AreEqual(200, value.Length);

			try { value = s_vdisk.Metadata[Guid.NewGuid()]; Assert.Fail(); }
			catch (Exception) { }

			// Indexer::set
			s_vdisk.Metadata[metadata1.Key] = Enumerable.Repeat((byte)0x40, 400).ToArray();
			s_vdisk.Metadata[metadata2.Key] = Enumerable.Repeat((byte)0x50, 500).ToArray();
			s_vdisk.Metadata[Guid.NewGuid()] = Enumerable.Repeat((byte)0x60, 600).ToArray();

			// CopyTo
			KeyValuePair<Guid, byte[]>[] localarray = new KeyValuePair<Guid, byte[]>[4];
			s_vdisk.Metadata.CopyTo(localarray, 0);

			try { s_vdisk.Metadata.CopyTo(localarray, 1); Assert.Fail(); }
			catch (Exception ex) { Assert.IsInstanceOfType(ex, typeof(IndexOutOfRangeException)); }

			// Remove via KeyValuePair
			s_vdisk.Metadata.Remove(metadata1);
			Assert.AreEqual(3, s_vdisk.Metadata.Count);

			// Remove via key
			s_vdisk.Metadata.Remove(metadata2.Key);
			Assert.AreEqual(2, s_vdisk.Metadata.Count);

			// Clear
			s_vdisk.Metadata.Clear();
			Assert.AreEqual(0, s_vdisk.Metadata.Count);
		}
	}
}
