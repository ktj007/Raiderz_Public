#region Using문

using System;
using System.ComponentModel;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;

#endregion

namespace SoulHunt
{
    [Serializable, DefaultProperty("fieldbgm")]
    public sealed class SHBGM //: SHDataObject
    {
        [XmlAttribute]
        public string fieldbgm;
    }

    [Serializable, DefaultProperty("set")]
    public sealed class SHEnv //: SHDataObject
    {
        [XmlAttribute]
        public string set;

        public SHWeather weather;
    }

    [Serializable, DefaultProperty("default")]
    public sealed class SHWeather //: SHDataObject
    {
        [XmlAttribute]
        public string Default;

        [XmlElement("ADD")]
        public ArrayList dataList;
    }

    [Serializable, DefaultProperty("id")]
    public sealed class SHSpawnNPC //: SHDataObject
    {
        [XmlAttribute]
        public int id;
        [XmlAttribute]
        public bool Default;
        [XmlAttribute]
        public int rate;
    }

    [Serializable, DefaultProperty("id")]
    public sealed class SHSpawn //: SHDataObject
    {
        [XmlAttribute]
        public int id;
        [XmlAttribute]
        public int group;
        [XmlAttribute]
        public string name;
        [XmlAttribute]
        public float x;
        [XmlAttribute]
        public float y;
        [XmlAttribute]
        public float z;
        [XmlAttribute]
        public float dir_x;
        [XmlAttribute]
        public float dir_y;
        [XmlAttribute]
        public float dir_z;
        [XmlAttribute]
        public float radius;
        [XmlAttribute]
        public bool random_dir;
        [XmlAttribute]
        public bool enabled;
        [XmlAttribute]
        public int start_time;
        [XmlAttribute]
        public int respawn_time;
        [XmlAttribute]
        public int roam_radius;
        [XmlAttribute]
        public int roam_depth;
        [XmlAttribute]
        public string color;

        [XmlElement(Type = typeof(SHSpawnNPC), ElementName = "NPC")]
        public ArrayList dataList;

        public SHSpawn() : base()
        {
            dataList = new ArrayList();
        }
    }

    #region SHFieldInfo
    [Serializable, DefaultProperty("zone")]
    public sealed class SHFieldInfo : SHDataObject
    {
        [XmlAttribute]
        public int zone;
        [XmlAttribute]
        public int min_x;
        [XmlAttribute]
        public int min_y;
        [XmlAttribute]
        public int max_x;
        [XmlAttribute]
        public int max_y;
        [XmlAttribute]
        public int boundary_min_x;
        [XmlAttribute]
        public int boundary_min_y;
        [XmlAttribute]
        public int boundary_max_x;
        [XmlAttribute]
        public int boundary_max_y;
        [XmlAttribute]
        public bool weather_effect;
        [XmlAttribute]
        public bool town;
        [XmlAttribute]
        public int max_gather_mine;

        [XmlElement("BGM")]
        public SHBGM bgm;

        [XmlElement("ENV")]
        public SHEnv env;

        [XmlElement(Type = typeof(SHSpawn), ElementName = "SPAWN")]
        public ArrayList spawnList;

        [XmlElement("MARKER")]
        public ArrayList markerList;

        [XmlElement("SENSOR")]
        public ArrayList sensorList;

        public SHFieldInfo() : base()
        {
            spawnList = new ArrayList();
            markerList = new ArrayList();
            sensorList = new ArrayList();
        }

        public override bool Compile()
        {
            _bPassed = true;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }
    #endregion

    [Serializable]
    [XmlRoot("maiet")]
    public sealed class SHFieldXml : SHDataObject
    {
        [XmlElement("FIELDINFO")]
        public SHFieldInfo fieldInfo;

        public override bool Compile()
        {
            _bPassed = true;

            return _bPassed;
        }

        public override bool Build(SHXmlCore e)
        {
            if (!_bPassed) return false;

            return _bPassed;
        }

        public override string ToBody(SHXmlCore core)
        {
            throw new NotImplementedException();
        }
    }
}
