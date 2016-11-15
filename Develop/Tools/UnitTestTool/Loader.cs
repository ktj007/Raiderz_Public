using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace UnitTestTool
{
    class Loader
    {
        public void LoadConfigFromFile(ConfigData data)
        {
            string strFileName = StringUtil.File.UnitTest;
            if (false == System.IO.File.Exists(strFileName))
            {
                strFileName = StringUtil.File.UnitTestRel;
            }
            if (false == System.IO.File.Exists(strFileName)) return;

            string strPathFileName = Environment.CurrentDirectory + '\\' + strFileName;            
            try
            {
                int nValue;
                StringBuilder strValue = new StringBuilder(512);

                nValue = Win32Until.INI.GetPrivateProfileInt(StringUtil.INI.UnitTest, StringUtil.INI.Execute, 0, strPathFileName);
                data.Execute = (0 == nValue) ? false : true;

                data.Repeat = Win32Until.INI.GetPrivateProfileInt(StringUtil.INI.UnitTest, StringUtil.INI.Repeat, 0, strPathFileName);

                nValue = Win32Until.INI.GetPrivateProfileInt(StringUtil.INI.UnitTest, StringUtil.INI.BreakOnFailure, 0, strPathFileName);
                data.BreakOnFailure = (0 == nValue) ? false : true;

                nValue = Win32Until.INI.GetPrivateProfileInt(StringUtil.INI.UnitTest, StringUtil.INI.LogSuiteName, 0, strPathFileName);
                data.LogSuiteName = (0 == nValue) ? false : true;


                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.DBTest, StringUtil.INI.Server, "", strValue, 512, strPathFileName);
                data.Server = strValue.ToString();
                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.DBTest, StringUtil.INI.DataBase, "", strValue, 512, strPathFileName);
                data.DataBase = strValue.ToString();
                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.DBTest, StringUtil.INI.UserName, "", strValue, 512, strPathFileName);
                data.UserName = strValue.ToString();
                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.DBTest, StringUtil.INI.PassWord, "", strValue, 512, strPathFileName);
                data.Password = strValue.ToString();


                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.Stage, "", strValue, 512, strPathFileName);
                data.Stage = strValue.ToString();

                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.Test1, "", strValue, 512, strPathFileName);
                data.Test1 = strValue.ToString();
                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.Test2, "", strValue, 512, strPathFileName);
                data.Test2 = strValue.ToString();
                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.Test3, "", strValue, 512, strPathFileName);
                data.Test3 = strValue.ToString();
                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.Test4, "", strValue, 512, strPathFileName);
                data.Test4 = strValue.ToString();

                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.TestSuite1, "", strValue, 512, strPathFileName);
                data.TestSuite1 = strValue.ToString();
                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.TestSuite2, "", strValue, 512, strPathFileName);
                data.TestSuite2 = strValue.ToString();
                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.TestSuite3, "", strValue, 512, strPathFileName);
                data.TestSuite3 = strValue.ToString();
                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.TestSuite4, "", strValue, 512, strPathFileName);
                data.TestSuite4 = strValue.ToString();

                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.DisabledSuite1, "", strValue, 512, strPathFileName);
                data.DisabledSuite1 = strValue.ToString();
                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.DisabledSuite2, "", strValue, 512, strPathFileName);
                data.DisabledSuite2 = strValue.ToString();
                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.DisabledSuite3, "", strValue, 512, strPathFileName);
                data.DisabledSuite3 = strValue.ToString();
                Win32Until.INI.GetPrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.DisabledSuite4, "", strValue, 512, strPathFileName);
                data.DisabledSuite4 = strValue.ToString();
            }

            catch (Exception e)
            {

            }
        }

        public void SaveConfigToFile(ConfigData data)
        {
            try
            {
                string strFileName = StringUtil.File.UnitTest;
                string strPathFileName = Environment.CurrentDirectory + '\\' + strFileName;

                int nValue = 0;

                nValue = (true == data.Execute) ? 1 : 0;
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.UnitTest, StringUtil.INI.Execute, nValue.ToString(), strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.UnitTest, StringUtil.INI.Repeat, data.Repeat.ToString(), strPathFileName);
                nValue = (true == data.BreakOnFailure) ? 1 : 0;
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.UnitTest, StringUtil.INI.BreakOnFailure, nValue.ToString(), strPathFileName);
                nValue = (true == data.LogSuiteName) ? 1 : 0;
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.UnitTest, StringUtil.INI.LogSuiteName, nValue.ToString(), strPathFileName);

                bool bDBSave = (0 == data.Server.Length && 0 == data.DataBase.Length && 0 == data.UserName.Length && 0 == data.Password.Length) ? false : true;
                if (true == bDBSave)
                {
                    Win32Until.INI.WritePrivateProfileString(StringUtil.INI.DBTest, StringUtil.INI.Server, data.Server, strPathFileName);
                    Win32Until.INI.WritePrivateProfileString(StringUtil.INI.DBTest, StringUtil.INI.DataBase, data.DataBase, strPathFileName);
                    Win32Until.INI.WritePrivateProfileString(StringUtil.INI.DBTest, StringUtil.INI.UserName, data.UserName, strPathFileName);
                    Win32Until.INI.WritePrivateProfileString(StringUtil.INI.DBTest, StringUtil.INI.PassWord, data.Password, strPathFileName);
                }                

                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.Stage, data.Stage, strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.Test1, data.Test1, strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.Test2, data.Test2, strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.Test3, data.Test3, strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.Test4, data.Test4, strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.TestSuite1, data.TestSuite1, strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.TestSuite2, data.TestSuite2, strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.TestSuite3, data.TestSuite3, strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.TestSuite4, data.TestSuite4, strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.DisabledSuite1, data.DisabledSuite1, strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.DisabledSuite2, data.DisabledSuite2, strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.DisabledSuite3, data.DisabledSuite3, strPathFileName);
                Win32Until.INI.WritePrivateProfileString(StringUtil.INI.Filter, StringUtil.INI.DisabledSuite4, data.DisabledSuite4, strPathFileName);
            }
            catch (System.Exception e)
            {

            }
        }

        public void LoadAutoCompleteFromFile(AutoCompleteData data)
        {
            LoadAutoCompleteFromResultFile(StringUtil.File.Stage1Result, STAGE_TYPE.ST_1, data);
            LoadAutoCompleteFromResultFile(StringUtil.File.Stage2Result, STAGE_TYPE.ST_2, data);
            LoadAutoCompleteFromResultFile(StringUtil.File.Stage3Result, STAGE_TYPE.ST_3, data);
            LoadAutoCompleteFromACFile(data);
        }

        private void LoadAutoCompleteFromResultFile(string fileName, STAGE_TYPE type, AutoCompleteData data)
        {
            try
            {
                XmlTextReader xtr = new XmlTextReader(fileName);
                while (xtr.Read())
                {
                    if (xtr.NodeType == XmlNodeType.Element)
                    {
                        if (xtr.LocalName.ToLower() == StringUtil.XML.Test)
                        {
                            for (int i = 0; i < xtr.AttributeCount; i++)
                            {
                                xtr.MoveToAttribute(i);
                                if (xtr.Name.ToLower() == StringUtil.XML.Suite)
                                {
                                    data.InsertWord(xtr.Value, type, true);
                                }
                                else if (xtr.Name.ToLower() == StringUtil.XML.Name)
                                {
                                    data.InsertWord(xtr.Value, type, false);
                                }                                
                            }
                        }
                        xtr.Read();
                    }
                }
                xtr.Close();
            }
            catch (Exception e)
            {

            }
        }

        private void LoadAutoCompleteFromACFile(AutoCompleteData data)
        {
            try
            {
                XmlTextReader xtr = new XmlTextReader(StringUtil.File.UniteTestToolAC);                
                while (xtr.Read())
                {
                    if (xtr.LocalName.ToLower() == StringUtil.XML.Stage1)
                    {
                        while (xtr.Read())
                        {
                            if (xtr.Name == StringUtil.XML.Suite)
                            {
                                xtr.MoveToFirstAttribute();
                                data.InsertWord(xtr.Value, STAGE_TYPE.ST_1, true);                                    
                            }
                            else if (xtr.Name == StringUtil.XML.Test)
                            {
                                xtr.MoveToFirstAttribute();
                                data.InsertWord(xtr.Value, STAGE_TYPE.ST_1, false);                                    
                            }
                        }
                    }
                    else if (xtr.LocalName.ToLower() == StringUtil.XML.Stage2)
                    {
                        while (xtr.Read())
                        {
                            if (xtr.Name == StringUtil.XML.Suite)
                            {
                                xtr.MoveToFirstAttribute();
                                data.InsertWord(xtr.Value, STAGE_TYPE.ST_2, true);
                            }
                            else if (xtr.Name == StringUtil.XML.Test)
                            {
                                xtr.MoveToFirstAttribute();
                                data.InsertWord(xtr.Value, STAGE_TYPE.ST_2, false);
                            }
                        }
                    }
                    else if (xtr.LocalName.ToLower() == StringUtil.XML.Stage3)
                    {
                        while (xtr.Read())
                        {
                            if (xtr.Name == StringUtil.XML.Suite)
                            {
                                xtr.MoveToFirstAttribute();
                                data.InsertWord(xtr.Value, STAGE_TYPE.ST_3, true);
                            }
                            else if (xtr.Name == StringUtil.XML.Test)
                            {
                                xtr.MoveToFirstAttribute();
                                data.InsertWord(xtr.Value, STAGE_TYPE.ST_3, false);
                            }
                        }
                    }
                    else if (xtr.LocalName.ToLower() == StringUtil.XML.DB)
                    {
                        while (xtr.Read())
                        {
                            if (xtr.Name == StringUtil.XML.Suite)
                            {
                                xtr.MoveToFirstAttribute();
                                data.InsertWord(xtr.Value, STAGE_TYPE.ST_DB, true);
                            }
                            else if (xtr.Name == StringUtil.XML.Test)
                            {
                                xtr.MoveToFirstAttribute();
                                data.InsertWord(xtr.Value, STAGE_TYPE.ST_DB, false);
                            }
                        }
                    }
                }
                xtr.Close();
            }
            catch (Exception e)
            {

            }
        }

        public void SaveAutoCompleteFromFile(AutoCompleteData data)
        {
            Queue<string> qStage1SuiteWord = new Queue<string>();
            Queue<string> qStage1TestWord = new Queue<string>();
            Queue<string> qStage2SuiteWord = new Queue<string>();
            Queue<string> qStage2TestWord = new Queue<string>();
            Queue<string> qStage3SuiteWord = new Queue<string>();
            Queue<string> qStage3TestWord = new Queue<string>();
            Queue<string> qDBSuiteWord = new Queue<string>();
            Queue<string> qDBTestWord = new Queue<string>();

            data.GetWordByStage(qStage1SuiteWord, STAGE_TYPE.ST_1, true);
            data.GetWordByStage(qStage1TestWord, STAGE_TYPE.ST_1, false);
            data.GetWordByStage(qStage2SuiteWord, STAGE_TYPE.ST_2, true);
            data.GetWordByStage(qStage2TestWord, STAGE_TYPE.ST_2, false);
            data.GetWordByStage(qStage3SuiteWord, STAGE_TYPE.ST_3, true);
            data.GetWordByStage(qStage3TestWord, STAGE_TYPE.ST_3, false);
            data.GetWordByStage(qDBSuiteWord, STAGE_TYPE.ST_DB, true);
            data.GetWordByStage(qDBTestWord, STAGE_TYPE.ST_DB, false);
            
            try
            {
                Encoding encoding = Encoding.ASCII;

                XmlTextWriter tw = new XmlTextWriter(StringUtil.File.UniteTestToolAC, encoding);
                tw.Formatting = Formatting.Indented;
                tw.WriteStartDocument();
                tw.WriteStartElement(StringUtil.XML.AutoComplete);                
                
                // Stage1 ---
                tw.WriteStartElement(StringUtil.XML.Stage1);
                foreach (string word in qStage1SuiteWord)
                {
                    tw.WriteStartElement(StringUtil.XML.Suite);
                    tw.WriteAttributeString(StringUtil.XML.Name, word);
                    tw.WriteEndElement();
                }
                foreach (string word in qStage1TestWord)
                {
                    tw.WriteStartElement(StringUtil.XML.Test);
                    tw.WriteAttributeString(StringUtil.XML.Name, word);
                    tw.WriteEndElement();
                }
                tw.WriteEndElement();

                // Stage2 ---
                tw.WriteStartElement(StringUtil.XML.Stage2);
                foreach (string word in qStage2SuiteWord)
                {
                    tw.WriteStartElement(StringUtil.XML.Suite);
                    tw.WriteAttributeString(StringUtil.XML.Name, word);
                    tw.WriteEndElement();
                }
                foreach (string word in qStage2TestWord)
                {
                    tw.WriteStartElement(StringUtil.XML.Test);
                    tw.WriteAttributeString(StringUtil.XML.Name, word);
                    tw.WriteEndElement();
                }
                tw.WriteEndElement();

                // Stage3 ---
                tw.WriteStartElement(StringUtil.XML.Stage3);
                foreach (string word in qStage3SuiteWord)
                {
                    tw.WriteStartElement(StringUtil.XML.Suite);
                    tw.WriteAttributeString(StringUtil.XML.Name, word);
                    tw.WriteEndElement();
                }
                foreach (string word in qStage3TestWord)
                {
                    tw.WriteStartElement(StringUtil.XML.Test);
                    tw.WriteAttributeString(StringUtil.XML.Name, word);
                    tw.WriteEndElement();
                }
                tw.WriteEndElement();

                // DB ---
                tw.WriteStartElement(StringUtil.XML.DB);
                foreach (string word in qDBSuiteWord)
                {
                    tw.WriteStartElement(StringUtil.XML.Suite);
                    tw.WriteAttributeString(StringUtil.XML.Name, word);
                    tw.WriteEndElement();
                }
                foreach (string word in qDBTestWord)
                {
                    tw.WriteStartElement(StringUtil.XML.Test);
                    tw.WriteAttributeString(StringUtil.XML.Name, word);
                    tw.WriteEndElement();
                }
                tw.WriteEndElement();

                tw.WriteEndElement();
                tw.Flush();
                tw.Close();
            }
            catch (Exception e)
            {
                
            }
        }        
    }
}
