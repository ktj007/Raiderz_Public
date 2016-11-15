
using System;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	internal sealed class RGB : IColorSpaceStructure {
		
		private int m_red;
		private int m_green;
		private int m_blue;

		internal int Red {
			get { return m_red; }
			set { m_red = value; }
		}

		internal int Green {
			get { return m_green; }
			set { m_green = value; }
		}

		internal int Blue {
			get { return m_blue; }
			set { m_blue = value; }
		}

		internal RGB( int red, int green, int blue ) {
			
			m_red = red;
			m_green = green;
			m_blue = blue;

		}

		public override string ToString() {
			return String.Format( "Red: {0}; Green: {1}; Blue: {2} ", Red, Green, Blue );
		}

		public override bool Equals( object obj ) {
			
			bool equal = false;

			if ( obj is RGB ) {
				
				RGB rgb = ( RGB ) obj;

				if ( this.Red == rgb.Red && this.Blue == rgb.Blue && this.Green == rgb.Green ) {
					equal = true;
				}
			
			}

			return equal;

		}

		public override int GetHashCode() {
			return base.GetHashCode ();
		}


	} // RGB

} // Sano.PersonalProjects.ColorPicker.Controls
