
using System;
using System.Runtime.InteropServices;

namespace Sano.Utility.NativeMethods {

	public class Gdi32 {
		
		[DllImport("gdi32.dll", SetLastError = true)]
		public static extern int StretchBlt( 
			IntPtr destDC,
			int dX, 
			int dY, 
			int dWidth,
			int dHeight,
			IntPtr sourceDC,
			int sX, 
			int sY,  
			int sWidth,
			int sHeight,
			int opCode );

		[DllImport("gdi32.dll", SetLastError = true)]
		public static extern int BitBlt( 
			IntPtr destDC,
			int x, 
			int y, 
			int nWidth, 
			int nHeight, 
			IntPtr sourceDC,
			int xSrc, 
			int ySrc, 
			int opCode );

		[DllImport("gdi32.dll", SetLastError = true)]
		public static extern IntPtr SelectObject( IntPtr hdc, IntPtr hgdiobj );

		[DllImport("gdi32.dll", SetLastError = true)]
		public static extern IntPtr DeleteObject( IntPtr hgdiobj );

		[DllImport("gdi32.dll", SetLastError = true)]
		public static extern IntPtr CreateCompatibleDC( IntPtr hdc );

		[DllImport("gdi32.dll", SetLastError = true)]
		public static extern IntPtr CreateCompatibleBitmap( IntPtr hdc, int width, int height );

		[DllImport("gdi32.dll", SetLastError = true)]
		public static extern bool DeleteDC( IntPtr hdc );

		/// <summary>
		/// Private constructor to ensure that the compiler does not 
		/// automatically generate a public constructor.
		/// </summary>

		private Gdi32() { }

	} // Gdi32

} // Sano.Utility.NativeMethods