
using System;
using System.Runtime.InteropServices;

namespace Sano.Utility.NativeMethods {

	public class User32 {
		
		[DllImport("User32.dll", SetLastError = true)]
		private static extern bool StretchBlt( 
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

		[DllImport("user32.dll", SetLastError = true)]
		public static extern IntPtr GetDC( IntPtr windowHandle );
		
		[DllImport("user32.dll", SetLastError = true)]
		public static extern IntPtr GetWindowDC( IntPtr windowHandle );
		
		[DllImport("user32.dll", SetLastError = true)]
		public static extern int ReleaseDC( IntPtr windowHandle, IntPtr dc );
		
		[DllImport("user32.dll", SetLastError = true)]
		public static extern bool ShowWindow( IntPtr handle, int message );

		/// <summary>
		/// Private constructor to ensure that the compiler does not 
		/// automatically generate a public constructor.
		/// </summary>

		private User32() { }

	} // User32

} // Sano.Utility.NativeMethods