
using System;
using System.Drawing;
using Sano.Utility.NativeMethods;

namespace Sano.Utility {

	/// <summary>
	/// This class provides an array of image manipulation functionality.
	/// </summary>

	public sealed class ImageManipulation {
		
		public static Image StretchImage( Image sourceImage, Point p, Rectangle zoomRegion, Size desiredSize ) {

			Image myImage = new Bitmap( desiredSize.Width, desiredSize.Height );
	
			using ( Graphics g = Graphics.FromImage( myImage ) ) {
		
				// use the DrawImage method in the framework to achieve a 
				// similar, but not as pixelated effect.
//				g.DrawImage( sourceImage, 
//					new Rectangle( new Point( 0 ), desiredSize ),
//					zoomRegion,
//					GraphicsUnit.Pixel );

				// we're dealing with memory device contexts here. first, a 
				// device context for the screen is retrieved. this serves as the
				// handle to the device context. we then create a memory device 
				// context that is compatible with the device context of the
				// screen (desktop).
				
				IntPtr hdc = User32.GetDC( IntPtr.Zero );
				IntPtr memoryDC = Gdi32.CreateCompatibleDC( hdc );

				// we retrieve a handle to the source bitmap object. this is 
				// then used to select the bitmap into the memory device 
				// context.
				
				IntPtr hBitmap = ( ( Bitmap ) sourceImage ).GetHbitmap();
				IntPtr oldObject = Gdi32.SelectObject( memoryDC, hBitmap );
				
				// Delete the object that was previously selected into the 
				// memory device context (there is a default bitmap object
				// that is encapsulated by the returned object, so we need
				// to make sure we get rid of that, otherwise we'll have
				// gdi object leaks).

				Gdi32.DeleteObject( oldObject );

				// the handle to the destination device context is retrieved.
				IntPtr destDeviceContext = g.GetHdc();

				// the source bitmap is stretched and its bits are blitted to 
				// the canvas represented by the destination device context.

				int result = Gdi32.StretchBlt( destDeviceContext, 0, 0, desiredSize.Width, desiredSize.Height, memoryDC, p.X, p.Y, zoomRegion.Width, zoomRegion.Height, RasterOperationCodes.SRCCOPY );				

				// error checking - StretchBlt returns non-zero on success.

				if ( result == 0 ) {

					// TODO: call GetLastError to dig down to the core of the problem.
					throw new ImageManipulationException( "There was a problem with the StretchBlt function call." );
				
				}
				
				// release everything
				g.ReleaseHdc( destDeviceContext );

				// clear the previously selected bitmap object from the memory
				// device context.
				Gdi32.DeleteObject( hBitmap );

				// get rid of the memory device context.
				Gdi32.DeleteDC( memoryDC );

				// return the device context to the device context pool.
				User32.ReleaseDC( IntPtr.Zero, hdc );

			} // dispose the Graphics object

			return myImage;

		}

		public static Image CaptureDesktopRegion( Point location, Size size ) {
				
			Image myImage = new Bitmap( size.Width, size.Height );

			using ( Graphics g = Graphics.FromImage( myImage ) ) {

				IntPtr destDeviceContext = g.GetHdc();
				IntPtr srcDeviceContext = User32.GetWindowDC( IntPtr.Zero ); // capture desktop
						
				// TODO: throw exception
				int result = Gdi32.BitBlt( destDeviceContext, 0, 0, size.Width, size.Height, srcDeviceContext, location.X, location.Y, RasterOperationCodes.SRCCOPY );

				if ( result == 0 ) {

					// TODO: call GetLastError to dig down to the core of the problem.
					throw new ImageManipulationException( "There was a problem with the BitBlt function call." );

				}
		
				User32.ReleaseDC( IntPtr.Zero, srcDeviceContext );
				g.ReleaseHdc( destDeviceContext );

			} // dispose the Graphics object

			return myImage;

		}

	} // ImageManipulation

} // Sano.Utility