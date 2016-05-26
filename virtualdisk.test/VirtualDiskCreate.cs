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
	public class VirtualDiskCreate
	{
		[TestMethod(), TestCategory("Create Virtual Disk")]
		public void VirtualDiskCreate_ISO()
		{
			// You can't create ISOs with the virtual disk API - should always fail
			string isopath = Path.Combine(Environment.CurrentDirectory, "testiso.iso");
			try { using (VirtualDisk vdisk = VirtualDisk.Create(isopath, VirtualDiskType.ISO, 4 * 1024 * 1024)) { }; Assert.Fail(); }
			catch (Exception) { }
		}

		[TestMethod(), TestCategory("Create Virtual Disk")]
		public void VirtualDiskCreate_VHD()
		{
			string vhdpath = Path.Combine(Environment.CurrentDirectory, "testvhd.vhd");
			if (File.Exists(vhdpath)) File.Delete(vhdpath);

			using (VirtualDisk vdisk = VirtualDisk.Create(vhdpath, VirtualDiskType.VHD, 4 * 1024 * 1024))
			{
				Assert.IsNotNull(vdisk);
			}
		}

		[TestMethod(), TestCategory("Create Virtual Disk")]
		public void VirtualDiskCreate_VHDX()
		{
			string vhdxpath = Path.Combine(Environment.CurrentDirectory, "testvhdx.vhdx");
			if (File.Exists(vhdxpath)) File.Delete(vhdxpath);

			using (VirtualDisk vdisk = VirtualDisk.Create(vhdxpath, VirtualDiskType.VHDX, 4 * 1024 * 1024))
			{
				Assert.IsNotNull(vdisk);
			}
		}

		[TestMethod(), TestCategory("Create Virtual Disk")]
		public async Task VirtualDiskCreate_Asynchronous()
		{
			string vhdxpath = Path.Combine(Environment.CurrentDirectory, "testvhdx.vhdx");
			if (File.Exists(vhdxpath)) File.Delete(vhdxpath);

			var createparams = new VirtualDiskCreateParameters(vhdxpath, VirtualDiskType.VHDX, 4 * 1024 * 1024);
			using (VirtualDisk vdisk = await VirtualDisk.CreateAsync(createparams, CancellationToken.None, null))
			{
				Assert.IsNotNull(vdisk);
			}
		}
	}
}
