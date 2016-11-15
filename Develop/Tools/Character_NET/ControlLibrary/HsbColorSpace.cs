using System;
using System.Collections;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	/// <summary>
	/// HSB color space.
	/// </summary>

	internal class HsbColorSpace : ColorSpace {

		// data fields
		private ColorSpaceComponent m_csBrightness;
		private ColorSpaceComponent m_csSaturation;
		private ColorSpaceComponent m_csHue;

		/// <summary>
		/// Gets or sets a value containing the coordinates of this color 
		/// space.
		/// </summary>

		internal sealed override IColorSpaceStructure Structure {

			get {
				return new HSB( m_csHue.Value, m_csSaturation.Value, m_csBrightness.Value );
			}

			set {
				
				HSB hsb = ( HSB ) value;

				m_csHue.Value = hsb.Hue;
				m_csSaturation.Value = hsb.Saturation;
				m_csBrightness.Value = hsb.Brightness;

			}
		
		}

		/// <summary>
		/// Constructor. Adds the color space components to the color space
		/// component collection.
		/// </summary>

		public HsbColorSpace() {

			InitializeComponent();

			ColorSpaceComponents.Add( m_csHue );
			ColorSpaceComponents.Add( m_csSaturation );
			ColorSpaceComponents.Add( m_csBrightness );
		
		}

		/// <summary>
		/// Sets the default color space component.
		/// </summary>

		internal override void SetDefaultSelection() {
			ChangeCurrentlySelectedComponent( m_csHue );
		}

		/// <summary>
		/// Updates the color space coordinate values.
		/// </summary>
		/// <param name="csStructure">A IColorSpaceStructure object containing 
		/// the values that are to be mapped to the coordinates of this color 
		/// space.</param>

		protected override void UpdateValues( IColorSpaceStructure m_csStructure ) {
			
			HSB hsb = ( HSB ) m_csStructure;
			
			m_csHue.Value = hsb.Hue;
			m_csSaturation.Value = hsb.Saturation;
			m_csBrightness.Value = hsb.Brightness;
		
		}

		/// <summary>
		/// Converts the coordinates represented by this color space to its 
		/// equivalent HEX representation.
		/// </summary>
		/// <returns>A string containing a hexadecimal value.</returns>

		internal override Color GetColor() {
			
			RGB rgb = ColorConverter.HsbToRgb( new HSB( m_csHue.Value, m_csSaturation.Value, m_csBrightness.Value ) );
			return Color.FromArgb( rgb.Red, rgb.Green, rgb.Blue );

		}
		
		#region Component Designer generated code

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			this.m_csBrightness = new ColorSpaceComponent();
			this.m_csSaturation = new ColorSpaceComponent();
			this.m_csHue = new ColorSpaceComponent();
			this.SuspendLayout();
			// 
			// m_csBrightness
			// 
			this.m_csBrightness.DisplayCharacter = 'B';
			this.m_csBrightness.Location = new System.Drawing.Point(8, 56);
			this.m_csBrightness.Name = "m_csBrightness";
			this.m_csBrightness.Size = new System.Drawing.Size(96, 32);
			this.m_csBrightness.TabIndex = 2;
			this.m_csBrightness.Unit = ComponentUnit.Percentage;
			this.m_csBrightness.Value = 0;
			this.m_csBrightness.MaximumValue = 100;
			this.m_csBrightness.ComponentTextKeyUp += new ColorSpaceComponentEventHandler(this.ComponentTextKeyUp);
			this.m_csBrightness.ComponentSelected += new ColorSpaceComponentEventHandler(ComponentSelected);
			// 
			// m_csSaturation
			// 
			this.m_csSaturation.DisplayCharacter = 'S';
			this.m_csSaturation.Location = new System.Drawing.Point(8, 32);
			this.m_csSaturation.Name = "m_csSaturation";
			this.m_csSaturation.Size = new System.Drawing.Size(96, 32);
			this.m_csSaturation.TabIndex = 1;
			this.m_csSaturation.Unit = ComponentUnit.Percentage;
			this.m_csSaturation.Value = 0;
			this.m_csSaturation.MaximumValue = 100;
			this.m_csSaturation.ComponentTextKeyUp += new ColorSpaceComponentEventHandler(this.ComponentTextKeyUp);
			this.m_csSaturation.ComponentSelected += new ColorSpaceComponentEventHandler(ComponentSelected);
			// 
			// m_csHue
			// 
			this.m_csHue.DisplayCharacter = 'H';
			this.m_csHue.Location = new System.Drawing.Point(8, 8);
			this.m_csHue.Name = "m_csHue";
			this.m_csHue.Size = new System.Drawing.Size(96, 32);
			this.m_csHue.TabIndex = 0;
			this.m_csHue.Unit = ComponentUnit.Degree;
			this.m_csHue.Value = 0;
			this.m_csHue.MaximumValue = 360;
			this.m_csHue.ComponentTextKeyUp += new ColorSpaceComponentEventHandler(this.ComponentTextKeyUp);
			this.m_csHue.ComponentSelected += new ColorSpaceComponentEventHandler(ComponentSelected);
			// 
			// HsbColorSpace
			// 
			this.Controls.Add(this.m_csBrightness);
			this.Controls.Add(this.m_csSaturation);
			this.Controls.Add(this.m_csHue);
			this.Name = "HsbColorSpace";
			this.Size = new System.Drawing.Size(112, 96);
			this.ResumeLayout(false);

		}
		#endregion

	} // HsbColorSpace

} // Sano.PersonalProjects.ColorPicker.Controls
