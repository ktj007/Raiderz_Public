

using System.ComponentModel;
using System.Windows.Forms;
using System.IO;
using Configuration;


namespace SoundManager
{
    public class SoundTypeConverter : StringConverter
    {
        public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
        {
            return true;
        }
        public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
        {
            return true;
        }
        public override System.ComponentModel.TypeConverter.StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
        {
            return new StandardValuesCollection(SoundItemProperty.typeSound);
        }
    }
    public class SoundSearchButton : System.Drawing.Design.UITypeEditor
    {
        public override System.Drawing.Design.UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
		{
			if (context != null&& context.Instance != null)
			{
				if (! context.PropertyDescriptor.IsReadOnly)
					return System.Drawing.Design.UITypeEditorEditStyle.Modal;
			}

            return System.Drawing.Design.UITypeEditorEditStyle.None;
		}
		
        public override object EditValue(ITypeDescriptorContext context, System.IServiceProvider provider, object value)
		{
			if (context == null || provider == null || context.Instance == null)
				return base.EditValue(provider, value);

            string pathName = (value.ToString() != string.Empty) ? Path.Combine(Config.GetOption("WORKINGDIR"), value.ToString()) : Config.GetOption("WORKINGDIR");

            System.Windows.Forms.OpenFileDialog dialogFile = new System.Windows.Forms.OpenFileDialog();
            dialogFile.Title = "Select a file";
            dialogFile.Filter = "WAV files (*.wav)|*.wav|OGG files (*.ogg)|*.ogg|MP3 files (*.mp3)|*.mp3|All files (*.*)|*.*";
            dialogFile.FilterIndex = int.Parse(Config.GetOption("EXTFILTER", "4"));
            if ( pathName != string.Empty)
            {
                dialogFile.InitialDirectory = Path.GetDirectoryName(pathName);
                dialogFile.FileName = Path.GetFileName(pathName);
            }

            if (dialogFile.ShowDialog() == System.Windows.Forms.DialogResult.OK && dialogFile.FileName != System.String.Empty)
            {
                System.Uri uriRoot = new System.Uri(Config.GetOption("WORKINGDIR"));
                System.Uri uriFile = new System.Uri(dialogFile.FileName);
                System.Uri relativeUri = uriRoot.MakeRelativeUri(uriFile);
                value = relativeUri.ToString().Replace('\\', '/');

                Config.SetOption("EXTFILTER", dialogFile.FilterIndex.ToString());
            }

            dialogFile.Dispose();

            return value;
		}
	}

    public class SoundItemProperty
    {
        internal static string[] typeSound = { "2D", "3D", "BGM" };


        // Name
        [Category("Default Settings"), DescriptionAttribute("이 사운드의 참조 이름을 설정합니다."), DisplayName("Name")]
        public string Name
        {
            get { return _name; }
            set { _name = value; }
        }
        private string _name = "";


        // File Name
        [Category("Default Settings"), DescriptionAttribute("이 사운드의 파일 이름을 설정합니다."), DisplayName("File Name")]
        [EditorAttribute(typeof(SoundSearchButton), typeof(System.Drawing.Design.UITypeEditor))]
        public string FileName
        {
            get { return _fname; }
            set { _fname = value; }
        }
        private string _fname = "";

        
        // Type
        [Category("Default Settings"), DescriptionAttribute("이 사운드의 재생 형식을 설정합니다."), DisplayName("Type")]
        [TypeConverter(typeof(SoundTypeConverter))]
        public string Type
        {
            get
            {
                string retVal = "";
                if (_type != null)
                    retVal = _type;
                else
                    retVal = typeSound[0];

                return retVal;
            }
            set { _type = value; }
        }
        private string _type = typeSound[0];

        
        // Volume
        [Category("Default Settings"), DescriptionAttribute("이 사운드의 볼륨을 퍼센트 단위로 설정합니다. (입력 범위 : 0~100)"), DisplayName("Volume")]
        public int Volume
        {
            get { return _volume; }
            set { _volume = value; }
        }
        private int _volume = 100;


        // Loop
        [Category("Default Settings"), DescriptionAttribute("사운드를 무한 반복 재생시킵니다."), DisplayName("Loop")]
        public bool Loop
        {
            get { return _loop; }
            set { _loop = value; }
        }
        private bool _loop = false;

        
        // Unique
        [Category("Default Settings"), DescriptionAttribute("오직 하나의 사운드만 재생할 수 있도록 합니다."), DisplayName("Unique")]
        public bool Unique
        {
            get { return _unique; }
            set { _unique = value; }
        }
        private bool _unique = false;


        // Min distance
        [Category("Three-Dimensional Settings"), DescriptionAttribute("사운드가 재생될 때 거리에 따른 볼륨이 최대가 되는 거리를 센치미터(cm) 단위로 설정합니다."), DisplayName("Min Distance")]
        public int MinDistance
        {
            get { return _mindist; }
            set { _mindist = value; }
        }
        private int _mindist = 100;


        // Max distance
        [Category("Three-Dimensional Settings"), DescriptionAttribute("사운드가 재생될 때 거리에 따른 볼륨이 최소가 되는 거리를 센치미터(cm) 단위로 설정합니다."), DisplayName("Max Distance")]
        public int MaxDistance
        {
            get { return _maxdist; }
            set { _maxdist = value; }
        }
        private int _maxdist = 1000;

        
        // Priority
        [Category("Three-Dimensional Settings"), DescriptionAttribute("이 사운드의 우선순위를 설정합니다. 값이 낮을 수록 더 우선순위가 높습니다. (입력 범위 : 0~255)"), DisplayName("Priority")]
        public int Priority
        {
            get { return _priority; }
            set { _priority = value; }
        }
        private int _priority = 128;
    }
    public class SoundGroupProperty
    {
        // Name
        [Category("Default Settings"), DescriptionAttribute("이 사운드 그룹의 이름을 설정합니다."), DisplayName("Name")]
        public string Name
        {
            get { return _name; }
            set { _name = value; }
        }
        private string _name = "";
   }
}