using System;
using System.Text;
using System.Windows.Forms;
using SoulHunt;

namespace Velixian
{
    static class MainProgram
    {
        /// <summary>
        /// 해당 응용 프로그램의 주 진입점입니다.
        /// </summary>
        [STAThread]
        static void Main(String[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            
            SHGlobal.Init();
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\npc.xml", SHSCF_Type.Both, Encoding.GetEncoding(51949)));
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\talent.xml", SHSCF_Type.Both, Encoding.GetEncoding(51949)));
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\conditions.xml", SHSCF_Type.Server, Encoding.UTF8));
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\conditions_item.xml", SHSCF_Type.Both, Encoding.UTF8));
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\xitem.xml", SHSCF_Type.Both, Encoding.GetEncoding(51949)));
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\lang\\ko_KR\\strings.xml", SHSCF_Type.Client, Encoding.UTF8));
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\dialog.xml", "\\dialog.xsd", SHSCF_Type.Server, Encoding.UTF8));
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\quest.xml", "\\quest.xsd", SHSCF_Type.Both, Encoding.UTF8));
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\loot.xml", SHSCF_Type.Server, Encoding.UTF8));
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\craft.xml", SHSCF_Type.Server, Encoding.UTF8));
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\npcshop.xml", SHSCF_Type.Server, Encoding.UTF8));
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\recipe.xml", SHSCF_Type.Both, Encoding.UTF8));
            SHGlobal.AddCheckFile(new SHSmartCheckFile("\\fieldlist.xml", SHSCF_Type.Both, Encoding.UTF8));

            if (0 >= args.Length)
            {
                Application.Run(new MainForm());
            }
            else if ("droper" == args[0])
            {
                SHXmlCore xmlCore = new SHXmlCore();
                xmlCore.SaveItemDropers();
            }
            else if ("questrewarder" == args[0])
            {
                SHXmlCore xmlCore = new SHXmlCore();
                xmlCore.SaveItemQuestRewarders();
            }
            else if ("fieldnpc" == args[0])
            {
                SHXmlCore xmlCore = new SHXmlCore();
                xmlCore.SaveNPCSpawnInfo();
            }
            else if ("recipenpc" == args[0])
            {
                SHXmlCore xmlCore = new SHXmlCore();
                xmlCore.SaveRecipeByNPC();
            }
            else if ("all" == args[0])
            {
                SHXmlCore xmlCore = new SHXmlCore();
                xmlCore.SaveItemDropers();
                xmlCore.SaveItemQuestRewarders();
                xmlCore.SaveNPCSpawnInfo();
                xmlCore.SaveRecipeByNPC();
            }
        }
    }
}
