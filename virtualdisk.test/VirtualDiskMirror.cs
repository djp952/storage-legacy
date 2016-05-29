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
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace zuki.storage.virtualdisk.test
{
	[TestClass()]
	public class VirtualDiskMirror
	{
		[TestMethod(), TestCategory("Mirror Virtual Disk")]
		public void VirtualDiskMirror_Synchronous()
		{
			string vhdxpath = Path.Combine(Environment.CurrentDirectory, "testvhdx.vhdx");
			if (File.Exists(vhdxpath)) File.Delete(vhdxpath);

			string mirrorpath = Path.Combine(Environment.CurrentDirectory, "testmirror.vhdx");
			if (File.Exists(mirrorpath)) File.Delete(mirrorpath);

			// Create an 8MB virtual disk with a non-standard block size
			VirtualDiskCreateParameters createparams = new VirtualDiskCreateParameters(vhdxpath, VirtualDiskType.VHDX, 8 * 1024 * 1024);
			createparams.BlockSize = 1024 * 1024;

			using (VirtualDisk vdisk = VirtualDisk.Create(createparams))
			{
				Assert.IsNotNull(vdisk);
				vdisk.Mirror(mirrorpath);
			}

			// Open the mirror and ensure that the virtual size and block size match the source
			using (VirtualDisk mirror = VirtualDisk.Open(mirrorpath))
			{
				Assert.IsNotNull(mirror);
				Assert.AreEqual((ulong)8 * 1024 * 1024, mirror.VirtualSize);
				Assert.AreEqual((ulong)1024 * 1024, mirror.BlockSize);
			}
		}
	}
}
