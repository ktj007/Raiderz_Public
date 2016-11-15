using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Globalization;
using System.Windows.Forms;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	/// <summary>
	/// RGB color space.
	/// </summary>

	internal class RgbColorSpace : ColorSpace {

		/// data fields
		private ColorSpaceComponent m_csRed;
		private ColorSpaceComponent m_csGreen;
		private ColorSpaceComponent m_csBlue;

		/// <summary>
		/// Gets or sets a value containing the coordinates of this color 
		/// space.
		/// </summary>

		internal override IColorSpaceStructure Structure {
			
			get {
				return new RGB( m_csRed.Value, m_csGreen.Value, m_csBlue.Value );
			}

			set {
				
				RGB rgb = ( RGB ) value;

				m_csRed.Value = rgb.Red;
				m_csGreen.Value = rgb.Green;
				m_csBlue.Value = rgb.Blue;

			}

		}

		/// <summary>
		/// Constructor. Adds the color space components to the color space
		/// component collection.
		/// </summary>

		internal RgbColorSpace() {

			InitializeComponent();
		
			ColorSpaceComponents.Add( m_csRed );
			ColorSpaceComponents.Add( m_csBlue );
			ColorSpaceComponents.Add( m_csGreen );

		}

		/// <summary>
		/// Sets the default color space component.
		/// </summary>

		internal override void SetDefaultSelection() {
			ChangeCurrentlySelectedComponent( m_csRed );
		}

		/// <summary>
		/// Converts the coordinates represented by this color space to its
		/// equivalent Color representation.
		/// </summary>
		/// <returns>A Color object.</returns>

		internal override Color GetColor() {
			return Color.FromArgb( m_csRed.Value, m_csGreen.Value, m_csBlue.Value );
		}
				
		/// <summary>
		/// Converts the coordinates represented by this color space to its 
		/// equivalent HEX representation.
		/// </summary>
		/// <returns>A string containing a hexadecimal value.</returns>

		internal string ConvertToHex() {
			return String.Format( "{0:x2}{1:x2}{2:x2}", m_csRed.Value, m_csGreen.Value, m_csBlue.Value ).ToUpper();
		}

		/// <summary>
		/// Updates the color space coordinate values.
		/// </summary>
		/// <param name="csStructure">A IColorSpaceStructure object containing 
		/// values that are to be mapped to the coordinates of this color 
		/// space.</param>
		
		protected override void UpdateValues( IColorSpaceStructure csStructure ) {
			
			RGB rgb = ( RGB ) csStructure;
			
			m_csRed.Value = rgb.Red;
			m_csGreen.Value = rgb.Green;
			m_csBlue.Value = rgb.Blue;

		}

		#region Component Designer generated code

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			this.m_csRed = new ColorSpaceComponent();
			this.m_csBlue = new ColorSpaceComponent();
			this.m_csGreen = new ColorSpaceComponent();
			this.SuspendLayout();
			// 
			// m_csRed
			// 
			this.m_csRed.DisplayCharacter = 'R';
			this.m_csRed.Location = new System.Drawing.Point(8, 8);
			this.m_csRed.Name = "m_csRed";
			this.m_csRed.Selected = false;
			this.m_csRed.Size = new System.Drawing.Size(96, 32);
			this.m_csRed.TabIndex = 0;
			this.m_csRed.Unit = ComponentUnit.Value;
			this.m_csRed.Value = 0;
			this.m_csRed.ComponentTextKeyUp += new ColorSpaceComponentEventHandler(ComponentTextKeyUp);
			this.m_csRed.ComponentSelected += new ColorSpaceComponentEventHandler(this.ComponentSelected);
			//			this.m_csRed.ComponentTextLostFocus += new ColorSpaceComponent.ComponentTextLostFocusHandler(this.ComponentTextLostFocus);
			// 
			// m_csBlue
			// 
			this.m_csBlue.DisplayCharacter = 'B';
			this.m_csBlue.Location = new System.Drawing.Point(8, 56);
			this.m_csBlue.Name = "m_csBlue";
			this.m_csBlue.Selected = false;
			this.m_csBlue.Size = new System.Drawing.Size(96, 32);
			this.m_csBlue.TabIndex = 2;
			this.m_csBlue.Unit = ComponentUnit.Value;
			this.m_csBlue.Value = 0;
			this.m_csBlue.ComponentTextKeyUp += new ColorSpaceComponentEventHandler(ComponentTextKeyUp);
			this.m_csBlue.ComponentSelected += new ColorSpaceComponentEventHandler(this.ComponentSelected);
			//			this.m_csBlue.ComponentTextLostFocus += new ColorSpaceComponent.ComponentTextLostFocusHandler(this.ComponentTextLostFocus);
			// 
			// m_csGreen
			// 
			this.m_csGreen.DisplayCharacter = 'G';
			this.m_csGreen.Location = new System.Drawing.Point(8, 32);
			this.m_csGreen.Name = "m_csGreen";
			this.m_csGreen.Selected = false;
			this.m_csGreen.Size = new System.Drawing.Size(96, 32);
			this.m_csGreen.TabIndex = 1;
			this.m_csGreen.Unit = ComponentUnit.Value;
			this.m_csGreen.Value = 0;
			
			this.m_csGreen.ComponentTextKeyUp += new ColorSpaceComponentEventHandler(ComponentTextKeyUp);
			this.m_csGreen.ComponentSelected += new ColorSpaceComponentEventHandler(this.ComponentSelected);
			//			this.m_csGreen.ComponentTextLostFocus += new ColorSpaceComponent.ComponentTextLostFocusHandler(this.ComponentTextLostFocus);
			// 
			// RgbColorSpace
			// 
			this.Controls.Add(this.m_csBlue);
			this.Controls.Add(this.m_csGreen);
			this.Controls.Add(this.m_csRed);
			this.Name = "RgbColorSpace";
			this.Size = new System.Drawing.Size(112, 96);
			this.ResumeLayout(false);

		}

		#endregion

	}

}
