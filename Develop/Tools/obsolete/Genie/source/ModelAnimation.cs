using System;

namespace Genie
{
	public enum AnimationLoopType
	{
		ALT_LOOP,
		ALT_LASTFRAME
	};

	public class ModelAnimation
	{
		private string				szName;

		public WeaponType			nWeaponType;
		public string				szFileName;
		public AnimationLoopType	nLoopType;

		public ModelAnimation()
		{
			nWeaponType = WeaponType.WT_NONE;
			szName="";
			szFileName="";
			nLoopType=AnimationLoopType.ALT_LOOP;
		}

		public void InitFromXml(string szXmlName, string szXmlFileName, string szXmlLoop)
		{
			string szWeapon = szXmlName.Substring(0, 3).ToLower();

			this.nWeaponType = WeaponType.WT_NONE;
			for (WeaponType weapon_type=WeaponType.WT_1H_SLASH; weapon_type<=WeaponType.WT_GUNPOWDER; weapon_type++)
			{
				if (szWeapon == ConvertString.GetWeaponTypeStr(weapon_type)) 
				{
					this.nWeaponType = weapon_type;
					break;
				}
			}
			
			this.szName = szXmlName;

			this.szFileName = szXmlFileName;
			if (szXmlLoop.ToLower() == "loop")
				this.nLoopType = AnimationLoopType.ALT_LOOP;
			else this.nLoopType = AnimationLoopType.ALT_LASTFRAME;
		}

		public void Set(WeaponType nWeaponType, string szName, string szFileName, AnimationLoopType nLoopType)
		{
			this.nWeaponType = nWeaponType;
			this.szFileName = szFileName;
			this.nLoopType = nLoopType;

			string header="";
			if (nWeaponType != WeaponType.WT_NONE)
			{
				header = ConvertString.GetWeaponTypeStr(nWeaponType) + "_";
			}

			this.szName = header + szName;
		}

		public string Name
		{
			get
			{
				string ret="";
				if (this.nWeaponType == WeaponType.WT_NONE) ret = this.szName;
				else
				{
					ret = szName.Substring(4, szName.Length-4);
				}
				return ret;
			}
		}
		public string RawName
		{
			get
			{
				return szName;
			}
		}
	};
}
