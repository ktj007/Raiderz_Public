using System;
using System.Collections.Generic;
using System.Text;

using System.IO;
using System.Xml;
using System.Xml.Serialization;
using System.Xml.Schema;
using System.ComponentModel;

namespace Config
{
    class ConfigData
    {
        private int _video_width;
        private int _video_height;
        private int _video_colorbits;
        private int _video_depthbits;
        private bool _video_fullscreen;
        private bool _video_shader;
        private bool _audio_mute;
        private float _audio_bgm_volume;
        private float _audio_effect_volume;
        private int _network_port;

        public ConfigData()
        {

            _video_width = 800;
            _video_height = 600;
            _video_colorbits = 32;
            _video_depthbits = 24;
            _video_fullscreen = true;    
            _video_shader = true;
            _audio_mute = false;
            _audio_bgm_volume = 1.0f;
            _audio_effect_volume = 1.0f;
            _network_port = 7400;
        }
        [CategoryAttribute("Video")]
        public int Width
        {
            get { return _video_width; }
            set { _video_width = value; }
        }

		[CategoryAttribute("Video")]
		public int Height
		{
			get {return _video_height;}
			set {_video_height=value;}
		}
		[CategoryAttribute("Video")]
		public bool FullScreen
		{
			get {return _video_fullscreen;}
			set {_video_fullscreen=value;}
		}
		[CategoryAttribute("Video")]
		public int ColorBits
		{
			get {return _video_colorbits;}
			set {_video_colorbits=value;}
		}
		[CategoryAttribute("Video")]
		public int DepthBits
		{
			get {return _video_depthbits;}
			set {_video_depthbits=value;}
		}
        [CategoryAttribute("Video")]
        public bool Shader
        {
            get { return _video_shader; }
            set { _video_shader = value; }
        }

		[DescriptionAttribute("아직 적용되지 않음"),
			CategoryAttribute("Audio")]
		public bool Mute
		{
			get {return _audio_mute;}
			set {_audio_mute=value;}
		}
		[DescriptionAttribute("아직 적용되지 않음"),
			CategoryAttribute("Audio")]
		public float BgmVolume
		{
			get {return _audio_bgm_volume;}
			set {_audio_bgm_volume=value;}
		}
		[DescriptionAttribute("아직 적용되지 않음"),
			CategoryAttribute("Audio")]
		public float EffectVolume
		{
			get {return _audio_effect_volume;}
			set {_audio_effect_volume=value;}
		}
        [CategoryAttribute("Network")]
        public int Port
        {
            get { return _network_port; }
            set { _network_port = value; }
        }

    };
}
