
using System;
using System.Drawing;
using System.Windows.Forms;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	// Used as a modal dialog that collects the color description from the 
	// user.

	internal class AddNewColorSwatchForm : Form {

		// controls
		private TextBox txtColorDescription;
		private Button btnOk;
		private PictureBox picColor;
		private Button btnCancel;
		private Label lblDescription;

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="colorToAdd">The color that the picture box's BackColor
		/// property is to be set to.</param>

		internal AddNewColorSwatchForm( Color colorToAdd ) : base() {
			
			InitializeComponent();
			picColor.BackColor = colorToAdd;

		}

		/// <summary>
		/// Gets the color description.
		/// </summary>

		internal string ColorDescription {
			get { return txtColorDescription.Text; }
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>

		private void InitializeComponent() {
			this.picColor = new System.Windows.Forms.PictureBox();
			this.lblDescription = new System.Windows.Forms.Label();
			this.txtColorDescription = new System.Windows.Forms.TextBox();
			this.btnCancel = new System.Windows.Forms.Button();
			this.btnOk = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// picColor
			// 
			this.picColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.picColor.Location = new System.Drawing.Point(12, 8);
			this.picColor.Name = "picColor";
			this.picColor.Size = new System.Drawing.Size(50, 50);
			this.picColor.TabIndex = 0;
			this.picColor.TabStop = false;
			// 
			// lblDescription
			// 
			this.lblDescription.Location = new System.Drawing.Point(79, 14);
			this.lblDescription.Name = "lblDescription";
			this.lblDescription.Size = new System.Drawing.Size(184, 23);
			this.lblDescription.TabIndex = 3;
			this.lblDescription.Text = "Enter color description:";
			// 
			// txtColorDescription
			// 
			this.txtColorDescription.Location = new System.Drawing.Point(79, 33);
			this.txtColorDescription.MaxLength = 100;
			this.txtColorDescription.Name = "txtColorDescription";
			this.txtColorDescription.Size = new System.Drawing.Size(208, 20);
			this.txtColorDescription.TabIndex = 0;
			this.txtColorDescription.Text = "";
			this.txtColorDescription.KeyUp += new System.Windows.Forms.KeyEventHandler(this.txtColorDescription_KeyUp);
			// 
			// btnCancel
			// 
			this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btnCancel.Location = new System.Drawing.Point(304, 35);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.TabIndex = 2;
			this.btnCancel.Text = "Cancel";
			// 
			// btnOk
			// 
			this.btnOk.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.btnOk.Location = new System.Drawing.Point(304, 8);
			this.btnOk.Name = "btnOk";
			this.btnOk.TabIndex = 1;
			this.btnOk.Text = "OK";
			this.btnOk.Click += new System.EventHandler(this.btnOk_Click);
			// 
			// AddNewColorSwatchForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(394, 66);
			this.Controls.Add(this.btnOk);
			this.Controls.Add(this.btnCancel);
			this.Controls.Add(this.txtColorDescription);
			this.Controls.Add(this.lblDescription);
			this.Controls.Add(this.picColor);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "AddNewColorSwatchForm";
			this.Text = "Add Color";
			this.ResumeLayout(false);

		}

		#endregion

		private void btnOk_Click(object sender, System.EventArgs e) {
			this.Close();
		}

		private void txtColorDescription_KeyUp(object sender, KeyEventArgs e) {

			if ( e.KeyData == Keys.Enter ) {
			
				this.DialogResult = DialogResult.OK;
				this.Close();

			} else 	if ( e.KeyData == Keys.Escape ) {
				
				this.DialogResult = DialogResult.Cancel;
				this.Close();
			
			}


		}

	} // AddNewColorSwatchForm

} // Sano.PersonalProjects.ColorPicker.Controls