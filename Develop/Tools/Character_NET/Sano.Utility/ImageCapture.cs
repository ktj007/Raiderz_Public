
using System;	
using System.Drawing;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;

using System.Diagnostics;

namespace Sano.Utility {

	public sealed class Imaging {

		internal const int SRCCOPY = 0x00CC0020;
		internal const int CAPTUREBLT = 0x40000000;
		internal const int SW_HIDE = 0x00000000;
		internal const int SW_SHOW = 0x00000006;

		public enum RasterOperations {

			SRCCOPY     = 0x00CC0020, // dest = source
			SRCPAINT    = 0x00EE0086, // dest = source OR dest
			SRCAND      = 0x008800C6, // dest = source AND dest
			SRCINVERT   = 0x00660046, // dest = source XOR dest
			SRCERASE    = 0x00440328, // dest = source AND (NOT dest )
			NOTSRCCOPY  = 0x00330008, // dest = (NOT source)
			NOTSRCERASE = 0x001100A6, // dest = (NOT src) AND (NOT dest) 
			MERGECOPY   = 0x00C000CA, // dest = (source AND pattern)
			MERGEPAINT  = 0x00BB0226, // dest = (NOT source) OR dest
			PATCOPY     = 0x00F00021, // dest = pattern
			PATPAINT    = 0x00FB0A09, // dest = DPSnoo
			PATINVERT   = 0x005A0049, // dest = pattern XOR dest
			DSTINVERT   = 0x00550009, // dest = (NOT dest)
			BLACKNESS   = 0x00000042, // dest = BLACK
			WHITENESS   = 0x00FF0062, // dest = WHITE
		
		};
		
		#region Unmanaged declarations

		[DllImport("gdi32.dll", ExactSpelling=true, SetLastError=true)]
		private static extern bool BitBlt( 
			IntPtr targetDC,
			int targetX, 
			int targetY, 
			int targetWidth, 
			int targetHeight, 
			IntPtr sourceDC,
			int sourceStartX, 
			int sourceStartY, 
			RasterOperations ro );

		[DllImport("gdi32.dll", ExactSpelling=true, SetLastError=true)]
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
			RasterOperations ro );

		[DllImport("user32.dll", ExactSpelling=true, SetLastError=true)]
		static extern IntPtr GetDesktopWindow();

		[DllImport("user32.dll", ExactSpelling=true, SetLastError=true)]
		private static extern IntPtr GetWindowDC( IntPtr windowHandle );
		
		[DllImport("user32.dll", ExactSpelling=true, SetLastError=true)]
		private static extern int ReleaseDC( IntPtr windowHandle, IntPtr dc );
		
		[DllImport("user32.dll", ExactSpelling=true, SetLastError=true)]
		private static extern bool ShowWindow( IntPtr handle, int message );

		[DllImport("user32.dll", ExactSpelling=true, SetLastError=true)]
		static extern int GetWindowLong(IntPtr hWnd, int nIndex);

		[DllImport("user32.dll", ExactSpelling=true, SetLastError=true)]
		static extern int SetWindowLong(IntPtr hWnd, int nIndex, int dwNewLong );

		[DllImport("user32.dll", ExactSpelling=true, SetLastError=true)]
		private static extern bool SetLayeredWindowAttributes(HandleRef hwnd, int crKey, byte bAlpha, int dwFlags);

		[DllImport("gdi32.dll", ExactSpelling=true, SetLastError=true)]
		static extern IntPtr CreateCompatibleDC(IntPtr hdc);

		[DllImport("gdi32.dll", ExactSpelling=true, SetLastError=true)]
		static extern bool DeleteDC(IntPtr hdc);

		[DllImport("gdi32.dll", ExactSpelling=true, SetLastError=true)]
		static extern IntPtr SelectObject(IntPtr hdc, IntPtr hgdiobj);

		[DllImport("gdi32.dll", ExactSpelling=true, SetLastError=true)]
		static extern bool DeleteObject(IntPtr hObject);

		#endregion

//		public static Image CaptureDesktop() {
//				
//			Size s = new Size( Screen.PrimaryScreen.Bounds.Width, Screen.PrimaryScreen.Bounds.Width );
//			return CaptureScreenRegion( new Point( 0, 0 ), s );
//
//		}

		public static Image CaptureActiveWindow() {
						
			System.Windows.Forms.SendKeys.Send( "%{PRTSC}" );
			
			Thread.Sleep( 10 ); // just to make sure it gets added to the clipboard
			
			Bitmap bmp = null;
			if ( Clipboard.GetDataObject().GetDataPresent( DataFormats.Bitmap ) ) {
				bmp = ( Bitmap ) Clipboard.GetDataObject().GetData( DataFormats.Bitmap );
			}

			return bmp;

		}

		private static Image CaptureScreenRegion( Point location, Size size, RasterOperations opcode ) {

			Image myImage = new Bitmap( size.Width, size.Height );

			using ( Graphics g = Graphics.FromImage( myImage ) ) {

				IntPtr destDeviceContext = g.GetHdc();
				IntPtr srcDeviceContext = GetWindowDC( IntPtr.Zero ); // capture desktop
			
				// TODO: throw exception
				if ( !BitBlt( destDeviceContext, 0, 0, size.Width, size.Height, srcDeviceContext, location.X, location.Y,  opcode ) ) {
					// throw exception
				}
		
				ReleaseDC( IntPtr.Zero, srcDeviceContext );
				g.ReleaseHdc( destDeviceContext );

			} // dispose the Graphics object

			return myImage;

		}

//		public static Image CaptureScreenRegion( Point location, Size size ) {
//			return CaptureScreenRegion( location, size, RasterOperations.SRCCOPY | CAPTUREBLT );
//		}

//		public static Image CaptureScreenRegionWithoutActiveWindow( HandleRef hwnd, Point location, Size size ) {
//			return CaptureScreenRegion( location, size, RasterOperations.SRCCOPY );
//		}

		public static Image CopyImageBits( Image currentImage, Rectangle region ) {
			
			Image newImage = new Bitmap( region.Width, region.Height );
			
			using ( Graphics g = Graphics.FromImage( newImage ) ) {
				g.DrawImage( currentImage, 0, 0, region, GraphicsUnit.Pixel );
			}

			return newImage;

		}

		public static Bitmap CopyBitmapBitsUsingBitBlt( Bitmap currentImage, Rectangle region ) {
			
			Bitmap newImage = new Bitmap( region.Width, region.Height );
			using( Graphics g = Graphics.FromHdc( GetWindowDC( IntPtr.Zero ) ) ) {
				g.DrawRectangle( Pens.Black, 0, 0, 100, 100 );
			}

//			using ( Graphics g = Graphics.FromImage( newImage ) ) {
//				
//				IntPtr target = g.GetHdc();
////				IntPtr source = CreateCompatibleDC( IntPtr.Zero );
//				IntPtr source = GetWindowDC( IntPtr.Zero );
//				CreateCompatibleDC(
//				IntPtr original = SelectObject( source, currentImage.GetHbitmap() );
//				BitBlt( target, 0, 0, region.Width, region.Height, source, region.X, region.Y, RasterOperations.SRCCOPY );
//				IntPtr pnew = SelectObject( source, original );
//				DeleteObject( pnew );
//				DeleteDC( source );
//				g.ReleaseHdc( target );
//
////				g.DrawImage( currentImage, 0, 0, region, GraphicsUnit.Pixel );
//			
//			}

			return newImage;

		}


		public static Image StretchImage( Point p, Rectangle zoomRegion, Size desiredSize, RasterOperations opcode ) {

			Image myImage = new Bitmap( desiredSize.Width, desiredSize.Height );

			using ( Graphics g = Graphics.FromImage( myImage ) ) {
		
				IntPtr destDeviceContext = g.GetHdc();
				IntPtr srcDeviceContext = GetWindowDC( IntPtr.Zero ); // capture desktop
						
//				StretchBlt( destDeviceContext, 0, 0, desiredSize.Width, desiredSize.Height, srcDeviceContext, p.X, p.Y, zoomRegion.Width, zoomRegion.Height, SRCCOPY | CAPTUREBLT );
				StretchBlt( destDeviceContext, 0, 0, desiredSize.Width, desiredSize.Height, srcDeviceContext, p.X, p.Y, zoomRegion.Width, zoomRegion.Height, RasterOperations.SRCCOPY );
				//Debug.WriteLine( "Error: " + Marshal.GetLastWin32Error() );

				ReleaseDC( IntPtr.Zero, srcDeviceContext );
				g.ReleaseHdc( destDeviceContext );

			} // dispose the Graphics object

			return myImage;

		}

		public static Image StretchImage( Point p, Rectangle zoomRegion, Size desiredSize, RasterOperations opcode, bool captureLayeredWindows ) {

			Image myImage = new Bitmap( desiredSize.Width, desiredSize.Height );

			using ( Graphics g = Graphics.FromImage( myImage ) ) {
		
				IntPtr destDeviceContext = g.GetHdc();
				IntPtr srcDeviceContext = GetWindowDC( IntPtr.Zero ); // capture desktop
						
				StretchBlt( destDeviceContext, 0, 0, desiredSize.Width, desiredSize.Height, srcDeviceContext, p.X, p.Y, zoomRegion.Width, zoomRegion.Height, opcode );
//				StretchBlt( destDeviceContext, 0, 0, desiredSize.Width, desiredSize.Height, srcDeviceContext, p.X, p.Y, zoomRegion.Width, zoomRegion.Height, SRCCOPY | CAPTUREBLT );
				//Debug.WriteLine( "Error: " + Marshal.GetLastWin32Error() );

				ReleaseDC( IntPtr.Zero, srcDeviceContext );
				g.ReleaseHdc( destDeviceContext );

			} // dispose the Graphics object

			return myImage;

		}
		
	}
	
}