
using System;
using System.ComponentModel;
using System.Windows.Forms;
using System.Diagnostics;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	/// <summary>
	/// A specially designed textbox class that ensures that only integers can
	/// be entered.
	/// </summary>

	internal class ColorSpaceComponentTextBox : TextBox {

		// member fields
		bool m_caretHidden;

		/// <summary>
		/// Overrides the Text property to set the DefaultValue attribute.
		/// </summary>
	
		[DefaultValue( "0" )]
		public override string Text {
			get { return base.Text; }
			set { 
				base.Text = value; 
			}
		}

		/// <summary>
		/// Constructor. Sets the maximum length to 3.
		/// </summary>
		
		internal ColorSpaceComponentTextBox() : base() {
			this.MaxLength = 3;
		}

		/// <summary>
		/// Determines whether or not the specified key is to be designated for
		/// preprocessing or sent directly to the control. 
		/// </summary>
		/// <param name="keyData">A Keys object representing the key that was 
		/// just pressed.</param>
		/// <returns>True if the key should be sent directly to the control, 
		/// false if it should be preprocessed.</returns>

		protected override bool IsInputKey( Keys keyData ) {

			bool isInputKey = IsInputChar( ( char )( int ) keyData );
			
			// check the combination keys.
			if ( keyData == ( Keys.Alt | Keys.F4 ) || 
				 keyData == ( Keys.Control | Keys.A ) ||
				 keyData == ( Keys.Control | Keys.C ) || 
				 keyData == ( Keys.Control | Keys.V ) || 
				 keyData == ( Keys.Control | Keys.X ) ) {

				isInputKey = true;
			
			}

			return isInputKey;

		}

		/// <summary>
		/// Determines whether or not the specified character is to be 
		/// designated for preprocessing or sent directly to the control.
		/// </summary>
		/// <param name="charCode">The character to be evaluated.</param>
		/// <returns>True if the key should be sent directly to the control, 
		/// false if it should be preprocessed.</returns>
		
		protected override bool IsInputChar(char charCode) {
			
			bool isInputChar = false;
			Keys controlModifierKey = Control.ModifierKeys;

			// the only character codes that should be processed are the digits 
			// 0-9 and the backspace character (8)
			if ( ( controlModifierKey != Keys.Shift &&
                 ((charCode >= (int)Keys.D0 && charCode <= (int)Keys.D9)) ||
                 ((charCode >= (int)Keys.NumPad0 && charCode <= (int)Keys.NumPad9)) || 
				 ( charCode >= ( int ) Keys.Left && charCode <= ( int ) Keys.Down ) || 
				 charCode == ( int ) Keys.Back ) ) {
				
				isInputChar = true;
			
			}

			return isInputChar;

		}

		/// <summary>
		/// Preprocessing leading up to the raising of the KeyUp event.
		/// 
		/// See the summary for OnKeyDown for a description of the ShowCaret
		/// functionality.
		/// </summary>
		/// <param name="e">A KeyEventArgs containing the event data.</param>

		protected override void OnKeyUp(KeyEventArgs e) {

			// i thought it was weird that this functionality doesn't already
			// exist. since it doesn't, we'll just add it.
			if ( e.KeyData == ( Keys.Control | Keys.A ) ) {
				this.SelectAll();
			}

			if ( ( IsKeyOrShiftPressed( e.KeyData, Keys.Up ) ||
				 IsKeyOrShiftPressed( e.KeyData, Keys.Down ) ) &&
				 m_caretHidden ) {

				Sano.Utility.NativeMethods.Caret.ShowCaret( this.Handle );
				this.SelectionStart = this.Text.Length;
				m_caretHidden = false;
			
			}
			
			base.OnKeyUp(e);
		
		}

		private bool IsKeyOrShiftPressed( Keys keyData, Keys comparsionKey ) {
			return ( ( keyData | Keys.Shift ) == ( comparsionKey | Keys.Shift ) );
		}
		
		/// <summary>
		/// Preprocessing leading up to the raising of the KeyDown event. 
		///
		/// The implementation of this method is a result of an interesting,
		/// yet very annoying problem that I ran into when trying to position
		/// the caret (fancy term for cursor) on the right of the text at all
		/// times. Changing the textbox's SelectionStart property works fine
		/// in cases where the user is not manipulating the text value using 
		/// the up/down arrow keys. Otherwise, there's a very annoying effect
		/// of the cursor moving to the left side (which is its default 
		/// location) and then to the right side of the text. After several
		/// hours of attempting to solve this problem, I decided I was going
		/// to give up, but persistence got the best of me, and what I told 
		/// myself would be a final search on MSDN revealed a win32 caret API. 
		/// To my relief, it was exactly what I needed.
		///	
		///	The caret is hidden in this method, and shown in the OnKeyUp 
		///	method.	
		/// </summary>
		/// <param name="e">A KeyEventArgs containing the event data.</param>

		protected override void OnKeyDown(KeyEventArgs e) {

			// since caret visibility is cumulative, meaning you have to call 
			// ShowCaret the same number of times as HideCaret, it's important
			// to make sure this is called only once during the entire key
			// sequence, and only if the up/down arrow keys are pressed.

			if ( ( IsKeyOrShiftPressed( e.KeyData, Keys.Up ) || 
				IsKeyOrShiftPressed( e.KeyData, Keys.Down ) ) &&
				!m_caretHidden ) {
				
				Sano.Utility.NativeMethods.Caret.HideCaret( this.Handle );
				m_caretHidden = true;

			}

			base.OnKeyDown (e);
		
		}

		/// <summary>
		/// Processes a key message and generates the appropriate key-related
		/// events. 
		/// </summary>
		/// <param name="m">A message representing the windows message to 
		/// process.</param>
		/// <returns>True if the message was processed by the control, 
		/// otherwise, false.</returns>

		protected override bool ProcessKeyEventArgs(ref Message m) {

			bool processKeyEvent = false;

			// we only want the key-related events to be generated if the 
			// message contains a character that has been approved for 
			// conveyance to the control.
			int charKey = ( int ) m.WParam;
			int controlModifierKey = ( int ) ( Keys ) Control.ModifierKeys;

			if ( IsInputKey( ( Keys ) ( charKey | controlModifierKey ) ) ) {
				processKeyEvent = base.ProcessKeyEventArgs (ref m);
			}

			return processKeyEvent;
		
		}

		/// <summary>
		/// Processes a dialog key. This tells Windows whether or not the control
		/// has processed the dialog characters (Tab, Shift, Enter, Backspace, 
		/// etc). This method is only called if IsInputKey returns false.
		/// </summary>
		/// <param name="keyData">The key that is to be processed.</param>
		/// <returns>A boolean value indicating whether or not the key was 
		/// processed.</returns>

		protected override bool ProcessDialogKey(Keys keyData) {
		
			bool wasKeyProcessed = true;
			Keys controlModifierKey = Control.ModifierKeys;
		
			// makes sure we can still process mnemonics
			if ( controlModifierKey == Keys.Alt ) {
				wasKeyProcessed = false;
			} else if ( keyData == Keys.Alt || keyData == Keys.Tab || keyData == Keys.Back || keyData == ( Keys.Shift | Keys.Tab ) ) {
				wasKeyProcessed = base.ProcessDialogKey( keyData );
			}

			return wasKeyProcessed;

		}

		/// <summary>
		/// Preprocessing work that leads up to the LostFocus event.
		/// </summary>
		/// <param name="e">An EventArgs containing the event data.</param>

		protected override void OnLostFocus(EventArgs e) {
			
			if ( this.Text != null && this.Text.Length == 0 ) {
				
				// retrieve the default value of the text property through the
				// DefaultValueAttribute (see the override of the Text property
				// in this class).

				// TODO: move to utility class
				AttributeCollection attributes = TypeDescriptor.GetProperties( this )[ "Text" ].Attributes;
				DefaultValueAttribute textDefaultValue = ( DefaultValueAttribute ) attributes[ typeof( DefaultValueAttribute ) ];
				this.Text = textDefaultValue.Value.ToString();

			}

			// putting this here ensures that the LostFocus event is not raised 
			// until the text property has been set to its default value.
			base.OnLostFocus( e );

		}

	} // ColorSpaceComponentTextBox

} // Sano.PersonalProjects.ColorPicker.Controls
