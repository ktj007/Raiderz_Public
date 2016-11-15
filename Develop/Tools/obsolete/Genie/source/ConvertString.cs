using System;

namespace Genie
{
	/// <summary>
	/// ConvertString에 대한 요약 설명입니다.
	/// </summary>
	public class ConvertString
	{
		static public string GetModelCategoryStr(ModelCategory nModelCategory)
		{
			switch (nModelCategory)
			{
				case ModelCategory.MC_PLAYER: return "Player";
				case ModelCategory.MC_MONSTER: return "Monster";
				case ModelCategory.MC_NPC: return "NPC";
				case ModelCategory.MC_WEAPON: return "Weapon";
				case ModelCategory.MC_SFX: return "SFX";
				case ModelCategory.MC_MAP_OBJECT: return "MapObject";
			}

			return "";
		}
		static public string GetWeaponTypeStr(WeaponType nType)
		{
			switch (nType)
			{
				case WeaponType.WT_1H_SLASH: return "1hs";
				case WeaponType.WT_2H_SLASH: return "2hs";
				case WeaponType.WT_1H_BLUNT: return "1hb";
				case WeaponType.WT_2H_BLUNT: return "2hb";
				case WeaponType.WT_1H_PIERCE: return "1hp";
				case WeaponType.WT_2H_PIERCE: return "2hp";
				case WeaponType.WT_ARCHERY: return "arc";
				case WeaponType.WT_GUNPOWDER: return "gun";
			}

			return "none";
		}

		static public string GetAnimationLoopTypeStr(AnimationLoopType nLoopType)
		{
			switch (nLoopType)
			{
				case AnimationLoopType.ALT_LOOP: return "loop";
				case AnimationLoopType.ALT_LASTFRAME: return "lastframe";
			}

			return "";
		}

		static public string GetRawAnimationName(WeaponType nWeaponType, string szAniName)
		{
			string szRawName = "";
			if (nWeaponType == WeaponType.WT_NONE) szRawName = szAniName;
			else
			{
				szRawName = ConvertString.GetWeaponTypeStr(nWeaponType) + "_" + szAniName;
			}

			return szRawName;
		}
	}
}
