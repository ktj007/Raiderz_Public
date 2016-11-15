using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Newtonsoft.Json.Linq;
using System.Windows.Forms;

namespace UpdateFilePackingTool
{
    // 패치 파일 정보
    public class PatchFileInfo
    {
        public string FileName;
        public Int64 FileSize;
        public Int64 FileCRC;
    }

    class MakePatchInfo
    {
        // 패치 버전의 정보
        class PatchVersionInfo
        {
            public int VersionNumber;
            public List<PatchFileInfo> PatchFiles = new List<PatchFileInfo>();

            public void Add( string FileName, Int64 FileSize, Int64 FileCRC)
            {
                PatchFileInfo info = new PatchFileInfo();
                info.FileName = FileName;
                info.FileSize = FileSize;
                info.FileCRC = FileCRC;
                PatchFiles.Add(info);
            }
        }
        

        List<PatchVersionInfo> PatchVersionInfoList = new List<PatchVersionInfo>();    // 버전별 패치 리스트

        // 패치정보 파일 생성을 생성
        public bool CreatePatchInfoFile(string FullPathFileName, int NextVersion, List<PatchFileInfo> PatchFileInfoList)
        {
            ReadPatchInfoFile(FullPathFileName);
            
            if (false == AddNewVersionPatchInfo(NextVersion, PatchFileInfoList)) { 
                return false;
            }

            if (false == WritePatchInfoFile(FullPathFileName))
            { 
                return false;
            }
           
            return true;
        }

        // 패치정보 파일을 읽는다.
        bool ReadPatchInfoFile(string FullPathFileName)
        {
            bool bResult = false;
            PatchVersionInfoList.Clear();

            if (false == File.Exists(FullPathFileName))
            {
                //MessageBox.Show("have not file");
                return bResult;
            }

            try
            {
                StreamReader sr = File.OpenText(FullPathFileName);
                string JsonText = sr.ReadToEnd();

                // http://james.newtonking.com/pages/json-net.aspx
                // http://james.newtonking.com/projects/json/help/LINQtoJSON.html
                JObject root = JObject.Parse(JsonText);
                JArray VersionList = (JArray)root["VERSION_LIST"];

                foreach (JObject VersionInfo in VersionList)
                {
                    PatchVersionInfo patchinfo = new PatchVersionInfo();
                    patchinfo.VersionNumber = (int)VersionInfo["Version"];

                    JArray FileInfos = (JArray)VersionInfo["Files"];
                    foreach (JObject fileInfo in FileInfos)
                    {
                        patchinfo.Add((string)fileInfo["filename"], (Int64)fileInfo["size"], (Int64)fileInfo["CRC"]);
                    }

                    PatchVersionInfoList.Add(patchinfo);
                }

                bResult = true;
                sr.Close();
            }
            catch(Exception ex)
            {
                MessageBox.Show(string.Format("Failed Read PatchInfofile. file:{0}, Error:{1}", FullPathFileName, ex.ToString()));
            }
            
            return bResult;
        }

        // 패치 정보를 파일에 기록한다
        bool WritePatchInfoFile(string FullPathFileName)
        {
            bool bResult = false;

            if( PatchVersionInfoList.Count < 1 ) {
                MessageBox.Show("PatchInfo Count Zero");
                return bResult;
            }

            try
            {
                JObject PatchInfoObject = new JObject();
                JProperty root = new JProperty("VERSION_LIST");
                JArray PatchVersions = new JArray();
                foreach (PatchVersionInfo patchinfo in PatchVersionInfoList)
                {
                    JArray Files = new JArray();
                    foreach( PatchFileInfo patchfile in patchinfo.PatchFiles )
                    {
                        Files.Add( new JObject(new JProperty("filename", patchfile.FileName),
                                                    new JProperty("size", patchfile.FileSize),
                                                    new JProperty("CRC", patchfile.FileCRC)) );
                    }

                    PatchVersions.Add( new JObject(new JProperty("Version", patchinfo.VersionNumber), new JProperty("Files", Files)) );
                }
               
                PatchInfoObject.Add(new JProperty("VERSION_LIST", PatchVersions));
                File.WriteAllText(FullPathFileName, PatchInfoObject.ToString(), Encoding.ASCII);
                bResult = true;
            }
            catch(Exception ex)
            {
               MessageBox.Show(ex.ToString());
            }
            
            return bResult;
        }

        // 패치 정보 중 가장 최신 버전 숫자를 반환
        int GetLastVersion()
        {
            int Count = PatchVersionInfoList.Count;
            if( Count < 1 ) { 
                return 1;
            }

            return PatchVersionInfoList[Count-1].VersionNumber;
        }

        // 새로운 버전의 패치 정보를 추가
        bool AddNewVersionPatchInfo( int NextVersion, List<PatchFileInfo> PatchFileInfoList )
        {
            // 다음 버전이 현재 패치정보파일의 마지막 버전보다 작으면 에러
            // 다음 버전이 현재 패치정보파일의 마지막 버전보다 같으면 마지막 정보를 제거한다.
            int LastVersion = GetLastVersion();
            if (LastVersion > NextVersion)
            {
                MessageBox.Show( string.Format("Invalid NextVersion {0}", NextVersion) );
                return false;
            }
            else if (LastVersion == NextVersion)
            {
                PatchVersionInfoList.RemoveAt(PatchVersionInfoList.Count - 1);
            }

            PatchVersionInfo NewPatchInfo = new PatchVersionInfo();
            NewPatchInfo.VersionNumber = NextVersion;
            foreach (PatchFileInfo fileinfo in PatchFileInfoList)
            {
                NewPatchInfo.Add(fileinfo.FileName, fileinfo.FileSize, fileinfo.FileCRC);
            }
            PatchVersionInfoList.Add(NewPatchInfo);

            return true;
        }
    }
}
