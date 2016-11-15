

using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;


namespace SoundManager
{
    enum SoundControlType { NONE = -1, VOLUME, DIRECT, REVERB, PANLEVEL, LOWPASS, MAX };

    public class SoundControlInfo
    {
        public bool Enable;
        public PointF Start;
        public PointF StartVel;
        public PointF End;
        public PointF EndVel;

        public SoundControlInfo(bool enable, PointF start, PointF startvel, PointF end, PointF endvel)
        {
            this.Enable = enable;
            this.Start = start;
            this.StartVel = startvel;
            this.End = end;
            this.EndVel = endvel;
        }
    }

    public class SoundControlProperty
    {
        public SoundControlInfo[] Controls = new SoundControlInfo[ (int)SoundControlType.MAX];

        public SoundControlProperty()
        {
            Controls[(int)SoundControlType.VOLUME] = new SoundControlInfo(true, new PointF(0.0f, 1.0f), new PointF(0.5f, 1.0f), new PointF(1.0f, 0.0f), new PointF(0.5f, 0.0f));
            Controls[(int)SoundControlType.DIRECT] = new SoundControlInfo(false, new PointF(0.0f, 1.0f), new PointF(0.2f, 0.8f), new PointF(1.0f, 0.0f), new PointF(0.8f, 0.2f));
            Controls[(int)SoundControlType.REVERB] = new SoundControlInfo(false, new PointF(0.0f, 1.0f), new PointF(0.2f, 0.8f), new PointF(1.0f, 0.0f), new PointF(0.8f, 0.2f));
            Controls[(int)SoundControlType.PANLEVEL] = new SoundControlInfo(false, new PointF(0.0f, 0.0f), new PointF(0.2f, 0.2f), new PointF(1.0f, 1.0f), new PointF(0.8f, 0.8f));
            Controls[(int)SoundControlType.LOWPASS] = new SoundControlInfo(false, new PointF(0.0f, 0.0f), new PointF(0.2f, 0.2f), new PointF(1.0f, 1.0f), new PointF(0.8f, 0.8f));
        }


        [DescriptionAttribute("사운드의 볼륨을 설정합니다."), DisplayName("Volume")]
        public bool TypeVolume
        {
            get { return Controls[(int)SoundControlType.VOLUME].Enable; }
            set { Controls[(int)SoundControlType.VOLUME].Enable = value; }
        }

        [DescriptionAttribute("사운드의 direct를 설정합니다."), DisplayName("Direct")]
        public bool TypeDirect
        {
            get { return Controls[(int)SoundControlType.DIRECT].Enable; }
            set { Controls[(int)SoundControlType.DIRECT].Enable = value; }
        }

        [DescriptionAttribute("사운드의 반향을 설정합니다."), DisplayName("Reverb")]
        public bool TypeReverb
        {
            get { return Controls[(int)SoundControlType.REVERB].Enable; }
            set { Controls[(int)SoundControlType.REVERB].Enable = value; }
        }

        [DescriptionAttribute("사운드의 Pan-level을 설정합니다."), DisplayName("Pan-level")]
        public bool TypePanlevel
        {
            get { return Controls[(int)SoundControlType.PANLEVEL].Enable; }
            set { Controls[(int)SoundControlType.PANLEVEL].Enable = value; }
        }

        [DescriptionAttribute("사운드의 Low-pass filter를 설정합니다."), DisplayName("Low-pass filter")]
        public bool TypeLowpass
        {
            get { return Controls[(int)SoundControlType.LOWPASS].Enable; }
            set { Controls[(int)SoundControlType.LOWPASS].Enable = value; }
        }
    }
}