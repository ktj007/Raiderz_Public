using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace CombatMonitor.Stats
{
    public class TalentElement
    {
        public string Name = "";
        public int ID = 0;
        public int HitSegmentQty = 0;

        // Offnese
        public int Use = 0;
        public int Act = 0;

        // Defense
        public int Hit = 0;
        public int HitSuccess = 0;
        public int HitAll = 0;
        public int CriHit = 0;
        public int Avoid = 0;
        public int Guard = 0;
        public int PerfectGuard = 0;
        public int PartialGuard = 0;
        public int AbsoluteGuard = 0;
        public int Immuned = 0;
        public int Damage = 0;
    }

    public class TalentAnalysis
    {
        public string Name = "";
        public int ID = 0;

        public float Success;
        public float Invalidation;
        public float Failed;
        public float Critical;
        public float GuardQuality;
        public int Damage;
    }

    public class Talent
    {
        public ArrayList talents;
        private Dictionary<int, TalentElement> Index;

        public Talent()
        {
            talents = new ArrayList();
            Index = new Dictionary<int, TalentElement>();
        }

        public bool Has(int nIndex)
        {
            TalentElement e;
            if (!Index.TryGetValue(nIndex, out e))
                return false;

            return true;
        }

        public TalentElement Add(int key)
        {
            TalentElement dummy;
            if (Index.TryGetValue(key, out dummy))
            {
                // already exist
                return null;
            }

            TalentElement t = new TalentElement();
            t.ID = key;
            XML.XMLTalentElement te = Global.Storage_TalentInfo.Get(key);
            t.Name = te!=null?te.name:"unknown";
            XML.XMLTalentHitElement th = Global.Storage_TalentHitInfo.Get(key);
            t.HitSegmentQty = th != null ? th.SegmentQty : 0;
            talents.Add(t);
            Index[key] = t;
            return t;
        }

        public TalentElement Get(int key)
        {
            TalentElement e;
            if (Index.TryGetValue(key, out e))
            {
                return e;
            }
            else
            {
                return Add(key);
            }

            return null;
        }

        public void Clear()
        {
            talents.Clear();
            Index.Clear();
        }
    }
}
