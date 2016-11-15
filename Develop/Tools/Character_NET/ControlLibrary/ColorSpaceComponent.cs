using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

using System.Diagnostics;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	// TODO: move all textbox functionality into the ColorSpaceComponentTextBox
	// class.
	
	internal class ColorSpaceComponent : UserControl {

		// controls		
		private ColorSpaceComponentTextBox txtComponentValue;
		private Label lblComponent;
		private Label lblComponentUnit;
		private RadioButton rdoComponent;
		
		// events
		internal event ColorSpaceComponentEventHandler ComponentSelected;
		internal event ColorSpaceComponentEventHandler ComponentTextKeyUp;

		// member data fields
		char m_displayCharacter;
		ComponentUnit m_unit = ComponentUnit.Value;
		int m_value;
		int m_minimumValue;
		int m_maximumValue = 255;
		string m_name = String.Empty;

		public bool Selected {
			get { return rdoComponent.Checked; }
			set { rdoComponent.Checked = value; }
		}

		public bool RadioButtonVisible {
			get { return rdoComponent.Visible; }
			set { 
				rdoComponent.Visible = value; 
				lblComponent.Visible = !value;
			}
		}

		public bool ReadOnly {
			get { return txtComponentValue.ReadOnly; }
			set { 
				txtComponentValue.ReadOnly = value; 
				txtComponentValue.BackColor = Color.White;
			}
		}

		public ComponentUnit Unit {
			
			get { return m_unit; }			
			set { 
				
				m_unit = value; 
				switch ( m_unit ) {

					case ComponentUnit.Degree:
						this.lblComponentUnit.Text = "?";
						break;

					case ComponentUnit.Percentage:
						this.lblComponentUnit.Text = "%";
						break;

					default:
						this.lblComponentUnit.Text = String.Empty;
						break;
				}			
			
			}

		}

		public int Value {

			get { 
				
				int currentValue;

				if ( txtComponentValue.Text.Length == 0 ) {
					currentValue = 0;
				} else {
					currentValue = Int32.Parse( this.txtComponentValue.Text ); 
				
					if ( currentValue > this.MaximumValue ) {
						currentValue = Int32.Parse( this.txtComponentValue.Text.Substring( 0, ( this.txtComponentValue.Text.Length - 1 ) ) );
					}
				}

				return currentValue;
			
			}

			set { 
				
				m_value = value;
				txtComponentValue.Text = m_value.ToString();
				txtComponentValue.SelectionStart = txtComponentValue.Text.Length;

			}

		}

		[DefaultValue(0)]
		public int MinimumValue {
			get { return m_minimumValue; }
			set { m_minimumValue = value; } 
		}

		[DefaultValue(255)]
		public int MaximumValue {
			get { return m_maximumValue; }
			set { m_maximumValue = value; } 
		}

		public char DisplayCharacter {
			get { return m_displayCharacter; }

			set {

				m_displayCharacter = value;
				this.rdoComponent.Text = String.Format( "{0}:", m_displayCharacter.ToString().ToUpper() );
				this.lblComponent.Text = String.Format( "{0}:", m_displayCharacter.ToString().ToUpper() );

			}
		}
		
		#region Constructor

		internal ColorSpaceComponent() : base() {
			InitializeComponent();
		}

		#endregion Constructor

		#region Component Designer generated code

		private void InitializeComponent() {
			this.lblComponentUnit = new System.Windows.Forms.Label();
			this.txtComponentValue = new Sano.PersonalProjects.ColorPicker.Controls.ColorSpaceComponentTextBox();
			this.rdoComponent = new System.Windows.Forms.RadioButton();
			this.lblComponent = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// lblComponentUnit
			// 
			this.lblComponentUnit.Location = new System.Drawing.Point(80, 6);
			this.lblComponentUnit.Name = "lblComponentUnit";
			this.lblComponentUnit.Size = new System.Drawing.Size(16, 16);
			this.lblComponentUnit.TabIndex = 49;
			this.lblComponentUnit.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// txtComponentValue
			// 
			this.txtComponentValue.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.txtComponentValue.Location = new System.Drawing.Point(40, 4);
			this.txtComponentValue.MaxLength = 3;
			this.txtComponentValue.Name = "txtComponentValue";
			this.txtComponentValue.Size = new System.Drawing.Size(40, 20);
			this.txtComponentValue.TabIndex = 0;
			this.txtComponentValue.Text = "";
			this.txtComponentValue.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txtComponentValue_KeyDown);
			this.txtComponentValue.LostFocus += new System.EventHandler(this.txtComponentValue_LostFocus);
			this.txtComponentValue.KeyUp += new System.Windows.Forms.KeyEventHandler(this.txtComponentValue_KeyUp);
			this.txtComponentValue.ContextMenu = new ContextMenu( null );
			// 
			// rdoComponent
			// 
			this.rdoComponent.Location = new System.Drawing.Point(4, 2);
			this.rdoComponent.Name = "rdoComponent";
			this.rdoComponent.Size = new System.Drawing.Size(36, 24);
			this.rdoComponent.TabIndex = 50;
			this.rdoComponent.Click += new System.EventHandler(this.rdoComponent_Click);
			// 
			// lblComponent
			// 
			this.lblComponent.Location = new System.Drawing.Point(18, 2);
			this.lblComponent.Name = "lblComponent";
			this.lblComponent.Size = new System.Drawing.Size(20, 24);
			this.lblComponent.TabIndex = 51;
			this.lblComponent.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblComponent.Visible = false;
			// 
			// ColorSpaceComponent
			// 
			this.Controls.Add(this.lblComponent);
			this.Controls.Add(this.lblComponentUnit);
			this.Controls.Add(this.txtComponentValue);
			this.Controls.Add(this.rdoComponent);
			this.Name = "ColorSpaceComponent";
			this.Size = new System.Drawing.Size(112, 32);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// Handles the Click event that the rdoComponent object raises.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs containing the event data.</param>

		private void rdoComponent_Click(object sender, EventArgs e) {

			RadioButton rb = ( RadioButton ) sender;
			
			if ( rb.Checked ) {
				OnComponentSelected( EventArgs.Empty );
			}

		}

		/// <summary>
		/// Handles the LostFocus event that the txtComponentValue object 
		/// raises.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs containing the event data.</param>

		private void txtComponentValue_LostFocus( object sender, EventArgs e ) {

			ColorSpaceComponentTextBox textbox = ( ColorSpaceComponentTextBox ) sender;

			int componentValue;
			int resetValue;
			bool showError = true;

			if ( textbox.Text != null && textbox.Text.Length != 0 ) {
				
				componentValue = Int32.Parse( textbox.Text );
				resetValue = componentValue;

				if ( ( componentValue > this.MaximumValue ) ) {
					resetValue = this.MaximumValue;				
				} else if ( componentValue < this.MinimumValue ) {
					resetValue = this.MinimumValue;
				} else {
					showError = false;
				}

			} else {
				resetValue = this.MinimumValue;
			}

			if ( showError ) {
				MessageBox.Show( this, String.Format( "An integer between {0} and {1} is required. Closest value inserted.", this.MinimumValue, this.MaximumValue), "Color Picker", MessageBoxButtons.OK, MessageBoxIcon.Error );
			}

			textbox.Text = resetValue.ToString();

		}

		/// <summary>
		/// Raises the ComponentSelected event.
		/// </summary>
		/// <param name="e">An EventArgs containing the event data.</param>

		protected virtual void OnComponentSelected( EventArgs e ) {

			if ( ComponentSelected != null ) {
				ComponentSelected( this, EventArgs.Empty );
			}

		}

		/// <summary>
		/// Raises the ComponentTextKeyUp event.
		/// </summary>
		/// <param name="e">An EventArgs containing the event data.</param>

		protected virtual void OnComponentTextKeyUp( EventArgs e ) {

			if ( ComponentTextKeyUp != null ) {
				ComponentTextKeyUp( this, EventArgs.Empty );
			}

		}

		private void txtComponentValue_KeyUp(object sender, KeyEventArgs e) {

			if ( e.KeyData !=  Keys.Tab ) {
				OnComponentTextKeyUp( EventArgs.Empty );
			}

		}

		private void txtComponentValue_KeyDown(object sender, KeyEventArgs e) {

			if ( txtComponentValue.Text.Length > 0 && 
				( ( e.KeyData | Keys.Shift ) == ( Keys.Up | Keys.Shift ) || 
				( e.KeyData | Keys.Shift ) == ( Keys.Down | Keys.Shift ) ) ) {

				int componentValue = Int16.Parse( txtComponentValue.Text );
				int incrementValue = 0;

				if ( ( e.KeyData & Keys.Shift ) == Keys.Shift ) {
					incrementValue = 0x00000A;
				} else {
					incrementValue = 0x000001;
				}

				if ( ( e.KeyData & Keys.Up ) == Keys.Up ) {
				
					if ( componentValue + incrementValue <= this.MaximumValue ) {
						componentValue += incrementValue;
					} else {
						componentValue = this.MaximumValue;
					}

				} else if ( ( e.KeyData | Keys.Shift ) == ( Keys.Down | Keys.Shift ) ) {

					if ( e.KeyData == ( Keys.Down | Keys.Shift ) ) {						
						incrementValue = -10;
					} else {
						incrementValue = -1;
					}

					if ( componentValue + incrementValue > this.MinimumValue ) {
						componentValue += incrementValue;
					} else {
						componentValue = this.MinimumValue;
					}
			
				} 			

				txtComponentValue.Text = componentValue.ToString();
				ComponentTextKeyUp( this, EventArgs.Empty );

				// this is a hack to fix some of the problems with the key 
				// combination selecting part of the text (only happens
				// with shift+down).
				//
				// TODO: see if there is a better way to do this.

				if ( e.KeyData == ( Keys.Shift | Keys.Down ) ) {
					txtComponentValue.SelectionStart = txtComponentValue.Text.Length;
				}

			}

		}

	} // ColorSpaceComponent

} // Sano.PersonalProjects.ColorPicker.Controls
