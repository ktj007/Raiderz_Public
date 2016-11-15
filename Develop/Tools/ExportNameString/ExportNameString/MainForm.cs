using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace ExportNameString
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void button2_Click(object sender, EventArgs e)
        { 
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = folderBrowserDialog1.SelectedPath;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                AddFileList(openFileDialog1.FileNames);        
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            foreach (string file in checkedListBox1.CheckedItems)
            {
                Exporter.Export(file, textBox1.Text);
            }
            
            MessageBox.Show("내보내기 완료", "알림");
        }

        private void AddFileList(string[] files)
        {
            foreach (string name in files)
            {
                // 중복 아이템이 추가되는 것을 피한다.
                if (-1 != checkedListBox1.FindStringExact(name)) continue;

                checkedListBox1.Items.Add(name, true);

                folderBrowserDialog1.SelectedPath = Path.GetDirectoryName(name) + "\\lang\\ko_KR";
                textBox1.Text = folderBrowserDialog1.SelectedPath;
            }
        }
    }
}
