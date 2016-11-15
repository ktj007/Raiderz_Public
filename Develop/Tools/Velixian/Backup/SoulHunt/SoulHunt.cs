/*
 * SoulHunt.cs
 * SoulHunt와 관련된 각종 자료형들을 구현
 */

using System;
using System.Windows.Forms;
using System.Configuration;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using System.ComponentModel;

namespace SoulHunt
{
    /// <summary>
    /// SHGlobal은 SoulHunt 네임스페이스에서 범용적으로 사용될만한 코드들을 모아둔 곳이다.
    /// 이 클래스의 핵심중 하나는 Serializer와 Deserializer로써 XML을 클래스로 직렬화시키는데 도움을 준다.
    /// </summary>
    #region Global

    public static class SHGlobal
    {
        private static SHLocale _Locale = new SHLocale();

        public static Encoding EUCKR { get { return Encoding.GetEncoding(51949); } }
        public static Encoding UTF8 { get { return Encoding.UTF8; } }
        public static SHLocale Locale { get { return _Locale; } }

        public const String COK = "OK";
        public const String CERROR = "Error.";

        #region Private Members
        
        private static string strProjectFolder = null;
        private static Dictionary<string, string> _Setup = new Dictionary<string,string>();
        private static List<SHSmartCheckFile> _checkFiles = new List<SHSmartCheckFile>();

        #endregion

        #region Init
        public static List<SHSmartCheckFile> GetChecklist()
        {
            return _checkFiles;
        }

        public static void AddCheckFile(SHSmartCheckFile checkFile)
        {
            if (checkFile != null) _checkFiles.Add(checkFile);
        }

        public static void Init()
        {
            foreach (string key in ConfigurationManager.AppSettings.Keys) _Setup[key] = ConfigurationManager.AppSettings[key];

            if (!GetSetting("ProjectFolder", ref strProjectFolder))
            {
                if (SetupFolder() == false)
                {
                    MessageBox.Show("Project folder is not set.", Application.ProductName);
                    Environment.Exit(-1);
                }
            }
            if (!IsSHFolder())
            {
                if (SetupFolder() == false)
                {
                    MessageBox.Show("Project folder is not set.", Application.ProductName);
                    Environment.Exit(-1);
                }
            }
        }
        #endregion

        #region Associated Folder Info
        private static bool IsSHFolder()
        {
            return File.Exists(strProjectFolder + "\\SoulHunterZ.exe");
        }

        public static bool SetFolder(string newFolder)
        {
            strProjectFolder = newFolder;
            SaveSetting();

            return true;
        }

        public static string GetFolder()
        {
            return strProjectFolder;
        }

        public static string GetServerSystemFolder()
        {
            return strProjectFolder + "\\Dev\\GameServer\\System";
        }

        public static string GetClientSystemFolder()
        {
            return strProjectFolder + "\\Data\\System";
        }

        public static string GetClientFieldFolder()
        {
            return strProjectFolder + "\\Data\\Field";
        }

        public static string GetServerColtFolder()
        {
            return strProjectFolder + "\\Dev\\GameServer\\Script\\npc\\Colt";
        }

        #endregion

        #region Setup
        public static bool GetSetting(string key, ref Int32 val)
        {
            try
            {
                val = Int32.Parse(_Setup[key]);
                return true;
            }
            catch
            {
                return false;
            }
        }


        public static bool GetSetting(string key, ref String val)
        {
            try
            {
                val = _Setup[key];
                return true;
            }
            catch
            {
                return false;
            }
        }

        public static void SetSetting(string key, Int32 value)
        {
            if (_Setup.ContainsKey(key))
            {
                _Setup[key] = value.ToString();
            }
            else
            {
                _Setup.Add(key, value.ToString());
            }
        }

        public static void SetSetting(string key, String value)
        {
            if (_Setup.ContainsKey(key))
            {
                _Setup[key] = value;
            }
            else
            {
                _Setup.Add(key, value);
            }
        }


        public static void SaveSetting()
        {
            SetSetting("ProjectFolder", strProjectFolder);

            Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
            
            foreach (string key in _Setup.Keys)
            {
                if (config.AppSettings.Settings[key] != null)
                {
                    if(config.AppSettings.Settings[key].Value != _Setup[key]) config.AppSettings.Settings[key].Value = _Setup[key];
                }
                else
                {
                    config.AppSettings.Settings.Add(key, _Setup[key]);
                }
            }

            config.Save(ConfigurationSaveMode.Full);
        }

        public static bool SetupFolder()
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();

            dlg.RootFolder = Environment.SpecialFolder.Desktop;
            dlg.Description = "Please set the path to your SoulHunterZ Runtime Folder.";

            if (IsSHFolder()) dlg.SelectedPath = strProjectFolder;

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                strProjectFolder = dlg.SelectedPath;
                SetSetting("ProjectFolder", strProjectFolder);
                
                SaveSetting();

                return true;
            }
            return false;
        }
        #endregion

        #region XML Serialization
        // XML Serialization
        public static void Serialize<T>(string filename, T data, Encoding encoding)
        {
            TextWriter writer = null;
            try
            {
                writer = new StreamWriter(filename, false, encoding);
                XmlSerializer serializer = new XmlSerializer(typeof(T));
                serializer.Serialize(writer, data);
            }
            finally
            {
                if (writer != null)
                {
                    writer.Close();
                }
            }
        }

        // XML Deserialization
        public static T Deserialize<T>(string filename, Encoding encoding)
        {
            TextReader reader = null;
            T data = default(T);
            try
            {
                reader = new StreamReader(filename, encoding);
                XmlSerializer serializer = new XmlSerializer(typeof(T));
                data = (T)serializer.Deserialize(reader);
            }
            catch (Exception e)
            {
                MessageBox.Show("Error: [" + filename + "] " + e.Message + "/" + e.InnerException + "XML Deserializer Error'd");
//                Environment.Exit(-1);
            }
            finally
            {
                if (reader != null)
                {
                    reader.Close();
                }
            }
            return data;
        }
        #endregion

        #region Sync
        internal static bool MatchFiles(string szFileName1, string szFileName2)
        {
            if (szFileName1 == szFileName2) return true;
            if (!File.Exists(szFileName1) || !File.Exists(szFileName2)) return false;

            FileStream fsPath1 = new FileStream(szFileName1, FileMode.Open);
            FileStream fsPath2 = new FileStream(szFileName2, FileMode.Open);

            if (fsPath1.Length != fsPath2.Length)
            {
                fsPath1.Close();
                fsPath2.Close();

                return false;
            }
            int byte1, byte2;
            int count = 1;

            do
            {
                byte1 = fsPath1.ReadByte();
                byte2 = fsPath2.ReadByte();
                count++;
            } while ((byte1 == byte2) && (byte1 != -1));

            fsPath1.Close();
            fsPath2.Close();

            return ((byte1 - byte2) == 0);
        }
        #endregion
        
        #region 파일체크
        /// <summary>
        /// 좀 심각하게 엉터리로 짠 코드.
        /// Woven into some seriously crappy code.
        /// </summary>
        /// <param name="szFileName">Comparing files.</param>
        private static void SyncFiles(string szFileName)
        {
            try
            {
                if (!MatchFiles(GetServerSystemFolder() + szFileName, GetClientSystemFolder() + szFileName))
                {
                    CompareFileForm dlg = new CompareFileForm();

                    if (!File.Exists(GetServerSystemFolder() + szFileName)) dlg.btnCopyServer.Enabled = false;
                    if (!File.Exists(GetClientSystemFolder() + szFileName)) dlg.btnCopyClient.Enabled = false;

                    dlg.lblFileName.Text = Path.GetFileName(szFileName);

                    switch (dlg.ShowDialog())
                    {
                        case DialogResult.Yes: // Server File Sync
                            File.Copy(GetServerSystemFolder() + szFileName, GetClientSystemFolder() + szFileName, true);
                            break;
                        case DialogResult.No: // Cloud File Sync
                            File.Copy(GetClientSystemFolder() + szFileName, GetServerSystemFolder() + szFileName, true);
                            break;
                        case DialogResult.Retry:
                            throw new Exception("Retrying. Try to update your SVN first.");
                        case DialogResult.Abort:
                            Environment.Exit(0);
                            break;
                    }
                }
            }
            catch (Exception e)
            {
                throw e;
            }
        }

        private static void myValidationEventHandler(object sender, ValidationEventArgs e)
        {
            throw new Exception("XML Syntax Error: " + Path.GetFileName(e.Exception.SourceUri) + " [Line:" + e.Exception.LineNumber + ", Pos: " + e.Exception.LinePosition + "] " + e.Exception.Message);
        }

        private static void GrammarCheck(string szFileName, string szXsdName)
        {
            XmlReaderSettings settings = new XmlReaderSettings();

            settings.ConformanceLevel = ConformanceLevel.Document;
            settings.IgnoreComments = true;
            settings.IgnoreWhitespace = true;
            settings.CloseInput = true;
            settings.ValidationFlags = XmlSchemaValidationFlags.ReportValidationWarnings;

            if (szXsdName == null || szXsdName == "") settings.ValidationType = ValidationType.None; else settings.ValidationType = ValidationType.Schema;

            settings.ValidationEventHandler += new ValidationEventHandler(myValidationEventHandler);

            if (szXsdName != null && szXsdName != "")
            {
                try
                {
                    settings.Schemas.Add(null, szXsdName);
                }
                catch (XmlSchemaException ex)
                {
                    throw new Exception("Schema Error File: " + szXsdName + " [Line:" + ex.LineNumber + ", Pos:" + ex.LinePosition + "] " + ex.Message);
                }
                catch (ArgumentNullException ex)
                {
                    throw new Exception("Null Exception: " + ex.Message + " (Contact the Programmers.");
                }
            }

            XmlReader reader = XmlReader.Create(szFileName, settings);

            try
            {
                while (reader.Read()) ;
            }
            catch (XmlException ex)
            {
                throw new Exception("XML Syntax Error: " + szFileName + " [Line:" + ex.LineNumber + ", Pos:" + ex.LinePosition + "] " + ex.Message);
            }
            finally
            {
                reader.Close();
            }
        }

        /// <summary>
        /// File Checking
        /// </summary>
        public static void CheckFiles(BackgroundWorker bgWorker)
        {
            try
            {
                int nProgressStep = 100 / _checkFiles.Count;
                int nCurrentProgress = 0;

                // 파일이 존재하는지 먼저 체크
                foreach (SHSmartCheckFile checkFile in _checkFiles)
                {
                    if (bgWorker != null && bgWorker.CancellationPending) return;
                    if (checkFile.CheckType == SHSCF_Type.Both)
                    {
                        if (bgWorker != null) bgWorker.ReportProgress(nCurrentProgress, Path.GetFileName(checkFile.FilePath) + " Sync Check");
                        SyncFiles(checkFile.FilePath);
                    }

                    if (checkFile.CheckType == SHSCF_Type.Both || checkFile.CheckType == SHSCF_Type.Server)
                    {
                        // 서버쪽 파일체크
                        if (!File.Exists(GetServerSystemFolder() + checkFile.FilePath))
                        {
                            throw new Exception(GetServerSystemFolder() + checkFile.FilePath + "No files.");
                        }
                        else
                        {
                            if (bgWorker != null) bgWorker.ReportProgress(nCurrentProgress, Path.GetFileName(checkFile.FilePath) + " Syntax check.");
                            if (checkFile.XSDPath != "") GrammarCheck(GetServerSystemFolder() + checkFile.FilePath, GetServerSystemFolder() + checkFile.XSDPath); else GrammarCheck(GetServerSystemFolder() + checkFile.FilePath, null);
                        }
                    }
                    if (checkFile.CheckType == SHSCF_Type.Both || checkFile.CheckType == SHSCF_Type.Client)
                    {
                        // 클라쪽 파일체크
                        if (!File.Exists(GetClientSystemFolder() + checkFile.FilePath))
                        {
                            throw new Exception(GetClientSystemFolder() + checkFile.FilePath + "(c)No files.");
                        }
                        else
                        {
                            if (bgWorker != null) bgWorker.ReportProgress(nCurrentProgress, Path.GetFileName(checkFile.FilePath) + "(c) Syntax Check");
                            if (checkFile.XSDPath != "") GrammarCheck(GetClientSystemFolder() + checkFile.FilePath, GetServerSystemFolder() + checkFile.XSDPath); else GrammarCheck(GetClientSystemFolder() + checkFile.FilePath, null);
                        }
                    }

                    nCurrentProgress += nProgressStep;
                    if (bgWorker != null) bgWorker.ReportProgress(nCurrentProgress, Path.GetFileName(checkFile.FilePath) + " Check Complete.");
                }
            }
            catch (Exception e)
            {
                throw e;
            }
        }
        #endregion
    }
    #endregion

    #region More Sync shit(SHSCF_Type, SHSmartCheckFile)
    public enum SHSCF_Type
    {
        Both,
        Client,
        Server
    }

    public sealed class SHSmartCheckFile
    {
        public string FilePath { get; set; }
        public SHSCF_Type CheckType { get; set; }
        public string XSDPath { get; set; }
        public Encoding EncodingType { get; set; }

        public SHSmartCheckFile(string szFilePath, SHSCF_Type eType, Encoding encType)
        {
            FilePath = szFilePath;
            XSDPath = "";
            CheckType = eType;
            EncodingType = encType;
        }

        public SHSmartCheckFile(string szFilePath, string szXsdPath, SHSCF_Type eType, Encoding encType)
        {
            FilePath = szFilePath;
            XSDPath = szXsdPath;
            CheckType = eType;
            EncodingType = encType;
        }
    }
    #endregion

    #region SHXmlDocument
    /// <summary>
    /// SHXmlDocument는 Xml 파일을 읽는데 사용되는 XmlDocument의 새로 작성된 코드이다. 이 클래스는 노드의 해당 파일내 위치를 트랙킹하는데 도움을 준다.
    /// 다만 현재로서는 사용되지 않으며, 일단은 백업으로 존재.
    /// </summary>
    internal sealed class SHXmlDocument : XmlDocument
    {
        IXmlLineInfo lineInfo;

        public SHXmlDocument():base()
        {
        }

        public override XmlElement CreateElement(string prefix, string localname, string nsURI)
        {
            return (lineInfo != null)
                ? new SHXmlElement(prefix, localname, nsURI, this, lineInfo.LineNumber, lineInfo.LinePosition)
                : new SHXmlElement(prefix, localname, nsURI, this);
        }

        public override void Load(XmlReader reader)
        {
            if (reader is IXmlLineInfo)
                lineInfo = (IXmlLineInfo)reader;
            base.Load(reader);
            lineInfo = null;
        }
    }

    internal sealed class SHXmlElement : XmlElement, IXmlLineInfo
    {
        #region Fields
        int lineNumber = 0;
        int linePosition = 0;
        #endregion

        public SHXmlElement(string prefix, string localname, string nsURI, XmlDocument doc)
            : base(prefix, localname, nsURI, doc)
        {
        }

        public SHXmlElement(string prefix, string localname, string nsURI, XmlDocument doc, int lineNum, int linePos)
            : base(prefix, localname, nsURI, doc)
        {
            this.lineNumber = lineNum;
            this.linePosition = linePos;
        }

        public void SetLineInfo(int linenum, int linepos)
        {
            lineNumber = linenum;
            linePosition = linepos;
        }

        public bool HasLineInfo()
        {
            return true;
        }

        public int LineNumber
        {
            get { return lineNumber; }
        }

        public int LinePosition
        {
            get { return linePosition; }
        }
    }
    #endregion

    #region SHDataObject
    /// <summary>
    /// ISHCompilable : 컴파일해서 메시지를 생성할 것들을 정의한 인터페이스
    /// </summary>
    public abstract class SHDataObject
    {
        protected String _szCompiled;
        protected bool _bPassed;

        [Browsable(false)]
        public string CompiledMessage { get { return _szCompiled; } }
        [Browsable(false)]
        public bool Passed { get { return _bPassed; } }

        protected bool Error(String message) { if (message != null) { _szCompiled = message; _bPassed = false; } return _bPassed; }

        public abstract bool Compile();
        public abstract bool Build(SHXmlCore core);
        public abstract string ToBody(SHXmlCore core);

        public virtual string GetHTML(SHXmlCore core)
        {
            return Properties.Resources.xhtml_template.Replace("[[CONTENT_BODY]]", ToBody(core));
        }
    }

    public abstract class SHXmlDataObject : SHDataObject
    {
        protected bool _bLoadingFailed;
        [Browsable(false)]
        public bool LoadingFailed 
        { 
            get { return _bLoadingFailed; }
//            set { _bLoadingFailed = value; }
        }
        public SHXmlDataObject()
        {
            _bLoadingFailed = false;
        }
        public void LoadingFail()
        {
            _bLoadingFailed = true;
        }
        public virtual void Sort()
        {

        }
        public virtual bool IsValid(String szIndex)
        {
            return true;
        }
        public virtual bool IsValid(Int32 nIndex)
        {
            return true;
        }

    }

    #endregion

    #region SHTextUtil
    public static class SHTextUtil
    {
        public static string GetTextFromTextBox(TextBox tbText)
        {
            string ret = "";
            int nCount = 1;

            foreach (string tmp in tbText.Lines)
            {
                if (nCount != 1) ret += "[br]";
                ret += tmp;
                nCount++;
            }

            return ret;
        }
        public static string GetTextFromTextBox(RichTextBox tbText)
        {
            string ret = "";
            int nCount = 1;

            foreach (string tmp in tbText.Lines)
            {
                if (nCount != 1) ret += "[br]";
                ret += tmp;
                nCount++;
            }

            return ret;
        }

        public static void SetTextToTextBox(TextBox tbText, string str)
        {
            if (tbText.Text != null && tbText.Text != "") tbText.Text = "";

            tbText.Lines = str.Split(new string[] { "[br]" }, StringSplitOptions.None);
        }

        public static void SetTextToTextBox(RichTextBox tbText, string str)
        {
            if (tbText.Text != null && tbText.Text != "") tbText.Text = "";

            tbText.Lines = str.Split(new string[] { "[br]" }, StringSplitOptions.None);
        }

    }
    #endregion
    
    #region List View stuff
    public static class SHListViewUtil
    {
        public static void SelectIndex(ListView lvList, int nIndex)
        {
            if (lvList.Items.Count > 0)
            {
                if (lvList.SelectedItems.Count > 0) lvList.SelectedIndices.Clear();
                nIndex = Math.Max(Math.Min(nIndex, lvList.Items.Count - 1), 0);

                ListViewItem lvi = lvList.Items[nIndex];

                try
                {
                    lvList.SelectedIndices.Add(nIndex);
                    if (!lvList.ClientRectangle.Contains(lvi.Position))
                    {
                        lvList.Items[nIndex].EnsureVisible();   // 해당 Item으로 이동
                    }
                }
                catch (ArgumentOutOfRangeException)
                {
                    lvList.SelectedIndices.Add(0);
                    lvList.Items[0].EnsureVisible();
                }
            }
        }

        /// <summary>
        /// 문자열을 검색한다.
        /// </summary>
        /// <param name="lvList">검색할 리스트뷰</param>
        /// <param name="szFind">검색할 문자열</param>
        /// <returns></returns>
        public static ListViewItem FullTextSearch(ListView lvList, String szFind, int nStartIndex)
        {
            for (int i = nStartIndex; i < lvList.Items.Count; i++)
            {
                ListViewItem lvi = lvList.Items[i];
                if (lvi.Text.Contains(szFind))
                {
                    return lvi;
                }
                else
                {
                    foreach (ListViewItem.ListViewSubItem item in lvi.SubItems)
                    {
                        if (item.Text.Contains(szFind))
                        {
                            return lvi;
                        }
                    }
                }
            }

            return null;
        }
    }
    #endregion

    #region Property Grid
    public static class SHPropertyGridUtil
    {
        public static GridItemCollection GetAllGridEntries(PropertyGrid grid)
        {
            if (grid == null)
                throw new ArgumentNullException("grid");

            object view = grid.GetType().GetField("gridView", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance).GetValue(grid);
            return (GridItemCollection)view.GetType().InvokeMember("GetAllGridEntries", System.Reflection.BindingFlags.InvokeMethod | System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance, null, view, null);
        }

        public static void SelectGridItem(PropertyGrid pg, string strGridLabel)
        {
            GridItemCollection gridItems = GetAllGridEntries(pg);

            foreach (GridItem gi in gridItems)
            {
                if (gi.Label == strGridLabel)
                {
                    gi.Select();
                    return;
                }
            }
            if (gridItems.Count > 0)
            {
                gridItems[0].Select();
            }
        }
    }

    #endregion

    public static class SHWebUtil
    {
        public static string ReplaceDialogTextForWeb(string szText)
        {
            if (szText == null) return "";

            szText = szText.Replace("{place}", "<font color=#6152E1>");
            szText = szText.Replace("{/place}", "</font>");
            szText = szText.Replace("{item}", "<font color=#6152E1>");
            szText = szText.Replace("{/item}", "</font>");
            szText = szText.Replace("{character}", "<font color=#6152E1>");
            szText = szText.Replace("{/character}", "</font>");
            szText = szText.Replace("{CR}", "<br/>");

            szText = szText.Replace("{", "<");
            szText = szText.Replace("}", ">");
            szText = szText.Replace("\n", "<br/>");
            szText = szText.Replace("[BR]", "<br/>");
            szText = szText.Replace("[br]", "<br/>");

            return szText;
        }

        public static string RemoveTag(string szText)
        {
            szText = szText.Replace("{place}", "");
            szText = szText.Replace("{/place}", "");
            szText = szText.Replace("{item}", "");
            szText = szText.Replace("{/item}", "");
            szText = szText.Replace("{character}", "");
            szText = szText.Replace("{/character}", "");
            szText = szText.Replace("{CR}", "");

            szText = szText.Replace("\n", "");
            szText = szText.Replace("[BR]", "");
            szText = szText.Replace("[br]", "");

            return szText;
        }

    }
}