using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

using System.Diagnostics;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	internal class ColorRenderingHelper {

		private static Bitmap m_gradientBitmap;

		private ColorRenderingHelper() {}

		public static void DrawGreenColorField( Graphics g , int green ) {
			
			int red = 255;
			Rectangle rect;
			
			for ( int i=0; i < 256; i++ ) {
								
				rect = new Rectangle( 0, i, 256, 1 );
								
				using ( LinearGradientBrush lgb = new LinearGradientBrush( rect, Color.FromArgb( red, green, 0 ), Color.FromArgb( red, green, 255 ), LinearGradientMode.Horizontal ) ) {
					g.FillRectangle( lgb, rect );
				}

				red--;

			}

		}

		public static Bitmap GetGreenColorField( int green ) {

			Bitmap bmp = new Bitmap( 256, 256 );
		
			using ( Graphics g = Graphics.FromImage( bmp ) ) {
				DrawGreenColorField( g, green );
			}

			return bmp;

		}

		public static void DrawBlueColorField( Graphics g, int blue ) {

			int green = 255;
			Rectangle rect;
				
			for ( int i=0; i < 256; i++ ) {
							
				rect = new Rectangle( 0, i, 256, 1 );

				using ( LinearGradientBrush lgb = new LinearGradientBrush( rect, Color.FromArgb( 0, green, blue ), Color.FromArgb( 255, green, blue ), LinearGradientMode.Horizontal ) ) {
					g.FillRectangle( lgb, rect );
				}

				green--;
				
			}
		
		}		

		public static Bitmap GetBlueColorField( int blue ) {

			Bitmap bmp = new Bitmap( 256, 256 );
		
			using ( Graphics g = Graphics.FromImage( bmp ) ) {
				DrawBlueColorField( g, blue );
			}

			return bmp;

		}

		public static void DrawRedColorField( Graphics g, int red ) {
						
			int green = 255;
			Rectangle rect;
			
			for ( int i=0; i < 256; i++ ) {
					
				rect = new Rectangle( 0, i, 256, 1 );

				using ( LinearGradientBrush lgb = new LinearGradientBrush( rect, Color.FromArgb( red, green, 0 ), Color.FromArgb( red, green, 255 ), 0f, false ) ) {
					g.FillRectangle( lgb , new Rectangle( 0, i, 256, 1 ) );
				}


				green--;
				
			}
						
		}

		public static Bitmap GetRedColorField( int red ) {

			Bitmap bmp = new Bitmap( 256, 256 );
		
			using ( Graphics g = Graphics.FromImage( bmp ) ) {
				DrawRedColorField( g, red );
			}

			return bmp;

		}

		public static void DrawHueColorField( Graphics g, Color sliderColor ) {

			Color endColor = sliderColor;
			double redIndex = 	( double ) ( 255 - endColor.R ) / 255;
			double blueIndex = ( double ) ( 255 - endColor.B ) / 255;
			double greenIndex = ( double ) ( 255 - endColor.G ) / 255;		

			double cR = 255;
			double cG = 255;
			double cB = 255;

				
			for ( int x=0; x < 256; x++ ) {
							
				using ( LinearGradientBrush lgb = new LinearGradientBrush( new Rectangle( x, 0, 1, 256 ), Color.FromArgb( ( int ) Math.Round( cR ), ( int ) Math.Round( cG ), ( int ) Math.Round( cB ) ), Color.FromArgb( 0, 0, 0 ), 90f, false ) ) {
					g.FillRectangle( lgb, new Rectangle( x, 0, 1, 256 ) );
				}

				cR = cR - redIndex;
				cG = cG - greenIndex;
				cB = cB - blueIndex;

			}
				
		}

		public static Bitmap GetHueColorField( Color color ) {

			Bitmap bmp = new Bitmap( 256, 256 );
		
			using ( Graphics g = Graphics.FromImage( bmp ) ) {
				DrawHueColorField( g, color );
			}

			return bmp;

		}

		public static void DrawSaturationColorField( Graphics g, int saturation ) {

			Rectangle rect = new Rectangle( 0, 0, 256, 256 );
			int saturatedColorValue = ( int )( 255 - Math.Round( 255 * ( ( double ) saturation / 100 ) ) );

			if ( m_gradientBitmap == null ) {				
				m_gradientBitmap = DrawMultiGradientBitmap();
			} 

			g.DrawImage( m_gradientBitmap, 0, 0 );

			Color startColor = Color.FromArgb( saturatedColorValue, 255, 255, 255 );
			using ( LinearGradientBrush lgb = new LinearGradientBrush( rect, startColor, Color.Black, 90f ) ) {
				g.FillRectangle( lgb, rect );
			}
			
			
		}

		public static Bitmap GetSaturationColorField( int saturation ) {
			
			Bitmap bmp = new Bitmap( 256, 256 );

			using( Graphics g = Graphics.FromImage( bmp ) ) {
				DrawSaturationColorField( g, saturation );
			}

			return bmp;

		}

		public static void DrawBrightnessColorField( Graphics g, int brightness ) {

			int alpha = ( int )( 255 - Math.Round( brightness * 2.55 ) );
			
			if ( m_gradientBitmap == null ) {
				m_gradientBitmap = DrawMultiGradientBitmap();
			} 

			g.DrawImage( m_gradientBitmap, 0, 0 );

			using ( SolidBrush sb = new SolidBrush( Color.FromArgb( alpha, 0, 0, 0 ) ) ) {
				g.FillRectangle( sb, 0, 0, 256, 256 );
			}

		}

		public static Bitmap GetBrightnessColorField( int brightness ) {
			
			Bitmap bmp = new Bitmap( 256, 256 );

			using( Graphics g = Graphics.FromImage( bmp ) ) {
				DrawBrightnessColorField( g, brightness );
			}

			return bmp;
		}

		private static Bitmap DrawMultiGradientBitmap() {

			Rectangle rect;
			int colorValue = 255;
			Bitmap bmp = new Bitmap( 256, 256 );

			using ( Graphics gBmp = Graphics.FromImage( bmp ) ) {

				for ( int i=0; i < 256; i++ ) {

					rect = new Rectangle( 0, i, 256, 1 );

					using ( LinearGradientBrush brBrush = new LinearGradientBrush( rect, Color.Blue, Color.Red, 0f, false ) ) {

						Color[] colorArray = {	
												 Color.FromArgb( colorValue, i, i ), // red 
												 Color.FromArgb( colorValue, colorValue, i ), // yellow 
												 Color.FromArgb( i, colorValue, i ), // green 
												 Color.FromArgb( i, colorValue, colorValue ), // cyan 
												 Color.FromArgb( i, i, colorValue ), // blue 
												 Color.FromArgb( colorValue, i, colorValue ), // magneta
												 Color.FromArgb( colorValue, i, i ) // red
											 };

						float[] posArray = { 0.0f, 0.1667f, 0.3372f, 0.502f, 0.6686f, 0.8313f, 1.0f };

						ColorBlend colorBlend = new ColorBlend();
						colorBlend.Colors = colorArray;
						colorBlend.Positions = posArray;
						brBrush.InterpolationColors = colorBlend;

						gBmp.FillRectangle( brBrush, rect );
				
					}
							
				}
					
			} 

			return bmp;

		}

	}

}