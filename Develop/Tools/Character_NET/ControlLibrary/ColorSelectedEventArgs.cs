
using System;
using System.Drawing;
using System.Windows.Forms;

namespace Sano.PersonalProjects.ColorPicker.Controls {
	
	internal class ColorSelectedEventArgs : EventArgs {
		
		private Color m_color;
		
		internal Color Color {
			get { return m_color; }
		}

		internal ColorSelectedEventArgs( Color color ) {
			m_color = color;
		}

	}

}