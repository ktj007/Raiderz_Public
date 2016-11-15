using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Xml;
using System.ComponentModel;

namespace ColtViewer.Model.Types
{
    [XmlRootAttribute(ElementName = "NPCAttr", IsNullable = false)]
    public class NPCAttr
    {
        public NPCAttr()
        {
            Name = "Unknown";
            Userdata0 = -1;
            Userdata1 = -1;
            Userdata2 = -1;
            Userdata3 = -1;
            Userdata4 = -1;

            Timer1 = "none";
            Timer2 = "none";
            Timer3 = "none";
            Timer4 = "none";
            Timer5 = "none";
        }

        [CategoryAttribute("01. ID")]
        //[DescriptionAttribute("테스트 설명")]
        public string Name { get; set; }
        [CategoryAttribute("01. ID")]
        public int ID { get; set; }
        [CategoryAttribute("01. ID")]
        public int UIID { get; set; }
        [CategoryAttribute("01. ID")]
        public int SpawnID { get; set; }

        [CategoryAttribute("02. Feature")]
        public int Level { get; set; }
        [CategoryAttribute("02. Feature")]
        public int Mode { get; set; }
        [CategoryAttribute("02. Feature")]
        public int Grade { get; set; }

        [CategoryAttribute("03. Status")]
        public float WalkSpeed { get; set; }
        [CategoryAttribute("03. Status")]
        public float RunSpeed { get; set; }
        [CategoryAttribute("03. Status")]
        public int IsNowCombat { get; set; }
        [CategoryAttribute("03. Status")]
        public string Motionfactor { get; set; }
        [CategoryAttribute("03. Status")]
        public string Attackable { get; set; }
        [CategoryAttribute("03. Status")]
        public string CounterAttack { get; set; }
        [CategoryAttribute("03. Status")]
        public string AA { get; set; }

        [CategoryAttribute("04. AI")]
        public int Stress { get; set; }
        [CategoryAttribute("04. AI")]
        public int Rage { get; set; }
        [CategoryAttribute("04. AI")]
        public string FSM { get; set; }
        [CategoryAttribute("04. AI")]
        public float SightRange { get; set; }
        [CategoryAttribute("04. AI")]
        public string AutoAssist { get; set; }
        [CategoryAttribute("04. AI")]
        public float AutoAssistRange { get; set; }
        [CategoryAttribute("04. AI")]
        public string CurrentJob { get; set; }
        [CategoryAttribute("04. AI")]
        public string CurrentTask { get; set; }


        [CategoryAttribute("05. Talent")]
        public int UsingTalentID { get; set; }
        [CategoryAttribute("05. Talent")]
        public string UsingTalentPhase { get; set; }

        [CategoryAttribute("06. Motionfactor")]
        public int BEATEN { get; set; }
        [CategoryAttribute("06. Motionfactor")]
        public int KNOCKBACK { get; set; }
        [CategoryAttribute("06. Motionfactor")]
        public int STUN { get; set; }
        [CategoryAttribute("06. Motionfactor")]
        public int DEFLECT { get; set; }
        [CategoryAttribute("06. Motionfactor")]
        public int THROWUP { get; set; }
        [CategoryAttribute("06. Motionfactor")]
        public int KNOCKDOWN { get; set; }
        [CategoryAttribute("06. Motionfactor")]
        public int UPPERED { get; set; }

        [CategoryAttribute("07. Userdata")]
        public int Userdata0 { get; set; }
        [CategoryAttribute("07. Userdata")]
        public int Userdata1 { get; set; }
        [CategoryAttribute("07. Userdata")]
        public int Userdata2 { get; set; }
        [CategoryAttribute("07. Userdata")]
        public int Userdata3 { get; set; }
        [CategoryAttribute("07. Userdata")]
        public int Userdata4 { get; set; }

        [CategoryAttribute("08. Hatetable")]
        public string Hatetable1 { get; set; }
        [CategoryAttribute("08. Hatetable")]
        public string Hatetable2 { get; set; }
        [CategoryAttribute("08. Hatetable")]
        public string Hatetable3 { get; set; }
        [CategoryAttribute("08. Hatetable")]
        public string Hatetable4 { get; set; }
        [CategoryAttribute("08. Hatetable")]
        public string Hatetable5 { get; set; }

        [CategoryAttribute("09. Timer")]
        public string Timer1 { get; set; }
        [CategoryAttribute("09. Timer")]
        public string Timer2 { get; set; }
        [CategoryAttribute("09. Timer")]
        public string Timer3 { get; set; }
        [CategoryAttribute("09. Timer")]
        public string Timer4 { get; set; }
        [CategoryAttribute("09. Timer")]
        public string Timer5 { get; set; }

        [CategoryAttribute("10. AI Room")]
        public int GroupID { get; set; }
        [CategoryAttribute("10. AI Room")]
        public bool IsActivateUpdate { get; set; }

        [CategoryAttribute("11. Cooltime")]
        public string Cooltime1 { get; set; }
        [CategoryAttribute("11. Cooltime")]
        public string Cooltime2 { get; set; }
        [CategoryAttribute("11. Cooltime")]
        public string Cooltime3 { get; set; }
        [CategoryAttribute("11. Cooltime")]
        public string Cooltime4 { get; set; }
        [CategoryAttribute("11. Cooltime")]
        public string Cooltime5 { get; set; }
    }
}
