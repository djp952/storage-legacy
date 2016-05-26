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
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace zuki.storage.virtualdisk.test
{
	[TestClass()]
	public class VirtualDiskOpen
	{
		void OnProgress(int value)
		{
			System.Diagnostics.Debug.WriteLine(value.ToString());
		}

		[TestMethod(), TestCategory("Open Virtual Disk")]
		public async Task VirtualDisk_OpenPath()
		{
			if (System.IO.File.Exists("D:\\test.vhdx")) System.IO.File.Delete("D:\\test.vhdx");

			IProgress<int> progress = new Progress<int>(new Action<int>(OnProgress));
			VirtualDiskCreateParameters createparams = new VirtualDiskCreateParameters("D:\\test.vhdx", VirtualDiskType.VHDX, 8 * 1024 * 1024);
			createparams.Flags = VirtualDiskCreateFlags.FullPhysicalAllocation;

			Task<VirtualDisk> task = VirtualDisk.CreateAsync(createparams, CancellationToken.None, progress);
			VirtualDisk disk = await task;

			Guid g = Guid.NewGuid();
			disk.Metadata.Add(g, new byte[] { 1, 2, 3 });
			byte[] newbytes;

			int y = 123;
		}
	}
}
