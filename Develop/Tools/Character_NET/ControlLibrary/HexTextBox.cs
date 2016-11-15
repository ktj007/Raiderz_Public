
using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows.Forms;

namespace Sano.PersonalProjects.ColorPicker.Controls {
	
	// TODO: Implement functionality that will paint preceding zeroes in a
	// lighter color, perhaps 50% grey. This control is a pain in the butt to 
	// customize since it's actually a wrapper around the Win32 TextBox control. 
	// Look into the RichTextBox. This might do it.

	/// <summary>
	/// A specially designed textbox class that ensures that only HEX values 
	/// are entered and displayed.
	/// </summary>

	internal class HexTextBox : ColorSpaceComponentTextBox {

		// constants
		private const int MINIMUM_VALUE = 0x0;
		private const int MAXIMUM_VALUE = 0xFFFFFF;

		/// <summary>
		/// Overrides the Text property to set the DefaultValue attribute.
		/// </summary>
	
		[DefaultValue( "000000" )]
		public override string Text {
			get { return base.Text; }
			set { base.Text = value; }
		}

		/// <summary>
		/// Constructor. Sets the max length to 6 and the text property to an 
		/// empty string.
		/// </summary>

		internal HexTextBox() : base() {
		
			this.MaxLength = 6;
			this.Text = String.Empty;

		}
			
		/// <summary>
		/// Determines whether or not the specified character is to be 
		/// designated for preprocessing or sent directly to the control.
		/// </summary>
		/// <param name="charCode">The character to be evaluated.</param>
		/// <returns>True if the key should be sent directly to the control, 
		/// false if it should be preprocessed.</returns>
		
		protected override bool IsInputChar(char charCode) {
			
			bool isInputChar = base.IsInputChar( charCode );

			if ( ( charCode >= ( int ) Keys.A && charCode <= ( int ) Keys.F ) ) {
				isInputChar = true;			
			}

			return isInputChar;

		}
	
		/// <summary>
		/// Preprocessing leading up to the raising of the KeyDown event.
		/// </summary>
		/// <param name="e">A KeyEventArgs containing the event data.</param>

		protected override void OnKeyDown(KeyEventArgs e) {

			base.OnKeyDown( e );

			if ( this.Text.Length > 0 && 
				 ( ( e.KeyData | Keys.Shift ) == ( Keys.Up | Keys.Shift ) || 
				 ( e.KeyData | Keys.Shift ) == ( Keys.Down | Keys.Shift ) ) ) {

				string textHexValue = String.Format( "0x{0}", this.Text );
				int hexValue = Convert.ToInt32( textHexValue, 16 );
				int incrementValue = 0;

				if ( ( e.KeyData & Keys.Shift ) == Keys.Shift ) {
					incrementValue = 0x00000A;
				} else {
					incrementValue = 0x000001;
				}

				if ( ( e.KeyData & Keys.Up ) == Keys.Up ) {
				
					if ( hexValue + incrementValue <= MAXIMUM_VALUE ) {
						hexValue += incrementValue;
					} else {
						hexValue = MAXIMUM_VALUE;
					}

				} else {

					if ( hexValue - incrementValue >= MINIMUM_VALUE ) {
						hexValue -= incrementValue;
					} else {
						hexValue = MINIMUM_VALUE;
					}

				}

				this.Text = hexValue.ToString( "x6" ).ToUpper();

				// this is a hack to fix some of the problems with the key 
				// combination selecting part of the text (only happens
				// with shift+down).
				//
				// TODO: see if there is a better way to do this.

				if ( e.KeyData == ( Keys.Shift | Keys.Down ) ) {
					this.SelectionStart = this.Text.Length;
				}

			}
		
		}

	} // HexTextBox

} // Sano.PersonalProjects.ColorPicker.Controls
