using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SoulHunt
{
    public partial class SHCE_PgEditor : Form
    {
        private Object _TagData;

        public SHCE_PgEditor()
        {
            InitializeComponent();
        }

        private void VelixianPgEditor_Load(object sender, EventArgs e)
        {
            pgData.SelectedObject = _TagData;
        }

        public void SetObject(Object data)
        {
            if (data != null) _TagData = data;
        }

        public Object GetObject()
        {
            return _TagData;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            Close();
        }
    }
}
