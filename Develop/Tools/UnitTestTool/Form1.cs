using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace UnitTestTool
{
    public partial class Form1 : Form
    {
        private Loader loader = new Loader();
        private ConfigData configData = new ConfigData();
        private ConfigData configDataBackUp = new ConfigData();
        private AutoCompleteData autoCompleteData = new AutoCompleteData();
        private KeyInputHandler keyInputHandler = new KeyInputHandler();

        public Form1()
        {
            InitializeComponent();

            Win32Until.ClipBoard.SetClipboardViewer((int) this.Handle);
            keyInputHandler.RegisterHotKey(this.Handle);
        }

        public void Reset()
        {
            this.SerializeConfigData(this.configDataBackUp);
        }

        public void FocusToEnableTab()
        {
            this.tabControl1.SelectedIndex = 0;
        }

        public void FocusToDisableTab()
        {
            this.tabControl1.SelectedIndex = 1;
        }

        public void FocusToOptionTab()
        {
            this.tabControl1.SelectedIndex = 2;
        }

        private void Form1_Load(object sender, EventArgs e)
        {            
            this.loader.LoadConfigFromFile(this.configData);
            this.loader.LoadConfigFromFile(this.configDataBackUp);

            // 자동완성은 우선 고려 안함
            //this.loader.LoadAutoCompleteFromFile(this.autoCompleteData);
            //this.loader.SaveAutoCompleteFromFile(this.autoCompleteData);

            this.SerializeConfigData(this.configData);
        }        

        private void button_Reset_Click(object sender, EventArgs e)
        {
            this.SerializeConfigData(this.configDataBackUp);
        }

        private void button_Close_Click(object sender, EventArgs e)
        {
            this.Close();
        }        

        private void Form1_KeyUp(object sender, KeyEventArgs e)
        {
            this.keyInputHandler.Handle_Local(sender, e, this);
        }

        private void checkBox_Execute_CheckStateChanged(object sender, EventArgs e)
        {
            this.configData.Execute = (CheckState.Checked == this.checkBox_Execute.CheckState) ? true : false;
            this.loader.SaveConfigToFile(this.configData);
        }


        private void textBox_TEST1_TextChanged(object sender, EventArgs e)
        {
            this.configData.Test1 = this.textBox_TEST1.Text;
            this.loader.SaveConfigToFile(this.configData);
        }

        private void textBox_TEST2_TextChanged(object sender, EventArgs e)
        {
            this.configData.Test2 = this.textBox_TEST2.Text;
            this.loader.SaveConfigToFile(this.configData);
        }

        private void textBox_TEST3_TextChanged(object sender, EventArgs e)
        {
            this.configData.Test3 = this.textBox_TEST3.Text;
            this.loader.SaveConfigToFile(this.configData);
        }

        private void textBox_TEST4_TextChanged(object sender, EventArgs e)
        {
            this.configData.Test4 = this.textBox_TEST4.Text;
            this.loader.SaveConfigToFile(this.configData);
        }

        private void textBox_TEST_SUITE1_TextChanged(object sender, EventArgs e)
        {
            this.configData.TestSuite1 = this.textBox_TEST_SUITE1.Text;
            this.loader.SaveConfigToFile(this.configData);
        }

        private void textBox_TEST_SUITE2_TextChanged(object sender, EventArgs e)
        {
            this.configData.TestSuite2 = this.textBox_TEST_SUITE2.Text;
            this.loader.SaveConfigToFile(this.configData);
        }

        private void textBox_TEST_SUITE3_TextChanged(object sender, EventArgs e)
        {
            this.configData.TestSuite3 = this.textBox_TEST_SUITE3.Text;
            this.loader.SaveConfigToFile(this.configData);
        }

        private void textBox_TEST_SUITE4_TextChanged(object sender, EventArgs e)
        {
            this.configData.TestSuite4 = this.textBox_TEST_SUITE4.Text;
            this.loader.SaveConfigToFile(this.configData);
        }
        
        private void textBox_DISABLE_TEST_SUITE1_TextChanged(object sender, EventArgs e)
        {
            this.configData.DisabledSuite1 = this.textBox_DISABLE_TEST_SUITE1.Text;
            this.loader.SaveConfigToFile(this.configData);
        }

        private void textBox_DISABLE_TEST_SUITE2_TextChanged(object sender, EventArgs e)
        {
            this.configData.DisabledSuite2 = this.textBox_DISABLE_TEST_SUITE2.Text;
            this.loader.SaveConfigToFile(this.configData);
        }

        private void textBox_DISABLE_TEST_SUITE3_TextChanged(object sender, EventArgs e)
        {
            this.configData.DisabledSuite3 = this.textBox_DISABLE_TEST_SUITE3.Text;
            this.loader.SaveConfigToFile(this.configData);
        }

        private void textBox_DISABLE_TEST_SUITE4_TextChanged(object sender, EventArgs e)
        {
            this.configData.DisabledSuite4 = this.textBox_DISABLE_TEST_SUITE4.Text;
            this.loader.SaveConfigToFile(this.configData);
        }

        private void textBox_Repeat_TextChanged(object sender, EventArgs e)
        {
            this.configData.Repeat = Convert.ToInt32(this.textBox_Repeat.Text.ToString());
            this.loader.SaveConfigToFile(this.configData);
        }

        private void checkBox_BreakOnFailure_CheckedChanged(object sender, EventArgs e)
        {
            this.configData.BreakOnFailure = (CheckState.Checked == this.checkBox_BreakOnFailure.CheckState) ? true : false;
            this.loader.SaveConfigToFile(this.configData);
        }        

        private void SerializeConfigData(ConfigData configData)
        {
            this.checkBox_Execute.CheckState = (true == configData.Execute) ? CheckState.Checked : CheckState.Unchecked;

            this.textBox_TEST1.Text = configData.Test1;
            this.textBox_TEST2.Text = configData.Test2;
            this.textBox_TEST3.Text = configData.Test3;
            this.textBox_TEST4.Text = configData.Test4;

            this.textBox_TEST_SUITE1.Text = configData.TestSuite1;
            this.textBox_TEST_SUITE2.Text = configData.TestSuite2;
            this.textBox_TEST_SUITE3.Text = configData.TestSuite3;
            this.textBox_TEST_SUITE4.Text = configData.TestSuite4;

            this.textBox_DISABLE_TEST_SUITE1.Text = configData.DisabledSuite1;
            this.textBox_DISABLE_TEST_SUITE2.Text = configData.DisabledSuite2;
            this.textBox_DISABLE_TEST_SUITE3.Text = configData.DisabledSuite3;
            this.textBox_DISABLE_TEST_SUITE4.Text = configData.DisabledSuite4;

            this.textBox_Repeat.Text = Convert.ToString(configData.Repeat);
            this.checkBox_BreakOnFailure.CheckState = (true == configData.BreakOnFailure) ? CheckState.Checked : CheckState.Unchecked;
        }

                
        	
        protected override void WndProc(ref Message m)
        {            
            if (m.Msg == 0x0312)
            {
                //if (true == keyInputHandler.Handle_Global(m, this.configData))
                //{
                    SerializeConfigData(this.configData);
                //    this.Activate();
                //}
            }

            base.WndProc(ref m);
        }        
    }
}