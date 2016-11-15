using System;
using System.IO;
using System.Windows.Forms;
using System.Text;
using System.Collections;
using System.Collections.Generic;

namespace SoulHunt
{
    public class SHStringSaver
    {
        protected void AddString(string szKey, string szValue, ref SHStringXml stringXml)
        {
            SHString newTitleString = new SHString();
            newTitleString.Key = szKey;
            newTitleString.String = szValue;
            stringXml.Add(newTitleString);
        }
    }

    public class SHQuestStringSaver : SHStringSaver
    {
        public void Save(SHQuestsXml Quests, ref SHStringXml QuestString)
        {
            // 문자열 저장
            File.Copy(SHGlobal.GetClientSystemFolder() + "\\lang\\ko_KR\\name_quest.xml", Directory.GetCurrentDirectory() + "\\Backup\\string_name_quest." + DateTime.Now.ToString("yy-MM-dd-HHmmss") + ".xml", true);

            QuestString.Clear();

            foreach (SHQuest quest in Quests.dataList)
            {
                string szKey = "QUEST_TITLE_" + quest.id.ToString();
                AddString(szKey, quest.title, ref QuestString);

                szKey = "QUEST_DESC_" + quest.id.ToString();
                AddString(szKey, quest.desc, ref QuestString);

                szKey = "QUEST_CATEGORY_" + quest.id.ToString();
                AddString(szKey, quest.category, ref QuestString);

                if (quest.Objectives != null)
                {
                    for (int r = 0; r < quest.Objectives.dataList.Count; r++)
                    {
                        SHQuestObjective obj = (SHQuestObjective)quest.Objectives.dataList[r];

                        szKey = "QUEST_OBJ_DESC_" + quest.id.ToString();
                        szKey = szKey + "_" + obj.id.ToString();
                        AddString(szKey, obj.desc, ref QuestString);
                    }
                }
            }
            QuestString.Compile();

            // Save
            SHGlobal.Serialize<SHStringXml>(SHGlobal.GetClientSystemFolder() + SHGlobal.Locale.Path(SHLocaleType.KO_KR) + "name_quest.xml", QuestString, Encoding.UTF8);
        }
    }

    public class SHQuestStringReplacer
    {
        public void ReplaceStringToKey(ref SHQuestsXml Quests)
        {
            foreach (SHQuest quest in Quests.dataList)
            {
                //                quest.title = "QUEST_TITLE_" + quest.id.ToString();
                quest.desc = "QUEST_DESC_" + quest.id.ToString();
                quest.category = "QUEST_CATEGORY_" + quest.id.ToString();

                if (quest.Objectives != null)
                {
                    for (int r = 0; r < quest.Objectives.dataList.Count; r++)
                    {
                        SHQuestObjective obj = (SHQuestObjective)quest.Objectives.dataList[r];

                        obj.desc = "QUEST_OBJ_DESC_" + quest.id.ToString() + "_" + obj.id.ToString();
                    }
                }
            }
        }

        public void ReplaceKeyToString(ref SHQuestsXml Quests, ref SHStringXml QuestString, bool bReplaceTitle)
        {
            foreach (SHQuest quest in Quests.dataList)
            {
                string szKey;

                if (bReplaceTitle)
                {
                    szKey = "QUEST_TITLE_" + quest.id.ToString();

                    if (QuestString.IsValid(szKey))
                    {
                        quest.title = QuestString[szKey].ToString();
                    }
                }

                szKey = "QUEST_DESC_" + quest.id.ToString();

                if (QuestString.IsValid(szKey))
                {
                    quest.desc = QuestString[szKey].ToString();
                }

                szKey = "QUEST_CATEGORY_" + quest.id.ToString();

                if (QuestString.IsValid(szKey))
                {
                    quest.category = QuestString[szKey].ToString();
                }

                if (quest.Objectives != null)
                {
                    for (int r = 0; r < quest.Objectives.dataList.Count; r++)
                    {
                        SHQuestObjective obj = (SHQuestObjective)quest.Objectives.dataList[r];

                        szKey = "QUEST_OBJ_DESC_" + quest.id.ToString() + "_" + obj.id.ToString();

                        if (QuestString.IsValid(szKey))
                        {
                            obj.desc = QuestString[szKey].ToString();
                        }
                    }
                }
            }
        }
    }

    public class SHDialogStringSaver : SHStringSaver
    {
        private int m_nSaySelectID;

        public SHDialogStringSaver()
        {
            m_nSaySelectID = 0;
        }

        private void AddStringSelect(SHDialog dialog, SHDialogSelect dialogSelect, ref SHStringXml DialogString)
        {
            m_nSaySelectID++;

            string szKey = "DIALOG_SELECT_TEXT_" + dialog.id.ToString() + "_" + m_nSaySelectID;
            AddString(szKey, dialogSelect.text, ref DialogString);

            if (dialogSelect.Say != null) AddStringSay(dialog, dialogSelect.Say, ref DialogString);
        }

        private void AddStringSay(SHDialog dialog, SHDialogSay dialogSay, ref SHStringXml DialogString)
        {
            m_nSaySelectID++;

            string szKey = "DIALOG_SAY_TEXT_" + dialog.id.ToString() + "_" + m_nSaySelectID;
            AddString(szKey, dialogSay.text, ref DialogString);

            foreach (SHDialogSelect dialogSelect in dialogSay.dataList) AddStringSelect(dialog, dialogSelect, ref DialogString);
        }

        public void Save(SHDialogsXml Dialogs, ref SHStringXml DialogString)
        {
            // 문자열 저장
            File.Copy(SHGlobal.GetClientSystemFolder() + "\\lang\\ko_KR\\name_dialog.xml", Directory.GetCurrentDirectory() + "\\Backup\\string_name_dialog." + DateTime.Now.ToString("yy-MM-dd-HHmmss") + ".xml", true);

            DialogString.Clear();

            foreach (SHDialog dialog in Dialogs.dataList)
            {
                m_nSaySelectID = 0;

                string szKey = "DIALOG_TEXT_" + dialog.id.ToString();
                AddString(szKey, dialog.text, ref DialogString);

                if (dialog.Say != null) AddStringSay(dialog, dialog.Say, ref DialogString);
            }

            DialogString.Compile();

            // Save
            SHGlobal.Serialize<SHStringXml>(SHGlobal.GetClientSystemFolder() + SHGlobal.Locale.Path(SHLocaleType.KO_KR) + "name_dialog.xml", DialogString, Encoding.UTF8);
        }
    }

    public class SHDialogStringReplacer
    {
        private int m_nSaySelectID;

        public SHDialogStringReplacer()
        {
            m_nSaySelectID = 0;
        }

        private void ReplaceStringToKey_Select(int nDialogID, SHDialogSelect dialogSelect)
        {
            m_nSaySelectID++;

            dialogSelect.text = "DIALOG_SELECT_TEXT_" + nDialogID.ToString() + "_" + m_nSaySelectID.ToString();

            if (dialogSelect.Say != null) ReplaceStringToKey_Say(nDialogID, dialogSelect.Say);
        }

        private void ReplaceStringToKey_Say(int nDialogID, SHDialogSay dialogSay)
        {
            m_nSaySelectID++;

            dialogSay.text = "DIALOG_SAY_TEXT_" + nDialogID.ToString() + "_" + m_nSaySelectID.ToString();

            foreach (SHDialogSelect dialogSelect in dialogSay.dataList) ReplaceStringToKey_Select(nDialogID, dialogSelect);
        }

        public void ReplaceStringToKey(ref SHDialogsXml Dialogs)
        {
            foreach (SHDialog dialog in Dialogs.dataList)
            {
                m_nSaySelectID = 0;

                if (dialog.Say != null) ReplaceStringToKey_Say(dialog.id, dialog.Say);
            }
        }

        private void ReplaceKeyToString_Select(int nDialogID, SHDialogSelect dialogSelect, SHStringXml DialogString)
        {
            m_nSaySelectID++;

            string szKey = "DIALOG_SELECT_TEXT_" + nDialogID.ToString() + "_" + m_nSaySelectID.ToString();

            if (DialogString.IsValid(szKey))
            {
                dialogSelect.text = DialogString[szKey].ToString();
            }

            if (dialogSelect.Say != null) ReplaceKeyToString_Say(nDialogID, dialogSelect.Say, DialogString);
        }

        private void ReplaceKeyToString_Say(int nDialogID, SHDialogSay dialogSay, SHStringXml DialogString)
        {
            m_nSaySelectID++;

            string szKey = "DIALOG_SAY_TEXT_" + nDialogID.ToString() + "_" + m_nSaySelectID.ToString();

            if (DialogString.IsValid(szKey))
            {
                dialogSay.text = DialogString[szKey].ToString();
            }

            foreach (SHDialogSelect dialogSelect in dialogSay.dataList) ReplaceKeyToString_Select(nDialogID, dialogSelect, DialogString);
        }

        public void ReplaceKeyToString(ref SHDialogsXml Dialogs, ref SHStringXml DialogString, bool bReplaceDialogText)
        {
            foreach (SHDialog dialog in Dialogs.dataList)
            {
                m_nSaySelectID = 0;

                string szKey;

                if (bReplaceDialogText)
                {
                    szKey = "DIALOG_TEXT_" + dialog.id.ToString();

                    if (DialogString.IsValid(szKey))
                    {
                        dialog.text = DialogString[szKey].ToString();
                    }
                }

                if (dialog.Say != null) ReplaceKeyToString_Say(dialog.id, dialog.Say, DialogString);
            }
        }
    }
}