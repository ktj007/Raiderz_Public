
using System;
using System.Drawing;
using System.Runtime.CompilerServices;
using System.Text;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	/// <summary>
	/// Provides color conversion functionality.
	/// </summary>

	internal class ColorConverter {

		/// <summary>
		/// Private constructor to ensure that the compiler does not 
		/// automatically generate a public constructor.
		/// </summary>
		
		private ColorConverter() {}

		/// <summary>
		/// Converts a Color to RGB.
		/// </summary>
		/// <param name="color">A Color object representing the color that is
		/// to be converted to RGB.</param>
		/// <returns>A RGB equivalent.</returns>

		internal static RGB ColorToRgb( Color color ) {
			return new RGB( color.R, color.G, color.B );
		}

		
		/// <summary>
		/// Converts a RGB color structure to a Color object.
		/// </summary>
		/// <param name="rgb">A RGB object representing the color that is to be
		/// converted.</param>
		/// <returns>A Color equivalent.</returns>
		
		internal static Color RgbToColor( RGB rgb ) {
			return Color.FromArgb( rgb.Red, rgb.Green, rgb.Blue );
		}


		/// <summary>
		/// Converts RGB to HSB.
		/// </summary>
		/// <param name="rgb">A RGB object containing the RGB values that are to 
		/// be converted to HSB values.</param>
		/// <returns>A HSB equivalent.</returns>

		internal static HSB RgbToHsb( RGB rgb, int nHue ) {

			// NOTE #1: Even though we're dealing with a very small range of
			// numbers, the accuracy of all calculations is fairly important.
			// For this reason, I've opted to use double data types instead
			// of float, which gives us a little bit extra precision (recall
			// that precision is the number of significant digits with which
			// the result is expressed).

			double r = rgb.Red / 255d;
			double g = rgb.Green / 255d;
			double b = rgb.Blue / 255d;

			double minValue = GetMinimumValue( r, g, b );
			double maxValue = GetMaximumValue( r, g, b );
			double delta = maxValue - minValue;

            double hue = nHue;
			double saturation = 0;
			double brightness = maxValue * 100;

			if ( maxValue == 0 || delta == 0 ) {

				saturation = 0;

			} else {

				// NOTE #2: FXCop insists that we avoid testing for floating 
				// point equality (CA1902). Instead, we'll perform a series of
				// tests with the help of Double.Epsilon that will provide 
				// a more accurate equality evaluation.

				if ( minValue == 0 ) {
					saturation = 100;
				} else {
					saturation = ( delta / maxValue ) * 100;
				}
			}

			return new HSB( 
				( int ) Math.Round( hue ),
				( int ) Math.Round( saturation ),
				( int ) Math.Round( brightness ) );

		} // RgbToHsb


		/// <summary>
		/// Converts HSB to RGB.
		/// </summary>
		/// <param name="rgb">A HSB object containing the HSB values that are to 
		/// be converted to RGB values.</param>
		/// <returns>A RGB equivalent.</returns>

		internal static RGB HsbToRgb( HSB hsb ) {

			double h, s, b;
			double red = 0, green = 0, blue = 0;
			
			h = hsb.Hue;
			s = ( ( double ) hsb.Saturation ) / 100;
			b = ( ( double ) hsb.Brightness ) / 100;

			if ( s == 0 ) {
				
				red = b;
				green = b;
				blue = b;

			} else {
				
				double p, q, t;

				// the color wheel has six sectors.
				double fractionalSector;
				int sectorNumber;
				double sectorPosition;

				sectorPosition = h / 60;
				sectorNumber = ( int ) Math.Floor( sectorPosition );
				fractionalSector = sectorPosition - sectorNumber;

				p = b * ( 1 - s );
				q = b * ( 1 - ( s * fractionalSector ) );
				t = b * ( 1 - ( s * ( 1 - fractionalSector ) ) );

				// Assign the fractional colors to r, g, and b
				// based on the sector the angle is in.
				switch (sectorNumber) {
					case 0:
						red = b;
						green = t;
						blue = p;
						break;

					case 1:
						red = q;
						green = b;
						blue = p;
						break;

					case 2:
						red = p;
						green = b;
						blue = t;
						break;

					case 3:
						red = p;
						green = q;
						blue = b;
						break;

					case 4:
						red = t;
						green = p;
						blue = b;
						break;

					case 5:
						red = b;
						green = p;
						blue = q;
						break;
				}

			}

			int nRed, nGreen, nBlue;
			nRed = ( int ) Math.Round( red * 255 );
			nGreen = ( int ) Math.Round( green * 255 );
			nBlue = ( int ) Math.Round( blue * 255 );

			return new RGB( nRed, nGreen, nBlue );

		} // HsbToRgb


		/// <summary>
		/// Determines the maximum value of all of the numbers provided in the
		/// variable argument list.
		/// </summary>
		/// <param name="values">An array of doubles.</param>
		/// <returns>The maximum value.</returns>
		
		internal static double GetMaximumValue( params double[] values ) {

			double maxValue = values[0];

			if ( values.Length >= 2 ) {

				double num;

				for ( int i=1; i < values.Length; i++ ) {
			
					num = values[i];
					maxValue = Math.Max( maxValue, num );

				}

			}

			return maxValue;

		} // GetMaximumValue


		/// <summary>
		/// Determines the minimum value of all of the numbers provided in the
		/// variable argument list.
		/// </summary>
		/// <param name="values">An array of doubles.</param>
		/// <returns>The minimum value.</returns>
		
		internal static double GetMinimumValue( params double[] values ) {

			double minValue = values[0];

			if ( values.Length >= 2 ) {

				double num;

				for ( int i=1; i < values.Length; i++ ) {
			
					num = values[i];
					minValue = Math.Min( minValue, num );

				}

			}

			return minValue;

		} // GetMinimumValue


		/// <summary>
		/// Creates a Color object based on the parameterized hexadecimal 
		/// string value.
		/// </summary>
		/// <param name="hexValue">A string object representing the hexadecimal 
		/// value.</param>
		/// <returns>A Color equivalent.</returns>

		internal static Color HexToColor( string hexValue ) {

			// TODO: Should probably implement a regex to validate the incoming
			// string, but since this is an internal class, it isn't too much
			// of a concern. For now, we'll just check for the empty string.
			
			string localHex = hexValue;

			if ( hexValue == null | hexValue.Length == 0 ) {
				localHex = "0";
			} 

			// NOTE #1: ColorTranslator.FromHtml() could also have been used 
			// here. 

			// prepend the string with 0x.
			localHex = String.Format( "0x{0}", localHex );

			// convert the string to a hexidecimal based integer.
			int colorInt = Convert.ToInt32( localHex, 16 );

			// use a combination of bit shifting and the binary operator to 
			// calculate the color based on the distribution of bits.
			return Color.FromArgb( 
				( colorInt & 0xFF0000 ) >> 16,	// shift 16 bits
				( colorInt & 0x00FF00 ) >> 8,	// shift 8 bits
				colorInt & 0x0000FF );			// no shifting

		}

		/// <summary>
		/// Creates a RGB object based on the parameterized hexadecimal
		/// string value.
		/// </summary>
		/// <param name="hexValue">A string object representing the hexadecimal 
		/// value.</param>
		/// <returns>A Color equivalent.</returns>

		internal static RGB HexToRgb( string hexValue ) {
			
			Color c = HexToColor( hexValue );
			return new RGB( c.R, c.G, c.B );
		
		}

	} // ColorConverter

} // Sano.PersonalProjects.ColorPicker.Controls
