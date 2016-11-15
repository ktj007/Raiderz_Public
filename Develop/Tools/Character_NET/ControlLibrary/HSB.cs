
using System;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	internal sealed class HSB : IColorSpaceStructure {
		
		private int m_hue;
		private int m_saturation;
		private int m_brightness;

		internal int Hue {
			get { return m_hue; }
			set { m_hue = value; }
		}

		internal int Saturation {
			get { return m_saturation; }
			set { m_saturation = value; }
		}

		internal int Brightness {
			get { return m_brightness; }
			set { m_brightness = value; }
		}

		internal HSB( int hue, int saturation, int brightness ) {
			
			m_hue = hue;
			m_saturation = saturation;
			m_brightness = brightness;

		}

		public override string ToString() {
			return String.Format( "Hue: {0}; Saturation: {1}; Brightness: {2} ", Hue, Saturation, Brightness );
		}

		public override bool Equals( object obj ) {
			
			bool equal = false;

			if ( obj is HSB ) {
				
				HSB hsb = ( HSB ) obj;

				if ( this.Hue == hsb.Hue && this.Saturation == hsb.Saturation && this.Brightness == hsb.Brightness ) {
					equal = true;
				}
			
			}

			return equal;

		}

		public override int GetHashCode() {
			return base.GetHashCode ();
		}

	} // HSB

} // Sano.PersonalProjects.ColorPicker.Controls
