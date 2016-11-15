/*
 * SHLocale.cs
 * Locale 관련 클래스들
 */

using System;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using cmlNET;

namespace SoulHunt
{
    public enum SHLocaleType
    {
        KO_KR,
        EN_US,
        DE_DE
    }

    public class SHLocale
    {
        private SHLocaleType _Type = SHLocaleType.EN_US;

        private void setType(SHLocaleType type)
        {
            _Type = type;
        }

        public SHLocaleType Type { get { return _Type; } set { setType(value); } }
        public string Path()
        {
            return Path(_Type);
        }

        public string Path(SHLocaleType type)
        {
            switch (_Type)
            {
                case SHLocaleType.KO_KR: return "\\lang\\ko_KR\\";
                case SHLocaleType.EN_US: return "\\lang\\en_US\\";
                case SHLocaleType.DE_DE: return "\\lang\\de_DE\\";
            }

            return "\\lang\\en_US\\";
        }
    }
}