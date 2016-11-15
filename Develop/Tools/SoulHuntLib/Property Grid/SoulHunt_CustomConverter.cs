using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;

// SHCT_는 SoulHunt Custom Type

namespace SoulHunt
{
    #region SHCT_BoolConverter
    /// <summary>
    /// Raiderz에서 True, False등을 마구잡이로 사용하기 때문에 이것을 쓴다. -_-
    /// </summary>
    public class SHCT_BoolConverter : StringConverter
    {
        public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
        {
            return true;
        }

        public override StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
        {
            return new StandardValuesCollection(new String[] { "TRUE", "FALSE" });
        }

        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            if (sourceType == typeof(String)) return true; else return false;
        }

        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            if (destinationType == typeof(String)) return true; else return false;
        }

        // 에디터에서 보이는 것을 내부에서 사용하는 데이터로 돌리고 싶을때 사용한다.
        public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
        {
            if (value != null && value.GetType() == typeof(String))
            {
                String val = (String)value;
                if (val.ToUpper() == "TRUE") return "True"; else return "False";
            }

            return base.ConvertFrom(context, culture, value);
        }

        // 내부에서 사용하는 데이터를 에디터로 옮길때 사용한다.
        public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
        {
            if (value != null && value.GetType() == typeof(String)) if (((String)value).ToUpper() == "TRUE") return "True"; else return "False";
            return base.ConvertTo(context, culture, value, destinationType);
        }

        public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
        {
            return true;
        }
    }
    #endregion

    #region SHCT_TalentConverter
    /// <summary>
    /// 탤런트 정보를 위한 컨버터. INT32형태와 호환한다.
    /// </summary>
    public class SHCT_TalentConverter : StringConverter
    {
        public const String INVALID = "(없음)";

        public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
        {
            return true;
        }

        public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
        {
            return true;
        }

        public override StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
        {
            List<String> _TalentNameList = new List<string>();

            _TalentNameList.Add(SHCT_TalentConverter.INVALID);
            foreach (SHTalent talent in SHXmlCore.CurrentCore.Talents.dataList) _TalentNameList.Add(talent.id + ": " + SHXmlCore.CurrentCore.GetSafeString(talent.name));
            return new StandardValuesCollection(_TalentNameList.ToArray());
        }

        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            if (sourceType == typeof(String)) return true;
            return base.CanConvertFrom(context, sourceType);
        }

        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            if (destinationType == typeof(String)) return true; else return false;
        }

        public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
        {
            if (value.GetType() == typeof(String))
            {
                String val = (String)value;
                int ret = 0;

                if (SHCT_TalentConverter.INVALID == val) return 0;

                String[] tmp = val.Split(new Char[] { ':' });
                if (tmp != null && Int32.TryParse(tmp[0], out ret)) return ret; else return 0;
            }

            return base.ConvertFrom(context, culture, value);
        }

        public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
        {
            if (value.GetType() == typeof(Int32))
            {
                Int32 val = (Int32)value;
                if (val == 0) return SHCT_TalentConverter.INVALID;
                if (SHXmlCore.CurrentCore.Talents.IsValid(val)) return SHXmlCore.CurrentCore.GetSafeString(SHXmlCore.CurrentCore.Talents[val].name); else val.ToString();
            }

            return base.ConvertTo(context, culture, value, destinationType);
        }
    }
    #endregion
        
    #region SHCT_CondType
    /// <summary>
    /// 조건문을 위한 타입
    /// </summary>
    public class SHCT_CondType : EnumConverter
    {
        public SHCT_CondType()
            : base(typeof(SHCondition.ConditionType))
        {
        }

        public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
        {
            return true;
        }

        public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
        {
            return true;
        }

        public override StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
        {
            List<String> Values = new List<string>();
            
            foreach (SHCondition.SHCTString tmp in SHCondition.TypeNaming) Values.Add(tmp.Text);            
            return new StandardValuesCollection(Values.ToArray());
        }

        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            return (sourceType == typeof(String));
        }

        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            return (destinationType == typeof(String) || destinationType == typeof(SHCondition.ConditionType));
        }

        // String -> Enum
        public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
        {

            if (value.GetType() == typeof(String))
            {
                String val = (String)value;
                foreach (SHCondition.SHCTString tmp in SHCondition.TypeNaming) if (val == tmp.Text) return tmp.Type;
                return null;
            }
            return base.ConvertFrom(context, culture, value);
        }

        // Enum -> String
        public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
        {
            if (destinationType == typeof(String) && value.GetType() == typeof(SHCondition.ConditionType))
            {
                foreach (SHCondition.SHCTString tmp in SHCondition.TypeNaming) if ((int)value == (int)tmp.Type) return tmp.Text;
                return String.Empty;
            }
            else if (destinationType == typeof(String) && value.GetType() == typeof(String))
            {
                return value;
            }

            return base.ConvertTo(context, culture, value, destinationType);
        }
    }
    #endregion
}