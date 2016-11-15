using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Xml;
using System.Collections;

namespace CombatMonitor.Types
{
    [XmlRootAttribute(ElementName = "Player", IsNullable = false)]
    public class Player : Actor
    {
        public int Equip_head = 0;
        public int Equip_face = 0;
        public int Equip_hands = 0;
        public int Equip_feet = 0;
        public int Equip_body = 0;
        public int Equip_leg = 0;
        public int Equip_lfinger = 0;
        public int Equip_rfinger = 0;
        public int Equip_neck = 0;
        public int Equip_charm = 0;
        public int Equip_lweapon = 0;
        public int Equip_rweapon = 0;
        public int Equip_lweapon2 = 0;
        public int Equip_rweapon2 = 0;

        public override ArrayList ExtractReport(ref ArrayList arr)
        {
            arr = base.ExtractReport(ref arr);
            arr.Add(new Report("< 장비목록 >"));
            arr.Add(new Report("머리", GetItemName(Equip_head)));
            arr.Add(new Report("얼굴", GetItemName(Equip_face)));
            arr.Add(new Report("손", GetItemName(Equip_hands)));
            arr.Add(new Report("발", GetItemName(Equip_feet)));
            arr.Add(new Report("몸", GetItemName(Equip_body)));
            arr.Add(new Report("다리", GetItemName(Equip_leg)));
            arr.Add(new Report("손가락(왼쪽)", GetItemName(Equip_lfinger)));
            arr.Add(new Report("손가락(오른쪽)", GetItemName(Equip_rfinger)));
            arr.Add(new Report("목", GetItemName(Equip_neck)));
            arr.Add(new Report("장신구", GetItemName(Equip_charm)));
            arr.Add(new Report("주무기(왼손)", GetItemName(Equip_lweapon)));
            arr.Add(new Report("주무기(오른손)", GetItemName(Equip_rweapon)));
            arr.Add(new Report("보조무기(왼손)", GetItemName(Equip_lweapon2)));
            arr.Add(new Report("보조무기(오른손)", GetItemName(Equip_rweapon2)));

            return arr;
        }

        public string GetItemName(int nItemID)
        {
            XML.XMLItemElement e = Global.Storage_ItemInfo.Get(nItemID);
            if (e == null)
                return "";

            return string.Format("{0}({1})", e.name, e.id.ToString());
        }
    }
}
