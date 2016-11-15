
using System;
using System.Runtime.InteropServices;

namespace Sano.Utility.NativeMethods {

	public class Caret {
		
		[DllImport( "user32.dll" )]
		public static extern int HideCaret( IntPtr hwnd );

		[DllImport( "user32.dll" )]
		public static extern int ShowCaret( IntPtr hwnd );

		/// <summary>
		/// Private constructor to ensure that the compiler does not 
		/// automatically generate a public constructor.
		/// </summary>

		private Caret() {}

	} // Caret

} // Sano.Utility.Win32