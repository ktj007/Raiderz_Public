using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace SoundManager
{
    public static class RealSound
    {
        public enum CONTROLTYPE
        {
            NONE = -1,
            DIRECT,
            REVERB,
            LOWPASS,
            ROLLOFF,
            PANLEVEL,
            MAX
        };
        public struct ControlInfo
        {
            public PointF Start;
            public PointF StartVel;
            public PointF End;
            public PointF EndVel;

            public ControlInfo(PointF start, PointF startvel, PointF end, PointF endvel)
            {
                this.Start = start;
                this.StartVel = startvel;
                this.End = end;
                this.EndVel = endvel;
            }
        }
        public static ControlInfo[] CONTROLS = new ControlInfo[(int)CONTROLTYPE.MAX];

        public enum REVERBTYPE
        {
            OFF,
            GENERIC,
            PADDEDCELL,
            ROOM,
            BATHROOM,
            LIVINGROOM,
            STONEROOM,
            AUDITORIUM,
            CONCERTHALL,
            CAVE,
            ARENA,
            HANGAR,
            CARPETTEDHALLWAY,
            HALLWAY,
            STONECORRIDOR,
            ALLEY,
            FOREST,
            CITY,
            MOUNTAINS,
            QUARRY,
            PLAIN,
            PARKINGLOT,
            SEWERPIPE,
            UNDERWATER,
            MAX
        };
        public static FMOD.REVERB_PROPERTIES[] REVERB = new FMOD.REVERB_PROPERTIES[(int)REVERBTYPE.MAX];
        private static REVERBTYPE reverb = 0;

        public class HRTF
        {
            public float Scale = 0.9f;
            public float MinAngle = 1.3f;
            public float MaxAngle = 2.3f;
        }
        public static HRTF HRTFDATA = new HRTF();


        private static FMOD.System system = null;
        private static FMOD.Sound sound = null;
        private static FMOD.Channel channel = null;
        private static bool ignorefod = false;

        private static FMOD.CHANNEL_CALLBACK CHANNELCALLBACK = new FMOD.CHANNEL_CALLBACK(channel_callback);
        private static FMOD.CB_3D_ROLLOFFCALLBACK ROLLOFFCALLBACK = new FMOD.CB_3D_ROLLOFFCALLBACK(rolloff_callback);


        public static bool Init()
        {
            FMOD.RESULT result;

            CONTROLS[(int)CONTROLTYPE.DIRECT] = new ControlInfo(new PointF(0.0f, 1.0f), new PointF(0.5f, 0.0f), new PointF(1.0f, 0.6f), new PointF(-0.5f, 0.0f));
            CONTROLS[(int)CONTROLTYPE.REVERB] = new ControlInfo(new PointF(0.0f, 0.3f), new PointF(0.5f, 0.0f), new PointF(1.0f, 0.0f), new PointF(-0.5f, 0.0f));
            CONTROLS[(int)CONTROLTYPE.LOWPASS] = new ControlInfo(new PointF(0.0f, 1.0f), new PointF(0.5f, 0.0f), new PointF(1.0f, 0.0f), new PointF(-0.5f, 0.0f));
            CONTROLS[(int)CONTROLTYPE.ROLLOFF] = new ControlInfo(new PointF(0.0f, 1.0f), new PointF(0.5f, 0.0f), new PointF(1.0f, 0.7f), new PointF(-0.5f, 0.0f));
            CONTROLS[(int)CONTROLTYPE.PANLEVEL] = new ControlInfo(new PointF(0.0f, 0.0f), new PointF(0.5f, 0.0f), new PointF(0.7f, 1.0f), new PointF(-0.5f, 0.0f));

            result = FMOD.Factory.System_Create(ref system);
            ERRCHECK(result);

            uint version = 0;
            result = system.getVersion(ref version);
            ERRCHECK(result);
            if (version < FMOD.VERSION.number)
            {
                MessageBox.Show("Error!  You are using an old version of FMOD " + version.ToString("X") + ".  This program requires " + FMOD.VERSION.number.ToString("X") + ".");
                Application.Exit();
            }

            FMOD.CAPS caps = FMOD.CAPS.NONE;
            FMOD.SPEAKERMODE speakermode = FMOD.SPEAKERMODE.STEREO;

            int minfrequency = 0, maxfrequency = 0;
            StringBuilder name = new StringBuilder(128);

            result = system.getDriverCaps(0, ref caps, ref minfrequency, ref maxfrequency, ref speakermode);
            ERRCHECK(result);

            result = system.setSpeakerMode(speakermode);
            ERRCHECK(result);

            if ((caps & FMOD.CAPS.HARDWARE_EMULATED) == FMOD.CAPS.HARDWARE_EMULATED)
            {
                result = system.setDSPBufferSize(1024, 10);
                ERRCHECK(result);
            }

            FMOD.GUID guid = new FMOD.GUID();

            result = system.getDriverInfo(0, name, 256, ref guid);
            ERRCHECK(result);

            if (name.ToString().IndexOf("SigmaTel") != -1)
            {
                result = system.setSoftwareFormat(48000, FMOD.SOUND_FORMAT.PCMFLOAT, 0, 0, FMOD.DSP_RESAMPLER.LINEAR);
                ERRCHECK(result);
            }

            result = system.init(32, FMOD.INITFLAGS.NORMAL | FMOD.INITFLAGS._3D_RIGHTHANDED, (IntPtr)null);
            if (result == FMOD.RESULT.ERR_OUTPUT_CREATEBUFFER)
            {
                result = system.setSpeakerMode(FMOD.SPEAKERMODE.STEREO);
                ERRCHECK(result);

                result = system.init(32, FMOD.INITFLAGS.NORMAL | FMOD.INITFLAGS._3D_RIGHTHANDED, (IntPtr)null);
                ERRCHECK(result);
            }

            result = system.set3DSettings(2.0f, 100.0f, 1.0f);
            ERRCHECK(result);

            result = system.set3DRolloffCallback(ROLLOFFCALLBACK);
            ERRCHECK(result);

            FMOD.VECTOR pos; pos.x = 0.0f; pos.y = 0.0f; pos.z = 0.0f;
            FMOD.VECTOR vel; vel.x = 0.0f; vel.y = 0.0f; vel.z = 0.0f;
            FMOD.VECTOR dir; dir.x = 0.0f; dir.y = 1.0f; dir.z = 0.0f;
            FMOD.VECTOR up; up.x = 0.0f; up.y = 0.0f; up.z = 1.0f;
            result = system.get3DListenerAttributes(0, ref pos, ref vel, ref dir, ref up);
            ERRCHECK(result);

            FMOD.PRESET preset = new FMOD.PRESET();
            REVERB[(int)REVERBTYPE.OFF] = preset.OFF();
            REVERB[(int)REVERBTYPE.GENERIC] = preset.GENERIC();
            REVERB[(int)REVERBTYPE.PADDEDCELL] = preset.PADDEDCELL();
            REVERB[(int)REVERBTYPE.ROOM] = preset.ROOM();
            REVERB[(int)REVERBTYPE.BATHROOM] = preset.BATHROOM();
            REVERB[(int)REVERBTYPE.LIVINGROOM] = preset.LIVINGROOM();
            REVERB[(int)REVERBTYPE.STONEROOM] = preset.STONEROOM();
            REVERB[(int)REVERBTYPE.AUDITORIUM] = preset.AUDITORIUM();
            REVERB[(int)REVERBTYPE.CONCERTHALL] = preset.CONCERTHALL();
            REVERB[(int)REVERBTYPE.CAVE] = preset.CAVE();
            REVERB[(int)REVERBTYPE.ARENA] = preset.ARENA();
            REVERB[(int)REVERBTYPE.HANGAR] = preset.HANGAR();
            REVERB[(int)REVERBTYPE.CARPETTEDHALLWAY] = preset.CARPETTEDHALLWAY();
            REVERB[(int)REVERBTYPE.HALLWAY] = preset.HALLWAY();
            REVERB[(int)REVERBTYPE.STONECORRIDOR] = preset.STONECORRIDOR();
            REVERB[(int)REVERBTYPE.ALLEY] = preset.ALLEY();
            REVERB[(int)REVERBTYPE.FOREST] = preset.FOREST();
            REVERB[(int)REVERBTYPE.CITY] = preset.CITY();
            REVERB[(int)REVERBTYPE.MOUNTAINS] = preset.MOUNTAINS();
            REVERB[(int)REVERBTYPE.QUARRY] = preset.QUARRY();
            REVERB[(int)REVERBTYPE.PLAIN] = preset.PLAIN();
            REVERB[(int)REVERBTYPE.PARKINGLOT] = preset.PARKINGLOT();
            REVERB[(int)REVERBTYPE.SEWERPIPE] = preset.SEWERPIPE();
            REVERB[(int)REVERBTYPE.UNDERWATER] = preset.UNDERWATER();
            SetReverb(REVERBTYPE.OFF);

            return true;
        }
        public static bool Play(string strRootPath, SoundItemProperty prop)
        {
            FMOD.RESULT result;

            if (channel != null)
                channel.stop();

            string name = strRootPath + prop.FileName;

            FMOD.MODE mode = FMOD.MODE.DEFAULT;
            if (prop.Type == SoundItemProperty.typeSound[0])
                mode |= FMOD.MODE._2D | FMOD.MODE.CREATESAMPLE | FMOD.MODE.SOFTWARE;
            else if (prop.Type == SoundItemProperty.typeSound[1])
                mode |= FMOD.MODE._3D | FMOD.MODE.CREATESAMPLE | FMOD.MODE.SOFTWARE | FMOD.MODE._3D_LINEARROLLOFF | FMOD.MODE._3D_WORLDRELATIVE | FMOD.MODE._3D_IGNOREGEOMETRY;
            else if (prop.Type == SoundItemProperty.typeSound[2])
                mode |= FMOD.MODE._2D | FMOD.MODE.CREATESTREAM | FMOD.MODE.SOFTWARE;

            if (prop.Loop == true)
                mode |= FMOD.MODE.LOOP_NORMAL;

            result = system.createSound(name, mode, ref sound);
            ERRCHECK(result);

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound, true, ref channel);
            ERRCHECK(result);

            if (prop.Type == SoundItemProperty.typeSound[1])
            {
                channel.set3DMinMaxDistance(100.0f, prop.MaxDistance);

                FMOD.VECTOR pos = new FMOD.VECTOR();
                pos.x = 0.0f; pos.y = 0.0f; pos.z = 0.0f;
                FMOD.VECTOR vel = new FMOD.VECTOR();
                vel.x = 0.0f; vel.y = 0.0f; vel.z = 0.0f;
                channel.set3DAttributes(ref pos, ref vel);

                FMOD.DSP dsp = null;
                result = system.createDSPByType(FMOD.DSP_TYPE.LOWPASS_SIMPLE, ref dsp);
                ERRCHECK(result);

                dsp.setParameter((int)FMOD.DSP_LOWPASS_SIMPLE.CUTOFF, 22000.0f);
                FMOD.DSPConnection dspcon = null;
                channel.addDSP(dsp, ref dspcon);
            }

            channel.setVolume((float)prop.Volume * 0.01f);
            channel.setCallback(CHANNELCALLBACK);
            channel.setPaused(false);

            return true;
        }
        public static void Stop()
        {
            if (channel != null)
                channel.stop();
        }
        public static void Update()
        {
            if (system == null)
                return;

            // Update sound
            if (channel != null)
            {
                FMOD.MODE mode = FMOD.MODE.DEFAULT;
                channel.getMode(ref mode);

                if ((mode & FMOD.MODE._3D) == FMOD.MODE._3D)
                {
                    FMOD.VECTOR pos; pos.x = 0.0f; pos.y = 0.0f; pos.z = 0.0f;
                    FMOD.VECTOR vel; vel.x = 0.0f; vel.y = 0.0f; vel.z = 0.0f;
                    channel.get3DAttributes(ref pos, ref vel);

                    float distance = (float)Math.Sqrt((double)(pos.x * pos.x + pos.y * pos.y));

                    float minDist = 0.0f, maxDist = 1000.0f;
                    channel.get3DMinMaxDistance(ref minDist, ref maxDist);

                    float distRatio = distance / maxDist;

                    if (ignorefod == false)
                    {
                        // Set direct and reverb occlusion
                        float direct = 1.0f - GetDistanceRatioValue(CONTROLTYPE.DIRECT, distRatio);
                        float reverb = 1.0f - GetDistanceRatioValue(CONTROLTYPE.REVERB, distRatio);
                        channel.set3DOcclusion(direct, reverb);

                        // Set lowpass
                        FMOD.DSP head = null;
                        channel.getDSPHead(ref head);
                        if (head != null)
                        {
                            int inputs = 0;
                            head.getNumInputs(ref inputs);
                            for ( int i = 0; i < inputs; i++)
                            {
                                FMOD.DSP dsp = null;
                                FMOD.DSPConnection dspcon = null;
                                head.getInput(i, ref dsp, ref dspcon);
                                if (dsp == null)
                                    continue;

                                FMOD.DSP_TYPE type = FMOD.DSP_TYPE.UNKNOWN;
                                dsp.getType(ref type);
                                if (type == FMOD.DSP_TYPE.LOWPASS_SIMPLE)
                                {
                                    float hrtf = 1.0f;

                                    FMOD.VECTOR dir;
                                    dir.x = pos.x;
                                    dir.y = pos.y;
                                    dir.z = 0.0f;

                                    float length = (float)Math.Sqrt((double)(dir.x * dir.x + dir.y * dir.y));
                                    if (length > 0.0001f)
                                    {
                                        length = 1.0f / length;
                                        dir.x *= length;
                                        dir.y *= length;
                                        float _angle = (float)Math.Acos( (double)dir.y);
                                        hrtf = 1.0f - HRTFDATA.Scale * Math.Min(1.0f, Math.Max(0.0f, (_angle - HRTFDATA.MinAngle) / (HRTFDATA.MaxAngle - HRTFDATA.MinAngle)));
                                    }

                                    float lowpass = 21990.0f * GetDistanceRatioValue(CONTROLTYPE.LOWPASS, distRatio) * hrtf + 10.0f;
                                    dsp.setParameter((int)FMOD.DSP_LOWPASS_SIMPLE.CUTOFF, lowpass);

                                    break;
                                }
                            }
                        }

                        // Set pan level
                        float panlevel = GetDistanceRatioValue(CONTROLTYPE.PANLEVEL, distRatio);
                        channel.set3DPanLevel(panlevel);
                    }

                    else
                    {
                        channel.set3DPanLevel(0.0f);
                    }
                }
            }

            system.update();
        }
        public static float GetDistanceRatioValue(CONTROLTYPE type, float ratio)
        {
            float retVal = 0.0f;

            int control = (int)type;
            if (ratio < CONTROLS[control].Start.X)
            {
                retVal = (CONTROLS[control].StartVel.Y / CONTROLS[control].StartVel.X) * (ratio - CONTROLS[control].Start.X) + CONTROLS[control].Start.Y;
            }
            else if (ratio > CONTROLS[control].End.X)
            {
                retVal = (CONTROLS[control].EndVel.Y / CONTROLS[control].EndVel.X) * (ratio - CONTROLS[control].End.X) + CONTROLS[control].End.Y;
            }
            else
            {
                float _ratio = (ratio - CONTROLS[control].Start.X) / (CONTROLS[control].End.X - CONTROLS[control].Start.X);
                float inv_ratio = 1.0f - _ratio;
                float a = inv_ratio * inv_ratio * inv_ratio;
                float b = 3.0f * _ratio * (inv_ratio * inv_ratio);
                float c = 3.0f * (_ratio * _ratio) * inv_ratio;
                float d = _ratio * _ratio * _ratio;
                retVal = a * CONTROLS[control].Start.Y + b * (CONTROLS[control].Start.Y + CONTROLS[control].StartVel.Y) + c * (CONTROLS[control].End.Y + CONTROLS[control].EndVel.Y) + d * CONTROLS[control].End.Y;
            }

            retVal = (float)Math.Min( 1.0f, Math.Max(0.0f, (double)retVal));
            return retVal;
        }
        public static bool IsPlaying()
        {
            return (channel == null ? false : true);
        }
        public static uint GetPlayTime()
        {
            if (sound == null)
                return 0;

            uint length = 0;
            sound.getLength(ref length, FMOD.TIMEUNIT.MS);

            return length;
        }
        public static uint GetPlayPosition()
        {
            if (channel == null)
                return 0;

            uint pos = 0;
            channel.getPosition(ref pos, FMOD.TIMEUNIT.MS);
            return pos;
        }
        public static void SetPlayPosition(uint position)
        {
            if (channel == null)
                return;

            channel.setPosition(position, FMOD.TIMEUNIT.MS);
        }
        public static void SetSoundPosition(float x, float y)
        {
            if (channel == null)
                return;

            FMOD.VECTOR pos; pos.x = x; pos.y = y; pos.z = 0.0f;
            FMOD.VECTOR vel; vel.x = 0.0f; vel.y = 0.0f; vel.z = 0.0f;
            channel.set3DAttributes(ref pos, ref vel);
        }
        public static REVERBTYPE GetReverb()
        {
            return reverb;
        }
        public static void SetReverb(REVERBTYPE type)
        {
            if (system == null)
                return;

            system.setReverbAmbientProperties(ref REVERB[(int)type]);
            reverb = type;
        }
        public static void GetreverbProp(REVERBTYPE type, ref FMOD.REVERB_PROPERTIES prop)
        {
            prop.AirAbsorptionHF = REVERB[(int)type].AirAbsorptionHF;
            prop.DecayHFRatio = REVERB[(int)type].DecayHFRatio;
            prop.DecayLFRatio = REVERB[(int)type].DecayLFRatio;
            prop.DecayTime = REVERB[(int)type].DecayTime;
            prop.Density = REVERB[(int)type].Density;
            prop.Diffusion = REVERB[(int)type].Diffusion;
            prop.EchoDepth = REVERB[(int)type].EchoDepth;
            prop.EchoTime = REVERB[(int)type].EchoTime;
            prop.EnvDiffusion = REVERB[(int)type].EnvDiffusion;
            prop.Environment = REVERB[(int)type].Environment;
            prop.EnvSize = REVERB[(int)type].EnvSize;
            prop.Flags = REVERB[(int)type].Flags;
            prop.HFReference = REVERB[(int)type].HFReference;
            prop.Instance = REVERB[(int)type].Instance;
            prop.LFReference = REVERB[(int)type].LFReference;
            prop.ModulationDepth = REVERB[(int)type].ModulationDepth;
            prop.ModulationTime = REVERB[(int)type].ModulationTime;
            prop.Reflections = REVERB[(int)type].Reflections;
            prop.ReflectionsDelay = REVERB[(int)type].ReflectionsDelay;
            prop.ReflectionsPan = REVERB[(int)type].ReflectionsPan;
            prop.Reverb = REVERB[(int)type].Reverb;
            prop.ReverbDelay = REVERB[(int)type].ReverbDelay;
            prop.ReverbPan = REVERB[(int)type].ReverbPan;
            prop.Room = REVERB[(int)type].Room;
            prop.RoomHF = REVERB[(int)type].RoomHF;
            prop.RoomLF = REVERB[(int)type].RoomLF;
            prop.RoomRolloffFactor = REVERB[(int)type].RoomRolloffFactor;
        }
        public static void SetReverbProp(REVERBTYPE type, FMOD.REVERB_PROPERTIES prop)
        {
            REVERB[(int)type].AirAbsorptionHF = prop.AirAbsorptionHF;
            REVERB[(int)type].DecayHFRatio = prop.DecayHFRatio;
            REVERB[(int)type].DecayLFRatio = prop.DecayLFRatio;
            REVERB[(int)type].DecayTime = prop.DecayTime;
            REVERB[(int)type].Density = prop.Density;
            REVERB[(int)type].Diffusion = prop.Diffusion;
            REVERB[(int)type].EchoDepth = prop.EchoDepth;
            REVERB[(int)type].EchoTime = prop.EchoTime;
            REVERB[(int)type].EnvDiffusion = prop.EnvDiffusion;
            REVERB[(int)type].Environment = prop.Environment;
            REVERB[(int)type].EnvSize = prop.EnvSize;
            REVERB[(int)type].Flags = prop.Flags;
            REVERB[(int)type].HFReference = prop.HFReference;
            REVERB[(int)type].Instance = prop.Instance;
            REVERB[(int)type].LFReference = prop.LFReference;
            REVERB[(int)type].ModulationDepth = prop.ModulationDepth;
            REVERB[(int)type].ModulationTime = prop.ModulationTime;
            REVERB[(int)type].Reflections = prop.Reflections;
            REVERB[(int)type].ReflectionsDelay = prop.ReflectionsDelay;
            REVERB[(int)type].ReflectionsPan = prop.ReflectionsPan;
            REVERB[(int)type].Reverb = prop.Reverb;
            REVERB[(int)type].ReverbDelay = prop.ReverbDelay;
            REVERB[(int)type].ReverbPan = prop.ReverbPan;
            REVERB[(int)type].Room = prop.Room;
            REVERB[(int)type].RoomHF = prop.RoomHF;
            REVERB[(int)type].RoomLF = prop.RoomLF;
            REVERB[(int)type].RoomRolloffFactor = prop.RoomRolloffFactor;
        }

        public static FMOD.RESULT channel_callback(IntPtr channelraw, FMOD.CHANNEL_CALLBACKTYPE type, IntPtr commanddata1, IntPtr commanddata2)
        {
            if (type == FMOD.CHANNEL_CALLBACKTYPE.END)
            {
                if (channel.getRaw() == channelraw)
                {
                    sound.release();
                    channel = null;
                }
            }
            return 0;
        }
        public static float rolloff_callback(IntPtr channelraw, float distance)
        {
            float rolloff = 1.0f;
            if (channel.getRaw() == channelraw)
            {
                float minDist = 0.0f, maxDist = 1000.0f;
                channel.get3DMinMaxDistance(ref minDist, ref maxDist);
                float distRatio = distance / maxDist; 
                
                rolloff = GetDistanceRatioValue(CONTROLTYPE.ROLLOFF, distRatio);
            }

            return rolloff;
        }

        private static void ERRCHECK(FMOD.RESULT result)
        {
            if (result != FMOD.RESULT.OK)
            {
                MessageBox.Show("FMOD error! " + result + " - " + FMOD.Error.String(result));
                Environment.Exit(-1);
            }
        }
    }
}
