
using Sano.Utility.NativeMethods;
using System;
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	internal class DragForm : Form {

		/// <summary>
		/// Private data fields.
		/// </summary>
		private int m_cursorXDifference;
		private int m_cursorYDifference;
		private Size m_clientRectangleSize;
		
		/// <summary>
		/// Overloaded constructor that allows you to set the size property.
		/// </summary>
		/// <param name="size">A Size object representing the desired size.</param>

		internal DragForm() : base() {

			this.ShowInTaskbar = false;
			this.TopMost = true;
			this.FormBorderStyle = FormBorderStyle.FixedToolWindow;

			this.Size = new Size( 0, 0 );

			// Since the second parameter of the ShowWindow method in ShowForm
			// is ignored the first time the form is shown, work around this by
			// quickly showing and then hiding the form.
			ShowForm();
			Hide();

		}

		/// <summary>
		/// Sets the client size of the form equal to the Size parameter.
		/// </summary>
		/// <param name="newSize">The desired client area size.</param>

		internal void ChangeSize( Size newSize ) {

			m_clientRectangleSize = newSize;
			this.Size = newSize + new Size( 6, 22 );
			this.Region = new Region( new Rectangle( new Point( 3, 19 ), newSize ) );

		}

		/// <summary>
		/// Sets the location of the form relative to the top left corner of
		/// the client area as opposed to the top left corner of the window.
		/// </summary>
		/// <param name="newLocation">The new location of the form.</param>
		
		internal void UpdateLocation( Point newLocation ) {
			this.Location = new Point( newLocation.X - 3, newLocation.Y - 19 ); 
		}

		/// <summary>
		/// Displays the form with a transparency of .5, but does not make 
		/// it active.
		/// </summary>
		
		internal void ShowForm() {

			this.Opacity = .5f;
			Window.ShowWindow( this.Handle, (long)ShowWindowMessages.SW_SHOWNOACTIVATE );
		
		}

		/// <summary>
		/// Represents the difference between the horizontal cursor location  
		/// and the location of this form.
		/// </summary>

		internal int CursorXDifference {
			get { return m_cursorXDifference; }
			set { m_cursorXDifference = value; }
		}

		/// <summary>
		/// Represents the difference between the vertical cursor location and 
		/// the location of this form.
		/// </summary>
		
		internal int CursorYDifference {
			get { return m_cursorYDifference; }
			set { m_cursorYDifference = value; }
		}
		
		/// <summary>
		/// Paints a thin 1f border around the boundaries of the client area.
		/// </summary>
		/// <param name="e">A PaintEventArgs that contains the event data.</param>
		
		protected override void OnPaint( PaintEventArgs e ) {
			e.Graphics.DrawRectangle( Pens.Black, 0, 0, this.ClientRectangle.Width - 1, this.ClientRectangle.Height - 1 );		
		}
		
		/// <summary>
		/// Overrides the Form's WndProc method to return HTTRANSPARENT when the
		/// WM_NCHITTEST message is returned.
		/// </summary>
		/// <param name="m">A message object containing the message data.</param>

		protected override void WndProc(ref Message m) {
	
			if ( m.Msg == WindowsMessages.WM_NCHITTEST ) {
				m.Result = ( IntPtr ) HitTestValues.HTTRANSPARENT;
			} else {
				base.WndProc(ref m);
			}

		}
	
	} // DragForm

} // Sano.PersonalProjects.ColorPicker.Controls
