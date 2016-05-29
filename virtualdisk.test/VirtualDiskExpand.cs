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
	public class VirtualDiskExpand
	{
		[TestMethod(), TestCategory("Expand Virtual Disk")]
		public void VirtualDiskExpand_Parameters()
		{
			VirtualDiskExpandParameters params1 = new VirtualDiskExpandParameters();
			Assert.AreEqual((ulong)0, params1.NewSize);
			Assert.AreEqual(VirtualDiskExpandFlags.None, params1.Flags);

			VirtualDiskExpandParameters params2 = new VirtualDiskExpandParameters(4 * 1024 * 1024);
			Assert.AreEqual((ulong)(4 * 1024 * 1024), params2.NewSize);
			Assert.AreEqual(VirtualDiskExpandFlags.None, params2.Flags);

			VirtualDiskExpandParameters params3 = new VirtualDiskExpandParameters(8 * 1024 * 1024, VirtualDiskExpandFlags.None);
			Assert.AreEqual((ulong)(8 * 1024 * 1024), params3.NewSize);
			Assert.AreEqual(VirtualDiskExpandFlags.None, params3.Flags);
		}

		[TestMethod(), TestCategory("Expand Virtual Disk")]
		public void VirtualDiskExpand_DynamicVHD()
		{
			string vhdpath = Path.Combine(Environment.CurrentDirectory, "testvhd.vhd");
			if (File.Exists(vhdpath)) File.Delete(vhdpath);

			using (VirtualDisk vdisk = VirtualDisk.Create(vhdpath, VirtualDiskType.VHD, 4 * 1024 * 1024))
			{
				Assert.IsNotNull(vdisk);
				Assert.AreEqual((ulong)(4 * 1024 * 1024), vdisk.VirtualSize);

				vdisk.Expand(8 * 1024 * 1024);
				Assert.AreEqual((ulong)(8 * 1024 * 1024), vdisk.VirtualSize);
			}
		}

		[TestMethod(), TestCategory("Expand Virtual Disk")]
		public void VirtualDiskExpand_FixedVHD()
		{
			string vhdpath = Path.Combine(Environment.CurrentDirectory, "testvhd.vhd");
			if (File.Exists(vhdpath)) File.Delete(vhdpath);

			using (VirtualDisk vdisk = VirtualDisk.Create(vhdpath, VirtualDiskType.VHDX, 4 * 1024 * 1024, VirtualDiskCreateFlags.FullPhysicalAllocation))
			{
				Assert.IsNotNull(vdisk);
				Assert.AreEqual((ulong)(4 * 1024 * 1024), vdisk.VirtualSize);

				vdisk.Expand(8 * 1024 * 1024);
				Assert.AreEqual((ulong)(8 * 1024 * 1024), vdisk.VirtualSize);
			}
		}

		[TestMethod(), TestCategory("Expand Virtual Disk")]
		public void VirtualDiskExpand_DynamicVHDX()
		{
			string vhdxpath = Path.Combine(Environment.CurrentDirectory, "testvhdx.vhdx");
			if (File.Exists(vhdxpath)) File.Delete(vhdxpath);

			using (VirtualDisk vdisk = VirtualDisk.Create(vhdxpath, VirtualDiskType.VHDX, 4 * 1024 * 1024))
			{
				Assert.IsNotNull(vdisk);
				Assert.AreEqual((ulong)(4 * 1024 * 1024), vdisk.VirtualSize);

				vdisk.Expand(8 * 1024 * 1024);
				Assert.AreEqual((ulong)(8 * 1024 * 1024), vdisk.VirtualSize);
			}
		}

		[TestMethod(), TestCategory("Expand Virtual Disk")]
		public void VirtualDiskExpand_FixedVHDX()
		{
			string vhdxpath = Path.Combine(Environment.CurrentDirectory, "testvhdx.vhdx");
			if (File.Exists(vhdxpath)) File.Delete(vhdxpath);

			using (VirtualDisk vdisk = VirtualDisk.Create(vhdxpath, VirtualDiskType.VHDX, 4 * 1024 * 1024, VirtualDiskCreateFlags.FullPhysicalAllocation))
			{
				Assert.IsNotNull(vdisk);
				Assert.AreEqual((ulong)(4 * 1024 * 1024), vdisk.VirtualSize);

				vdisk.Expand(8 * 1024 * 1024);
				Assert.AreEqual((ulong)(8 * 1024 * 1024), vdisk.VirtualSize);
			}
		}

		[TestMethod(), TestCategory("Expand Virtual Disk")]
		public async Task VirtualDiskExpand_Async()
		{
			string vhdxpath = Path.Combine(Environment.CurrentDirectory, "testvhdx.vhdx");
			if (File.Exists(vhdxpath)) File.Delete(vhdxpath);

			using (VirtualDisk vdisk = VirtualDisk.Create(vhdxpath, VirtualDiskType.VHDX, 4 * 1024 * 1024, VirtualDiskCreateFlags.FullPhysicalAllocation))
			{
				Assert.IsNotNull(vdisk);
				Assert.AreEqual((ulong)(4 * 1024 * 1024), vdisk.VirtualSize);

				await vdisk.ExpandAsync(new VirtualDiskExpandParameters(8 * 1024 * 1024, VirtualDiskExpandFlags.None), CancellationToken.None, null);
				Assert.AreEqual((ulong)(8 * 1024 * 1024), vdisk.VirtualSize);
			}
		}
	}
}
