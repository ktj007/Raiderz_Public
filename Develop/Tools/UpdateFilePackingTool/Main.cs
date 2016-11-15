using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.IO.Compression;
using SevenZip;

/// 프로그램을 빌들하기 위해서는 다음의 라이브러리가 필요하다
/// ICSharpCode.SharpZipLib  위치 ThirdParty\NET_CompressLibs\SharpZipLib_0855_Bin\net-20
/// Newtonsoft.Json.Net35     위치 ThirdParty\NET_Json\Bin\Net35
/// SevenZipSharp               위치 ThirdParty\NET_CompressLibs\7Zip



namespace UpdateFilePackingTool
{
    public partial class Main : Form
    {
        string DIFF_MARK_TEST = "[Diff]   ";
        string NEW_MARK_TEST = "[New] ";
        string PackingFileName = "UnKnown";

        string FOLDER_PREFIX_NAME = "Ver";   // 클라이언트 각 버전별 디렉토리의 시작 단어
        string PATCHINFOFILENAME = "RaiderZPatchInfo.txt"; // 패치정보 파일 이름

        List<string> PatchFileList = new List<string>(); 
        List<string> DiffTagFiles = new List<string>(); 
        List<string> CurVersionFiles = new List<string>();
        List<string> CurVersionFilesFullPath = new List<string>();
        List<string> NextVersionFiles = new List<string>();
        MakePatchInfo makePatch = new MakePatchInfo();


        public Main()
        {
            InitializeComponent();
        }

        // 현재 버전 폴더 선택
        private void btnSelCurVerFolder_Click(object sender, EventArgs e)
        {
            //textBoxCurVerFolder.Text = "E:\\Download\\Temp\\Ver1"; // 테스트나 디버깅 때 사용
            folderBrowserDialog1.RootFolder = Environment.SpecialFolder.MyComputer;
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                if (GetVersionNumber(folderBrowserDialog1.SelectedPath) == 0)
                {
                    MessageBox.Show("폴더 이름 규격이 다릅니다. 폴더이름은 Ver숫자 입니다");
                    return;
                }
                textBoxCurVerFolder.Text = folderBrowserDialog1.SelectedPath;
            }
        }

        // 다음 버전 폴더 선택
        private void btnSelNextVerFolder_Click(object sender, EventArgs e)
        {
            //textBoxNextVerFolder.Text = "E:\\Download\\Temp\\Ver2"; // 테스트나 디버깅 때 사용
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                if (GetVersionNumber(folderBrowserDialog1.SelectedPath) == 0)
                {
                    MessageBox.Show("폴더 이름 규격이 다릅니다. 폴더이름은 Ver숫자 입니다");
                    return;
                }
                textBoxNextVerFolder.Text = folderBrowserDialog1.SelectedPath;
            }
        }

        // 패치 파일 생성 폴더 선택
        private void btnSelFackingFilesFolder_Click(object sender, EventArgs e)
        {
            //textBoxPackingFilesFolder.Text = "E:\\Download\\Temp\\Update"; // 테스트나 디버깅 때 사용
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                textBoxPackingFilesFolder.Text = folderBrowserDialog1.SelectedPath;
            }
        }

        // 현재와 다음 버전간 파일 변경 비교
        private void btnAnalysisDiffFile_Click(object sender, EventArgs e)
        {
            PrepareCheckCurAndNextFiles();
            CheckCurAndNextFiles(textBoxCurVerFolder.Text, textBoxNextVerFolder.Text);
        }

        // 패치 파일을 팩킹하고 패치정보 파일을 만든다
        private void btnStartPacking_Click(object sender, EventArgs e)
        {
            CreatePackingFileName();
            
            List<PatchFileInfo> PatchFileInfoList = PatchFilesCompression();
            if( null == PatchFileInfoList ) 
            {
                 btnStartPacking.Enabled = false;
                 WriteLogToGUI("패치할 파일이 없습니다 !!");
                return;
            }
            
            int NewVersion = GetVersionNumber(textBoxNextVerFolder.Text);

            // 패치정보 파일을 만든다
            makePatch.CreatePatchInfoFile(textBoxPackingFilesFolder.Text + "\\" + PATCHINFOFILENAME, NewVersion, PatchFileInfoList);

            btnStartPacking.Enabled = false;
            WriteLogToGUI( PackingFileName + " 팩킹 파일 생성이 성공 했습니다."); 
        }

        // 테스트 버튼 - 개발할 때만 사용한다
        private void btnTest_Click(object sender, EventArgs e)
        {
            List<PatchFileInfo> PatchFileInfoList = new List<PatchFileInfo>();
            PatchFileInfo info1 = new PatchFileInfo();
            info1.FileName = "dsd1.dsd1";
            info1.FileSize = 32323;
            info1.FileCRC =3232323;
            PatchFileInfo info2 = new PatchFileInfo();
            info2.FileName = "dsd2.dsd1";
            info2.FileSize = 42323;
            info2.FileCRC = 4232323;
            PatchFileInfoList.Add( info1 );
            PatchFileInfoList.Add( info2 );
            makePatch.CreatePatchInfoFile(textBoxPackingFilesFolder.Text + "\\" + PATCHINFOFILENAME, 3, PatchFileInfoList);
        }

        ///// < Functions > //////
        
        // 폴더 이름에서 버전 숫자를 추출한다.
        int GetVersionNumber( string FullPathName )
        {
            int nVerNumber = 0;
            FileInfo fileinfo = new FileInfo(FullPathName);
            string FolderName = fileinfo.Name;

            if (FolderName.IndexOf(FOLDER_PREFIX_NAME, 0) != 0)
            {
                return nVerNumber;
            }

            string VersionNumber = FolderName.Substring(3, FolderName.Length - 3);
            if( VersionNumber.Length < 1 ) {
                return nVerNumber;
            }

            try {
                nVerNumber = Convert.ToInt32(VersionNumber);
            } catch {
                nVerNumber = 0;
            }
            return nVerNumber;
        }

        // 패치 파일이 있는지 조사한다.
        bool IsHavePatchInfoFile(string FullPathFolder)
        {
            string[] FildFiles = Directory.GetFiles(FullPathFolder, PATCHINFOFILENAME, SearchOption.TopDirectoryOnly);
            if (1 != FildFiles.Length) {
                return false;
            }
            return true;
        }

        // 특정 폴더나 파일을 그냥 통과시킨다
        bool IsPassFolderAndFile(string FullFilePath)
        {
            /// svn 폴더나 그 안의 모든 파일 패스
            if (0 <= FullFilePath.IndexOf(".svn\\") || 0 <= FullFilePath.IndexOf("CRASH\\") || 0 <= FullFilePath.IndexOf("log\\") )
            {
                return true;
            }

            return false;
        }

        // 버전 비교를 하기 전에 해야할 작업
        void PrepareCheckCurAndNextFiles()
        {
            PatchFileList.Clear();
            DiffTagFiles.Clear();
            CurVersionFiles.Clear();
            CurVersionFilesFullPath.Clear();
            NextVersionFiles.Clear();

            CurVersionlistBox.Items.Clear();
            NextVersionlistBox.Items.Clear();
            UpdateFileslistBox.Items.Clear();
            LoglistBox.Items.Clear();

            btnStartPacking.Enabled = false;
        }

        // 현재와 다음 버전 비교
        void CheckCurAndNextFiles(string CurVersionFold, string NextVersionFold)
        {
            int CurVersionFoldTextLen = CurVersionFold.Length;
            int NextVersionFoldTextLen = NextVersionFold.Length;

            List<string> OldFileFiles = new List<string>();

            this.Cursor = Cursors.WaitCursor;

            // 현재 버전의 모든 파일,폴더를 리스팅한다.
            try
            {
                // 지금 폴더에 있는 디렉토리와 이 디렉토리들의 모든 하위 디렉토리에 있는 모든 파일들을 컨테이너에 담는다.
                string[] Cur_AllFiles = Directory.GetFiles(CurVersionFold, "*.*", SearchOption.AllDirectories);
                foreach (string Entry in Cur_AllFiles)
                {
                    if (IsPassFolderAndFile(Entry)) {
                        continue;
                    }

                    CurVersionFilesFullPath.Add(Entry); // 풀 파일 패스
                    CurVersionFiles.Add(Entry.Substring(CurVersionFoldTextLen + 1)); // 시작 폴더는 제거한 파일 패스
                }


                // 다음 버전의 파일,폴더를 읽으면서 현재 버전과 비교한다.
                int iFindIndex = 0;
                string NoneStartFolder;
                string[] Next_AllFiles = Directory.GetFiles(NextVersionFold, "*.*", SearchOption.AllDirectories);
                foreach (string Entry in Next_AllFiles)
                {
                    if (IsPassFolderAndFile(Entry)) {
                        continue;
                    }

                    NoneStartFolder = Entry.Substring(NextVersionFoldTextLen + 1);  // 시작 폴더를 제거한다.
                    NextVersionFiles.Add(NoneStartFolder);
                    iFindIndex = CurVersionFiles.IndexOf(NoneStartFolder);

                    if (iFindIndex >= 0) // 현재 버전에 있는 파일이다
                    {
                        // 파일의 크기나 마지막 쓰기 시간이 다르면 다른 파일이다.
                        FileInfo Cur_fileinfo = new FileInfo(CurVersionFilesFullPath[iFindIndex]);
                        FileInfo Next_fileinfo = new FileInfo(Entry);

                        if (
                            /*Cur_fileinfo.LastWriteTime != Next_fileinfo.LastWriteTime ||*/    // svn상 파일 일자만 바뀔 수 있다
                            Cur_fileinfo.Length != Next_fileinfo.Length ||
                            false == IsSameCRC( Cur_fileinfo.FullName, Next_fileinfo.FullName ) ) 
                        {
                            // 다음 버전의 파일이 현재 버전 보다 더 오래된 파일이다.
                            if (Cur_fileinfo.LastWriteTime > Next_fileinfo.LastWriteTime) {
                                WriteLogToGUI( string.Format("현재 버전 파일이 다음 버전 파일보다 최신:{0}", Next_fileinfo.FullName) );
                            }

                            DiffTagFiles.Add(DIFF_MARK_TEST + Entry);
                            PatchFileList.Add(Entry);
                        }
                    }
                    else
                    {
                        DiffTagFiles.Add(NEW_MARK_TEST + Entry);
                        PatchFileList.Add(Entry);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());

            }
            finally
            {
                this.Cursor = Cursors.Default;
            }

            btnStartPacking.Enabled = true;


            ShowCurVersionFiles(CurVersionFiles);
            ShowNextVersionFiles(NextVersionFiles);
            ShowUpdateFiles(DiffTagFiles);
        }

        // 파일의 CRC 값 반환
        Int64 GetCRC(string FileName)
        {
            ICSharpCode.SharpZipLib.Checksums.Crc32 crc = new ICSharpCode.SharpZipLib.Checksums.Crc32();

            System.IO.FileStream fs1 = new System.IO.FileStream(FileName, System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.Read);
            byte[] buffer1 = new byte[fs1.Length];
            fs1.Read(buffer1, 0, buffer1.Length);
            fs1.Close();
            crc.Reset();
            crc.Update(buffer1);
            return crc.Value;
        }

        // 두 개의 파일이 CRC가 같은지 비교. 같으면 true 반환
        bool IsSameCRC( string FileName1, string FileName2 )
        {
            long File1Crc = GetCRC(FileName1);
            long File2Crc = GetCRC(FileName2);

            if( File1Crc == File2Crc ) {
                return true;
            }

            return false;
        }

        // 패킹할 파일의 이름을 생성.
        void CreatePackingFileName()
        {
            // "현재 버전 폴더 이름" + "_" + "다음 버전 폴더 이름" 으로 만든다
            int index = textBoxCurVerFolder.Text.LastIndexOf(@"\");
            string CurVersionFoldName = textBoxCurVerFolder.Text.Substring(index + 1);

            index = textBoxNextVerFolder.Text.LastIndexOf(@"\");
            string NextVersionFoldName = textBoxNextVerFolder.Text.Substring(index + 1);

            PackingFileName = CurVersionFoldName + "_" + NextVersionFoldName;
        }

        // 패치할 파일을 압축한다.
        List<PatchFileInfo> PatchFilesCompression()
        {
            if( PatchFileList.Count < 1 ) {
                return null;
            }

            List<PatchFileInfo> PatchFileInfoList = PatchFilesCompression_Zip();   // zip 압축
            //List<PatchFileInfo> PatchFileInfoList = DiffFileCompression_7Zip();    // 7zip 압축
            return PatchFileInfoList;
        }

        // 패치할 파일들을 zip 으로 압축한다.
        List<PatchFileInfo> PatchFilesCompression_Zip()
        {
            List<PatchFileInfo> PatchFileInfoList = new List<PatchFileInfo>();
            PatchFileInfo patchfileinfo = new PatchFileInfo();

            // 사용법 http://dobon.net/vb/dotnet/links/sharpziplib.html
            // SharpZipLib 사이트 http://www.icsharpcode.net/OpenSource/SharpZipLib/
            try
            {
                this.Cursor = Cursors.WaitCursor;

                //ICSharpCode.SharpZipLib.Checksums.Crc32 crc = new ICSharpCode.SharpZipLib.Checksums.Crc32();

                string zipFullPathName = textBoxPackingFilesFolder.Text + @"\" + PackingFileName + ".zip";
                System.IO.FileStream writer = new System.IO.FileStream(zipFullPathName, System.IO.FileMode.Create,
                                                                    System.IO.FileAccess.Write, System.IO.FileShare.Write);
                ICSharpCode.SharpZipLib.Zip.ZipOutputStream zos = new ICSharpCode.SharpZipLib.Zip.ZipOutputStream(writer);
                
                // 압축레벨을 설정
                //zos.SetLevel(6);
                // 패스워드를 설정한다.
                //zos.Password = "ekdltmsakstp";

                foreach (string file in PatchFileList)
                {
                    int Substringindex = textBoxNextVerFolder.Text.Length;
                    string f = file.Substring(Substringindex + 1);

                    ICSharpCode.SharpZipLib.Zip.ZipEntry ze = new ICSharpCode.SharpZipLib.Zip.ZipEntry(f);

                    System.IO.FileStream fs = new System.IO.FileStream(file, System.IO.FileMode.Open, System.IO.FileAccess.Read,
                                                                            System.IO.FileShare.Read);

                    byte[] buffer = new byte[fs.Length];
                    fs.Read(buffer, 0, buffer.Length);
                    fs.Close();

                    // CRC를 설정한다
                    //crc.Reset();
                    //crc.Update(buffer);
                    //ze.Crc = crc.Value;

                    // 사이즈를 설정한다
                    ze.Size = buffer.Length;

                    // 시간을 설정한다
                    ze.DateTime = DateTime.Now;

                    // 새로운 파일을 추가
                    zos.PutNextEntry(ze);

                    // 쓰기
                    zos.Write(buffer, 0, buffer.Length);
                }

                zos.Close();
                writer.Close();

                patchfileinfo.FileName = new FileInfo(zipFullPathName).Name;
                patchfileinfo.FileCRC = GetCRC(zipFullPathName);
                patchfileinfo.FileSize = new FileInfo(zipFullPathName).Length;

                PatchFileInfoList.Add(patchfileinfo);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
            finally
            {
                this.Cursor = Cursors.Default;
            }

            return PatchFileInfoList;
        }

        // 패치할 파일들을 7zip으로 압축한다
        List<PatchFileInfo> PatchFilesCompression_7Zip()
        {
            List<PatchFileInfo> PatchFileInfoList = new List<PatchFileInfo>();
            PatchFileInfo patchfileinfo = new PatchFileInfo();

            // 사용법 
            // SevenZipSharp 사이트 http://sevenzipsharp.codeplex.com/
            try
            {
                 this.Cursor = Cursors.WaitCursor;
                string zipName = textBoxPackingFilesFolder.Text + @"\" + PackingFileName + ".7z";
                                
                string[] CompressFileNames = GetCompressFileNames(PatchFileList);

                /// 7Zip 라이브러리를 사용하기 위해서 꼭 7z.dll 파일을 실행파일이 있는 곳에 놓아두고 설정한다 
                SevenZipCompressor.SetLibraryPath(String.Format(@"{0}\7z.dll", Application.StartupPath));

                SevenZipCompressor szc = new SevenZipCompressor();  
                szc.CompressionMethod = CompressionMethod.Lzma;
                szc.CompressionMode = SevenZip.CompressionMode.Create;
                szc.CompressionLevel = CompressionLevel.Normal;
                szc.CompressFiles(zipName, CompressFileNames);

                
                patchfileinfo.FileName = new FileInfo(zipName).Name;
                patchfileinfo.FileCRC = GetCRC(zipName);
                patchfileinfo.FileSize = new FileInfo(zipName).Length;
                
                PatchFileInfoList.Add(patchfileinfo);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
            finally
            {
                this.Cursor = Cursors.Default;
            }

            return PatchFileInfoList;
        }
        
        // 압축할 파일 이름 리스트
        string[] GetCompressFileNames( List<string> FileNameList )
        {
            string[] CompressFileNames = new string[FileNameList.Count];
            int i = 0;
            foreach (string file in FileNameList)
            {
                CompressFileNames[i] = file;
                ++i;
            }

            return CompressFileNames;
        }
        
        // 현재 버전의 파일들을 보여준다
        void ShowCurVersionFiles( List<string> Files )
        {
            if (false == checkBoxCurVerFiles.Checked) {
                return;
            }

            foreach (string FileName in Files)
            {
                CurVersionlistBox.Items.Add(FileName);
            }
        }

        // 다음 버전의 파일들을 보여준다
        void ShowNextVersionFiles(List<string> Files)
        {
            if (false == checkBoxCurVerFiles.Checked) { 
                return;
            }

            foreach (string FileName in Files)
            {
                NextVersionlistBox.Items.Add(FileName);
            }
        }

        // 패치할 파일들을 보여준다
        void ShowUpdateFiles(List<string> Files)
        {
            foreach (string FileName in Files)
            {
                UpdateFileslistBox.Items.Add(FileName);
            }
        }

        // GUI로 로그를 출력한다
        void WriteLogToGUI( string logtext )
        {
            LoglistBox.Items.Add(logtext);
        }

        
    }
}
