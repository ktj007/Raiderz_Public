

using System.ComponentModel;
using System.Windows.Forms;


namespace SoundManager
{
    public class SoundReverbProperty
    {
        [DisplayName("Environment")]
        public int Environment
        {
            get { return _environment; }
            set { _environment = value; }
        }
        private int _environment = -1;


        [DisplayName("EnvSize")]
        public float EnvSize
        {
            get { return _envSize; }
            set { _envSize = value; }
        }
        private float _envSize = 7.5f;


        [DisplayName("EnvDiffusion")]
        public float EnvDiffusion
        {
            get { return _envDiffusion; }
            set { _envDiffusion = value; }
        }
        private float _envDiffusion = 1.0f;


        [DisplayName("Room")]
        public int Room
        {
            get { return _room; }
            set { _room = value; }
        }
        private int _room = -1000;


        [DisplayName("RoomHF")]
        public int RoomHF
        {
            get { return _roomHF; }
            set { _roomHF = value; }
        }
        private int _roomHF = -100;


        [DisplayName("RoomLF")]
        public int RoomLF
        {
            get { return _roomLF; }
            set { _roomLF = value; }
        }
        private int _roomLF = 0;


        [DisplayName("DecayTime")]
        public float DecayTime
        {
            get { return _decayTime; }
            set { _decayTime = value; }
        }
        private float _decayTime = 1.49f;


        [DisplayName("DecayHFRatio")]
        public float DecayHFRatio
        {
            get { return _decayHFRatio; }
            set { _decayHFRatio = value; }
        }
        private float _decayHFRatio = 0.83f;

        
        [DisplayName("DecayLFRatio")]
        public float DecayLFRatio
        {
            get { return _decayLFRatio; }
            set { _decayLFRatio = value; }
        }
        private float _decayLFRatio = 1.0f;


        [DisplayName("Reflections")]
        public int Reflections
        {
            get { return _reflections; }
            set { _reflections = value; }
        }
        private int _reflections = 0;


        [DisplayName("ReflectionsDelay")]
        public float ReflectionsDelay
        {
            get { return _reflectionsDelay; }
            set { _reflectionsDelay = value; }
        }
        private float _reflectionsDelay = 0.007f;


        [DisplayName("Reverb")]
        public int Reverb
        {
            get { return _reverb; }
            set { _reverb = value; }
        }
        private int _reverb = 200;


        [DisplayName("ReverbDelay")]
        public float ReverbDelay
        {
            get { return _reverbDelay; }
            set { _reverbDelay = value; }
        }
        private float _reverbDelay = 0.011f;


        [DisplayName("EchoTime")]
        public float EchoTime
        {
            get { return _echoTime; }
            set { _echoTime = value; }
        }
        private float _echoTime = 0.25f;


        [DisplayName("EchoDepth")]
        public float EchoDepth
        {
            get { return _echoDepth; }
            set { _echoDepth = value; }
        }
        private float _echoDepth = 0.0f;


        [DisplayName("ModulationTime")]
        public float ModulationTime
        {
            get { return _modulationTime; }
            set { _modulationTime = value; }
        }
        private float _modulationTime = 0.25f;


        [DisplayName("ModulationDepth")]
        public float ModulationDepth
        {
            get { return _modulationDepth; }
            set { _modulationDepth = value; }
        }
        private float _modulationDepth = 0.0f;


        [DisplayName("AirAbsorptionHF")]
        public float AirAbsorptionHF
        {
            get { return _airAbsorptionHF; }
            set { _airAbsorptionHF = value; }
        }
        private float _airAbsorptionHF = -5.0f;


        [DisplayName("HFReference")]
        public float HFReference
        {
            get { return _HFReference; }
            set { _HFReference = value; }
        }
        private float _HFReference = 5000.0f;


        [DisplayName("LFReference")]
        public float LFReference
        {
            get { return _LFReference; }
            set { _LFReference = value; }
        }
        private float _LFReference = 250.0f;


        [DisplayName("RoomRolloffFactor")]
        public float RoomRolloffFactor
        {
            get { return _roomRolloffFactor; }
            set { _roomRolloffFactor = value; }
        }
        private float _roomRolloffFactor = 0.0f;


        [DisplayName("Diffusion")]
        public float Diffusion
        {
            get { return _diffusion; }
            set { _diffusion = value; }
        }
        private float _diffusion = 100.0f;


        [DisplayName("Density")]
        public float Density
        {
            get { return _density; }
            set { _density = value; }
        }
        private float _density = 100.0f;
    }
}