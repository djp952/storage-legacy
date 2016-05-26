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

			//disk.Compact();
			//

			//CancellationTokenSource cts = new CancellationTokenSource();

			//VirtualDisk vdisk = VirtualDisk.Open(@"D:\LINUX-BUILD.vhdx");
			//vdisk.CompactTest();
			//Thread.Sleep(60000);

			//Task t = vdisk.ExpandAsync(2 * 34359738368L, VirtualDiskExpandFlags.None, cts.Token, progress);
			////cts.CancelAfter(1000);
			//try
			//{
			//	await t;
			//}
			//catch (OperationCanceledException)
			//{
			//	int x = 123;
			//}

			//int y = 123;
		}
	}
}
