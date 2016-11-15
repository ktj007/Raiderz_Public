using System;
using System.IO;
using System.Windows.Forms;
using System.Text;
using System.Collections;
using System.Collections.Generic;

namespace SoulHunt
{
    /// <summary>
    /// SHXmlCore
    /// SoulHunt에서 사용하는 XML파일을 전부 읽고 쓰는 코어엔진. 아울러 각 XML의 파싱을 돕기 위한 상호참조를 위한 테이블을 생성시킨다.
    /// </summary>
    public sealed class SHXmlCore
    {
        #region 데이터멤버

        public SHNPCsXml NPCs;
        public SHConditionsXml Conditions;
        public SHConditionsXml ItemConditions;
        public SHItemsXml Items;
        public SHStringXml Strings;
        public SHQuestsXml Quests;
        public SHDialogsXml Dialogs;
        public SHTalentsXml Talents;
        public SHLootingsXml Lootings;
        public SHCraftXml Craft;
        public SHNPCShopXml NPCShop;
        public SHRecipeXml Recipe;
        public SHFieldListXml FieldList;

        public static SHXmlCore CurrentCore;

        public struct SHLocaleXml
        {
            public SHStringXml NPCNameString;
            public SHStringXml ItemNameString;
            public SHStringXml TalentNameString;

            public SHStringXml QuestString;
            public SHStringXml DialogString;
        };

        public SHLocaleXml    LocalXml;


        #endregion

        #region 생성자
        
        public SHXmlCore()
        {
            CurrentCore = this;
            Refresh();
        }

        #endregion

        #region 새로고침

        public void Refresh()
        {

            LoadStrings();
            LoadLocaleStrings();

            // String 이외의 로딩 순서는 뭐래도 상관없다.
            LoadTalents();
            LoadNPCs();
            LoadDialogs();
            LoadConditions();
            LoadItemConditions();
            LoadLootings();
            LoadItems();
            LoadQuests();

            LoadCraft();
            LoadNPCShop();
            LoadRecipe();
            LoadFieldList();

            // 빌드는 컴파일단계까지만 참조하도록한다.
            // 느리더라도 이것이 안전하다.

            Strings.Build(this);
            Talents.Build(this);
            ItemConditions.Build(this);
            Items.Build(this);
            Conditions.Build(this);
            Lootings.Build(this);
            NPCs.Build(this);
            Dialogs.Build(this);
            Quests.Build(this);

            Craft.Build(this);
            NPCShop.Build(this);
            Recipe.Build(this);
            FieldList.Build(this);

            
        }

        #endregion

        #region 데이터 로딩 메소드들

        public SHColtXml LoadColt(string path)
        {
            return SHGlobal.Deserialize<SHColtXml>(path, Encoding.UTF8);
        }

        public bool LoadLootings()
        {
            Lootings = SHGlobal.Deserialize<SHLootingsXml>(SHGlobal.GetServerSystemFolder() + "\\loot.xml", Encoding.UTF8);
            if (Lootings == null)
            {
                Lootings = new SHLootingsXml();
                Lootings.LoadingFail();
            }
            return Lootings.Compile();
        }

        public bool LoadTalents()
        {
            Talents = SHGlobal.Deserialize<SHTalentsXml>(SHGlobal.GetServerSystemFolder() + "\\talent.xml", Encoding.UTF8);
            return Talents.Compile();
        }

        public bool LoadNPCs()
        {
            NPCs = SHGlobal.Deserialize<SHNPCsXml>(SHGlobal.GetServerSystemFolder() + "\\npc.xml", Encoding.UTF8);
            if (NPCs == null)
            {
                NPCs = new SHNPCsXml();
                NPCs.LoadingFail();
            }

            foreach (SHNPC npc in NPCs.dataList)
            {
                string szKey = "NPC_NAME_" + npc.id.ToString();

                if (LocalXml.NPCNameString.IsValid(szKey))
                {
                    string szValue = LocalXml.NPCNameString[szKey].ToString();
                    npc.Name = szValue;
                }
            }
            SHGlobal.Serialize<SHNPCsXml>(SHGlobal.GetServerSystemFolder() + "\\npc_translated.xml", NPCs, Encoding.UTF8);
            SHGlobal.Serialize<SHNPCsXml>(SHGlobal.GetClientSystemFolder() + "\\npc_translated.xml", NPCs, Encoding.UTF8);
            return NPCs.Compile();
        }

        public void LoadConditions()
        {
            Conditions = SHGlobal.Deserialize<SHConditionsXml>(SHGlobal.GetServerSystemFolder() + "\\conditions.xml", Encoding.UTF8);
            if (Conditions == null)
            {
                Conditions = new SHConditionsXml();
                Conditions.LoadingFail();
            }
            Conditions.Compile();
        }

        public void LoadItemConditions()
        {
            ItemConditions = SHGlobal.Deserialize<SHConditionsXml>(SHGlobal.GetServerSystemFolder() + "\\conditions_item.xml", Encoding.UTF8);
            if (ItemConditions == null)
            {
                ItemConditions = new SHConditionsXml();
                ItemConditions.LoadingFail();
            }
            ItemConditions.Compile();
        }

        public void LoadItems()
        {
            Items = SHGlobal.Deserialize<SHItemsXml>(SHGlobal.GetServerSystemFolder() + "\\xitem.xml", Encoding.UTF8);
            if (Items == null)
            {
                Items = new SHItemsXml();
                Items.LoadingFail();
            }

            foreach (SHItem item in Items.dataList)
            {
                string szKey = "ITEM_NAME_" + item.id.ToString();

                if (LocalXml.ItemNameString.IsValid(szKey))
                {
                    string szValue = LocalXml.ItemNameString[szKey].ToString();
                    item.name = szValue;
                }

                szKey = "ITEM_DESC_" + item.id.ToString();

                if (LocalXml.ItemNameString.IsValid(szKey))
                {
                    string szValue = LocalXml.ItemNameString[szKey].ToString();
                    item.desc = szValue;
                }
            }

            Items.Compile();
        }

        public void LoadStrings()
        {
            Strings = SHGlobal.Deserialize<SHStringXml>(SHGlobal.GetClientSystemFolder() + "\\lang\\ko_KR\\strings.xml", Encoding.UTF8);
            if (Strings == null)
            {
                Strings = new SHStringXml();
                Strings.LoadingFail();
            }
            Strings.Compile();
        }

        public void LoadDialogs()
        {
            Dialogs = SHGlobal.Deserialize<SHDialogsXml>(SHGlobal.GetServerSystemFolder() + "\\dialog.xml", Encoding.UTF8);
            if (Dialogs == null)
            {
                Dialogs = new SHDialogsXml();
                Dialogs.LoadingFail();
            }

            SHDialogStringReplacer replacer = new SHDialogStringReplacer();
            replacer.ReplaceKeyToString(ref Dialogs, ref LocalXml.DialogString, true);

            Dialogs.Compile();
        }

        public void LoadQuests()
        {
            Quests = SHGlobal.Deserialize<SHQuestsXml>(SHGlobal.GetServerSystemFolder() + "\\quest.xml", Encoding.UTF8);
            if (Quests == null)
            {
                Quests = new SHQuestsXml();
                Quests.LoadingFail();
            }

            SHQuestStringReplacer replacer = new SHQuestStringReplacer();
            replacer.ReplaceKeyToString(ref Quests, ref LocalXml.QuestString, true);

            Quests.Compile();
        }

        public void LoadCraft()
        {
            Craft = SHGlobal.Deserialize<SHCraftXml>(SHGlobal.GetServerSystemFolder() + "\\craft.xml", Encoding.UTF8);
            Craft.Compile();
        }

        public void LoadNPCShop()
        {
            NPCShop = SHGlobal.Deserialize<SHNPCShopXml>(SHGlobal.GetServerSystemFolder() + "\\npcshop.xml", Encoding.UTF8);
            NPCShop.Compile();
        }

        public void LoadRecipe()
        {
            Recipe = SHGlobal.Deserialize<SHRecipeXml>(SHGlobal.GetServerSystemFolder() + "\\recipe.xml", Encoding.UTF8);
            Recipe.Compile();
        }

        public void LoadFieldList()
        {
            FieldList = SHGlobal.Deserialize<SHFieldListXml>(SHGlobal.GetServerSystemFolder() + "\\fieldlist.xml", Encoding.UTF8);
            FieldList.Compile();
        }

        public void LoadLocaleStrings()
        {
            if (LocalXml.NPCNameString != null) LocalXml.NPCNameString.Clear();
            LocalXml.NPCNameString = SHGlobal.Deserialize<SHStringXml>(SHGlobal.GetClientSystemFolder() + SHGlobal.Locale.Path() + "name_npc.xml", Encoding.UTF8);
            LocalXml.NPCNameString.Compile();

            if (LocalXml.ItemNameString != null) LocalXml.ItemNameString.Clear();
            LocalXml.ItemNameString = SHGlobal.Deserialize<SHStringXml>(SHGlobal.GetClientSystemFolder() + SHGlobal.Locale.Path() + "name_xitem.xml", Encoding.UTF8);
            LocalXml.ItemNameString.Compile();

            if (LocalXml.TalentNameString != null) LocalXml.TalentNameString.Clear();
            LocalXml.TalentNameString = SHGlobal.Deserialize<SHStringXml>(SHGlobal.GetClientSystemFolder() + SHGlobal.Locale.Path() + "name_talent.xml", Encoding.UTF8);
            LocalXml.TalentNameString.Compile();

            if (LocalXml.QuestString != null) LocalXml.QuestString.Clear();
            LocalXml.QuestString = SHGlobal.Deserialize<SHStringXml>(SHGlobal.GetClientSystemFolder() + SHGlobal.Locale.Path() + "name_quest.xml", Encoding.UTF8);
            LocalXml.QuestString.Compile();

            if (LocalXml.DialogString != null) LocalXml.DialogString.Clear();
            LocalXml.DialogString = SHGlobal.Deserialize<SHStringXml>(SHGlobal.GetClientSystemFolder() + SHGlobal.Locale.Path() + "name_dialog.xml", Encoding.UTF8);
            LocalXml.DialogString.Compile();
        }

        #endregion

        #region 데이터 저장 메소드들

        public void CreateBackupDirectory()
        {
            if (!Directory.Exists(Directory.GetCurrentDirectory() + "\\Backup"))
            {
                MessageBox.Show("백업 디렉토리가 없어서 생성하였습니다.\n경로는 다음과 같습니다.\n\n" + Directory.GetCurrentDirectory() + "\\Backup", Application.ProductName);
                Directory.CreateDirectory(Directory.GetCurrentDirectory() + "\\Backup");
            }
        }

        public void SaveNPCString()
        {
            if (NPCs.LoadingFailed) return;

            CreateBackupDirectory();

            File.Copy(SHGlobal.GetClientSystemFolder() + "\\lang\\ko_KR\\name_npc.xml", Directory.GetCurrentDirectory() + "\\Backup\\string_name_npc." + DateTime.Now.ToString("yy-MM-dd-HHmmss") + ".xml", true);

            // Save
            SHGlobal.Serialize<SHStringXml>(SHGlobal.GetClientSystemFolder() + SHGlobal.Locale.Path(SHLocaleType.KO_KR) + "name_npc.xml", LocalXml.NPCNameString, Encoding.UTF8);
        }

        public void SaveLootings()
        {
            if (Lootings.LoadingFailed) return;

            CreateBackupDirectory();
            // Backup
            File.Copy(SHGlobal.GetServerSystemFolder() + "\\loot.xml", Directory.GetCurrentDirectory() + "\\Backup\\loot." + DateTime.Now.ToString("yy-MM-dd-HHmmss") + ".xml", true);
            // Save
            SHGlobal.Serialize<SHLootingsXml>(SHGlobal.GetServerSystemFolder() + "\\loot.xml", Lootings, Encoding.UTF8);
        }

        public void SaveConditions()
        {
            if (Conditions.LoadingFailed) return;

            CreateBackupDirectory();
            // Backup
            File.Copy(SHGlobal.GetServerSystemFolder() + "\\conditions.xml", Directory.GetCurrentDirectory() + "\\Backup\\conditions." + DateTime.Now.ToString("yy-MM-dd-HHmmss") + ".xml", true);
            // Save
            SHGlobal.Serialize<SHConditionsXml>(SHGlobal.GetClientSystemFolder() + "\\conditions.xml", Conditions, Encoding.UTF8);
            SHGlobal.Serialize<SHConditionsXml>(SHGlobal.GetServerSystemFolder() + "\\conditions.xml", Conditions, Encoding.UTF8);
        }

        public void SaveItemConditions()
        {
            if (ItemConditions.LoadingFailed) return;

            CreateBackupDirectory();
            // Backup
            File.Copy(SHGlobal.GetServerSystemFolder() + "\\conditions_item.xml", Directory.GetCurrentDirectory() + "\\Backup\\conditions_item." + DateTime.Now.ToString("yy-MM-dd-HHmmss") + ".xml", true);
            // Save
            SHGlobal.Serialize<SHConditionsXml>(SHGlobal.GetClientSystemFolder() + "\\conditions_item.xml", ItemConditions, Encoding.UTF8);
            SHGlobal.Serialize<SHConditionsXml>(SHGlobal.GetServerSystemFolder() + "\\conditions_item.xml", ItemConditions, Encoding.UTF8);
        }

        public void SaveItems()
        {
            if (Items.LoadingFailed) return;

            CreateBackupDirectory();
            // Backup
            File.Copy(SHGlobal.GetClientSystemFolder() + "\\xitem.xml", Directory.GetCurrentDirectory() + "\\Backup\\xitem." + DateTime.Now.ToString("yy-MM-dd-HHmmss") + ".xml", true);
            // Save
            SHGlobal.Serialize<SHItemsXml>(SHGlobal.GetClientSystemFolder() + "\\xitem.xml", Items, Encoding.UTF8);
            SHGlobal.Serialize<SHItemsXml>(SHGlobal.GetServerSystemFolder() + "\\xitem.xml", Items, Encoding.UTF8);
        }

        public void SaveStrings()
        {
            if (Strings.LoadingFailed) return;

            CreateBackupDirectory();
            // Backup
            File.Copy(SHGlobal.GetClientSystemFolder() + "\\lang\\ko_KR\\strings.xml", Directory.GetCurrentDirectory() + "\\Backup\\strings." + DateTime.Now.ToString("yy-MM-dd-HHmmss") + ".xml", true);
            // Save
            SHGlobal.Serialize<SHStringXml>(SHGlobal.GetClientSystemFolder() + "\\lang\\ko_KR\\strings.xml", Strings, Encoding.UTF8);
        }

        public void SaveDialogs()
        {
            if (Dialogs.LoadingFailed) return;

            CreateBackupDirectory();

            SHDialogStringSaver stringSaver = new SHDialogStringSaver();
            stringSaver.Save(Dialogs, ref LocalXml.DialogString);

            // Backup
            File.Copy(SHGlobal.GetServerSystemFolder() + "\\dialog.xml", Directory.GetCurrentDirectory() + "\\Backup\\dialog." + DateTime.Now.ToString("yy-MM-dd-HHmmss") + ".xml", true);

            SHDialogStringReplacer replacer = new SHDialogStringReplacer();
            replacer.ReplaceStringToKey(ref Dialogs);

            // Save
            SHGlobal.Serialize<SHDialogsXml>(SHGlobal.GetClientSystemFolder() + "\\dialog.xml", Dialogs, Encoding.UTF8);
            SHGlobal.Serialize<SHDialogsXml>(SHGlobal.GetServerSystemFolder() + "\\dialog.xml", Dialogs, Encoding.UTF8);

            // 복구
            replacer.ReplaceKeyToString(ref Dialogs, ref LocalXml.DialogString, false);

        }

        public void SaveQuests()
        {
            if (Quests.LoadingFailed) return;

            CreateBackupDirectory();

            SHQuestStringSaver stringSaver = new SHQuestStringSaver();
            stringSaver.Save(Quests, ref LocalXml.QuestString);


            // Backup
            File.Copy(SHGlobal.GetServerSystemFolder() + "\\quest.xml", Directory.GetCurrentDirectory() + "\\Backup\\quest." + DateTime.Now.ToString("yy-MM-dd-HHmmss") + ".xml", true);


            // Save
            SHQuestStringReplacer replacer = new SHQuestStringReplacer();
            replacer.ReplaceStringToKey(ref Quests);

            SHGlobal.Serialize<SHQuestsXml>(SHGlobal.GetClientSystemFolder() + "\\quest.xml", Quests, Encoding.UTF8);
            SHGlobal.Serialize<SHQuestsXml>(SHGlobal.GetServerSystemFolder() + "\\quest.xml", Quests, Encoding.UTF8);

            // 복구
            replacer.ReplaceKeyToString(ref Quests, ref LocalXml.QuestString, false);
        }

        public void SaveColt(String path, SHColtXml xmlData)
        {
            SHGlobal.Serialize<SHColtXml>(path, xmlData, Encoding.UTF8);
        }

        public void SaveItemDropers()
        {
            SHItemRelatedNPCXml itemRelatedNPCXml = new SHItemRelatedNPCXml();

            foreach (SHLoot loot in Lootings.dataList)
            {
                for (int i = 0; i < loot.dataList.Count; i++)
                {
                    if (loot.dataList[i].GetType() != typeof(SHLootItem))
                    {
                        continue;
                    }

                    SHLootItem lootItem = loot.dataList[i] as SHLootItem;

                    ArrayList npcIDList = NPCs.GetNPCIDListByLootID(loot.id);
                    if (null == npcIDList) continue;

                    SHItemRelatedNPC_Item itemRelatedNPC_Item = itemRelatedNPCXml[lootItem.id];
                    if (null == itemRelatedNPC_Item)
                    {
                        itemRelatedNPC_Item = new SHItemRelatedNPC_Item();
                        itemRelatedNPC_Item.id = lootItem.id;
                        itemRelatedNPCXml.Add(itemRelatedNPC_Item);
                    }

                    foreach (int nNPCID in npcIDList)
                    {
                        SHItemRelatedNPC_NPC itemRelatedNPC_NPC = itemRelatedNPC_Item[nNPCID];
                        if (null == itemRelatedNPC_NPC)
                        {
                            itemRelatedNPC_NPC = new SHItemRelatedNPC_NPC();
                            itemRelatedNPC_NPC.id = nNPCID;
                            itemRelatedNPC_Item.Add(itemRelatedNPC_NPC);
                        }
                    }
                }
            }

            string strFileName = "\\item_droper.xml";

            if (File.Exists(SHGlobal.GetClientSystemFolder() + strFileName))
            {
                CreateBackupDirectory();

                // Backup
                File.Copy(SHGlobal.GetClientSystemFolder() + strFileName, Directory.GetCurrentDirectory() + "\\Backup\\conditions." + DateTime.Now.ToString("yy-MM-dd-HHmmss") + ".xml", true);
            }
            
            // Save
            SHGlobal.Serialize<SHItemRelatedNPCXml>(SHGlobal.GetClientSystemFolder() + strFileName, itemRelatedNPCXml, Encoding.UTF8);
        }

        public void SaveItemQuestRewarders()
        {
            SHItemRelatedNPCXml itemRelatedNPCXml = new SHItemRelatedNPCXml();

            foreach (SHQuest quest in Quests.dataList)
            {
                foreach (SHQuestReward questReward in quest.Rewards.dataList)
                {
                    if (SHQuestRewardType.ITEM != questReward.type) continue;

                    string[] strParam1s =  questReward.param1.Split(',');

                    foreach (string strParam1 in strParam1s)
                    {
                        int nItemID = int.Parse(strParam1);

                        ArrayList npcIDList = NPCs.GetNPCIDListByQuestID(quest.id);
                        if (null == npcIDList) continue;

                        SHItemRelatedNPC_Item itemRelatedNPC_Item = itemRelatedNPCXml[nItemID];
                        if (null == itemRelatedNPC_Item)
                        {
                            itemRelatedNPC_Item = new SHItemRelatedNPC_Item();
                            itemRelatedNPC_Item.id = nItemID;
                            itemRelatedNPCXml.Add(itemRelatedNPC_Item);
                        }

                        foreach (int nNPCID in npcIDList)
                        {
                            SHItemRelatedNPC_NPC itemRelatedNPC_NPC = itemRelatedNPC_Item[nNPCID];
                            if (null == itemRelatedNPC_NPC)
                            {
                                itemRelatedNPC_NPC = new SHItemRelatedNPC_NPC();
                                itemRelatedNPC_NPC.id = nNPCID;
                                itemRelatedNPC_Item.Add(itemRelatedNPC_NPC);
                            }
                        }
                    }                    
                }
            }

            string strFileName = "\\item_questrewarder.xml";
            if (File.Exists(SHGlobal.GetClientSystemFolder() + strFileName))
            {
                CreateBackupDirectory();
                // Backup
                File.Copy(SHGlobal.GetClientSystemFolder() + strFileName, Directory.GetCurrentDirectory() + "\\Backup\\conditions." + DateTime.Now.ToString("yy-MM-dd-HHmmss") + ".xml", true);
            }
            
            // Save
            SHGlobal.Serialize<SHItemRelatedNPCXml>(SHGlobal.GetClientSystemFolder() + strFileName, itemRelatedNPCXml, Encoding.UTF8);
        }

        #endregion

        public string GetSafeString(string key)
        {
            try
            {
                if (key != null && key.StartsWith("$$"))
                {
                    return Strings[key.Substring(2)].ToString() + "(!!)";
                }
                else
                {
                    return key;
                }
            }
            catch (KeyNotFoundException)
            {
                return key;
            }
            catch (Exception)
            {
                return key;
            }
        }

        /// <summary>
        /// "a" 태그로 둘러쌓인 문자열을 만든다. 이는 MainForm의 Navigate 함수를 호출하도록 구성되어 있다.
        /// </summary>
        /// <param name="szContent">본문의 내용</param>
        /// <param name="szType">a태그에서 참조할 타입. "QUEST", "NPC"등등이 가능하다. _TabPages[]에서 사용되는 Keyword와도 동일</param>
        /// <param name="szID">참조타입의 ID</param>
        /// <param name="bSafeString">szContent에 대하여 GetSafeString을 이용하여 문자열참조 연산자($$)를 사용할수 있게 할지 여부</param>
        /// <returns></returns>
        public string GetAnchorString(string szContent, string szType, string szID, bool bSafeString)
        {
            string ret;

            try
            {
                if (bSafeString)
                {
                    ret = "<a href=\"javascript:window.external.Navigate('" + szType + "', '" + szID + "')\">" + GetSafeString(szContent) + "</a>";
                }
                else
                {
                    ret = "<a href=\"javascript:window.external.Navigate('" + szType + "', '" + szID + "')\">" + szContent + "</a>";
                }
            }
            catch
            {
                ret = szContent;
            }

            return ret;
        }

        public void SaveNPCSpawnInfo()
        {
            ArrayList fieldList = FieldList.GetArrayList();

            foreach (SHField item in fieldList)
            {
                NPCInfoSave(item.id);
            }
        }

        public void NPCInfoSave(int fieldID)
        {
            string strFieldFileName = String.Format("\\{0}.field.xml", fieldID);

            if (File.Exists(SHGlobal.GetClientFieldFolder() + strFieldFileName) == false) return;

            SHFieldXml field = SHGlobal.Deserialize<SHFieldXml>(SHGlobal.GetClientFieldFolder() + strFieldFileName, Encoding.UTF8);
            field.Compile();

            SHFieldNPCXml fieldNPCXml = new SHFieldNPCXml();

            foreach (SHSpawn spawn in field.fieldInfo.spawnList)
            {
                if (false == spawn.enabled) continue;

                foreach (SHSpawnNPC spawnnpc in spawn.dataList)
                {
                    if (NPCs.IsValid(spawnnpc.id) == false) continue;

                    SHNPC npc = NPCs[spawnnpc.id];
                    if (npc.InteractCount == 0) continue;

                    string[] _Type = new string[2];
                    string[] _ICon = new string[2];
                    int Index = 0;

                    SHNPCIType[] _iType = npc.GetIType();
                    string[] _iAct = npc.GetIAct();
                    for (int i = 0; i < 10; i++)
                    {
                        if (_iType[i] == SHNPCIType.craft)
                        {
                            if (Craft.IsValid(_iAct[i]) == false) continue;

                            SHCraft craftinfo = Craft[_iAct[i]];
                            if (craftinfo.hidden == true) continue;

                            _Type[Index] = "craft";
                            _ICon[Index] = craftinfo.type;
                            Index++;

                            break;
                        }
                    }
                    for (int i = 0; i < 10; i++)
                    {
                        if (_iType[i] == SHNPCIType.trade)
                        {
                            if (NPCShop.IsValid(_iAct[i]) == false) continue;

                            _ICon[Index] = "common";
                            SHNPCShop npcshop = NPCShop[_iAct[i]];
                            if (npcshop.repairable == true)
                                _ICon[Index] = "repair";

                            if (npcshop.IsLicensable(Items, Talents) == true)
                                _ICon[Index] = "license";

                            _Type[Index] = "shop";
                            Index++;

                            break;
                        }
                    }

                    if (Index == 0) continue;

                    SHFieldNPC sNPC = new SHFieldNPC();
                    sNPC.id = spawnnpc.id;
                    sNPC.name = npc.Name;
                    sNPC.x = spawn.x;
                    sNPC.y = spawn.y;
                    sNPC.z = spawn.z;
                    sNPC.type1 = _Type[0];
                    sNPC.icon1 = _ICon[0];
                    sNPC.type2 = _Type[1];
                    sNPC.icon2 = _ICon[1];
                    fieldNPCXml.Add(sNPC);

                }
            }

            if (fieldNPCXml.dataList.Count == 0) return;

            string strFileName = String.Format("\\{0}.field.npc.xml", fieldID);
            SHGlobal.Serialize<SHFieldNPCXml>(SHGlobal.GetClientFieldFolder() + "\\map" + strFileName, fieldNPCXml, Encoding.UTF8);
        }

        public void SaveRecipeByNPC()
        {
            SHRecipeNPCXml RecipeNPCXml = new SHRecipeNPCXml();

            foreach (SHRecipe recipe in Recipe.dataList)
            {
                SHRecipeNPC sNPC = new SHRecipeNPC();
                sNPC.recipe = recipe.id;

                int Index = 0;
                foreach (SHCraft craft in Craft.dataList)
                {
                    foreach(int recipeid in craft.RecipeIDList)
                    {
                        if(recipe.id == recipeid)
                        {
                            foreach (SHNPC npc in NPCs.dataList)
                            {
                                SHNPCIType[] _iType = npc.GetIType();
                                string[] _iAct = npc.GetIAct();
                                for (int i = 0; i < 10; i++)
                                {
                                    if (_iType[i] == SHNPCIType.craft)
                                    {
                                        if (Craft.IsValid(_iAct[i]) == false) continue;

                                        SHCraft craftinfo = Craft[_iAct[i]];
                                        if (craftinfo.hidden == true) continue;

                                        int cID = Convert.ToInt32(_iAct[i]);
                                        if (craft.id == cID)
                                        {
                                            //
                                            if (Index >= 10) break;

                                            int FieldID = GetFieldIDByNPC(npc.id);
                                            if (FieldID == 0) continue;
                                            
                                            sNPC.Add(Index, npc.id.ToString(), FieldID.ToString());
                                            Index++;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                RecipeNPCXml.Add(sNPC);
            }

            string strFileName = "\\recipeNPC.xml";
            SHGlobal.Serialize<SHRecipeNPCXml>(SHGlobal.GetClientSystemFolder() + strFileName, RecipeNPCXml, Encoding.UTF8);
        }


        public int GetFieldIDByNPC(int NPCID)
        {
            ArrayList fieldList = FieldList.GetArrayList();

            foreach (SHField field in fieldList)
            {
                int fieldID = field.id;

                string strFieldFileName = String.Format("\\{0}.field.xml", fieldID);

                if (File.Exists(SHGlobal.GetClientFieldFolder() + strFieldFileName) == false) return 0;

                SHFieldXml fieldxml = SHGlobal.Deserialize<SHFieldXml>(SHGlobal.GetClientFieldFolder() + strFieldFileName, Encoding.UTF8);
                fieldxml.Compile();

                foreach (SHSpawn spawn in fieldxml.fieldInfo.spawnList)
                {
                    foreach (SHSpawnNPC spawnnpc in spawn.dataList)
                    {
                        if (NPCs.IsValid(spawnnpc.id) == false) continue;

                        if (spawnnpc.id == NPCID) return fieldID;
                    }
                }
            }
            return 0;
        }
    }
}