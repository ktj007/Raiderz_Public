
using System;
using System.Windows.Forms;

namespace Sano.PersonalProjects.ColorPicker {
	
	internal class ComponentTextBox : TextBox {

		internal ComponentTextBox() : base() {
			this.MaxLength = 3;
		}

		protected override bool IsInputKey(Keys keyData) {
		
			bool isInputKey = base.IsInputKey( keyData );
			int keyVal = ( int ) keyData;

			if ( keyVal > 47 && keyVal < 58 ) {
				isInputKey = true;
			} 

			return isInputKey;

		}
	
		protected override bool ProcessDialogKey(Keys keyData) {
		
			bool wasKeyProcessed = true;
			int keyVal = ( int ) keyData;

			if ( ( keyVal == ( int ) Keys.Tab ) || ( keyVal == ( int ) Keys.Back ) || ( keyVal == ( int ) ( Keys.Shift | Keys.Tab ) ) ) {
				wasKeyProcessed = base.ProcessDialogKey( keyData );
			}

			return wasKeyProcessed;

		}

		protected override void OnLostFocus(EventArgs e) {
			if ( this.Text.Trim().Equals( String.Empty ) ) {
				this.Text = "0";
			}
		}

	}

}