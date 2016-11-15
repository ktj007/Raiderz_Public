
using System;
using System.Runtime.InteropServices;

namespace Sano.Utility.NativeMethods {

	public class Window {
		
		[DllImport( "User32.dll" )]
		public static extern IntPtr ShowWindow( IntPtr hwnd, long cmdShow );
		
		/// <summary>
		/// Private constructor to ensure that the compiler does not 
		/// automatically generate a public constructor.
		/// </summary>

		private Window() {}

	} // Window

} // Sano.Utility.Win32