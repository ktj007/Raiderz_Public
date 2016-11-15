
using System;
using System.Drawing;
using System.Windows.Forms;

namespace Sano.PersonalProjects.ColorPicker.Controls {
	
	internal struct ColorSwatch {

		private Color m_color;
		private string m_description;
		private Point m_location;
		private Size m_size;

		public string Description {
			get { return m_description; }
		}

		public Color Color {
			get { return m_color; }
		}

		public Point Location {
			get { return m_location; }
			set { m_location = value; }
		}

		public Size Size {
			get { return m_size; }
			set { m_size = value; }
		}

		public ColorSwatch( Color color, string description ) : 
			this( color, description, new Point( 0, 0 ), new Size( 10, 10 ) ) {

		}

		public ColorSwatch( Color color, string description, Point location, Size size ) {
			
			m_color = color;
			m_description = description;
			m_location = location; 
			m_size = size;

		}
	
		public override bool Equals( object obj ) {
			
			ColorSwatch swatch = ( ColorSwatch ) obj;
			bool isEquals = false;

			if ( this.Color == swatch.Color && this.Description.Equals( swatch.Description ) ) {
				isEquals = true;
			}

			return isEquals;
			
		}

		public override int GetHashCode() {
			return base.GetHashCode ();
		}

		public override string ToString() {
			return String.Format( "Description: {0}, Color: {1}", this.Description, this.Color );
		}

	}

}