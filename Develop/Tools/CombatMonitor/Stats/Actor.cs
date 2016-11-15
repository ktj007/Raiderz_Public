using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using CombatMonitor.Types;

namespace CombatMonitor.Stats
{
    public class ActorElement
    {
        public int UIID = -1;
        public int CombatTime = 0;
        public bool IsNowCombat = false;
        public Talent talentOffense = new Talent();
        public Talent talentDefense = new Talent();

        public ActorElement()
        {
        }

        public void Clear()
        {
            talentOffense.Clear();
            talentDefense.Clear();
        }

        public virtual ArrayList ExtractReportAnalysis() { return new ArrayList();  }
        public virtual ArrayList ExtractReportBasic() { return new ArrayList(); }
        public void ExtractAnalysisImpl(ref ArrayList arr)
        {
            arr.Add(new Report("DPS", GetOffenseDPS().ToString()));
            arr.Add(new Report("누적피해 (공격측)", GetOffenseDamage().ToString()));
            arr.Add(new Report("누적피해 (방어측)", GetDefenseDamage().ToString()));
            arr.Add(new Report("치명타율 (%)", GetCriticalRate().ToString()));
            arr.Add(new Report("공격성공률 (%)", GetHitRate().ToString()));
        }

        public int GetOffenseDamage()
        {
            int nDamage = 0;
            foreach (TalentElement each in talentOffense.talents)
            {
                nDamage += each.Damage;
            }

            return nDamage;
        }

        public int GetDefenseDamage()
        {
            int nDamage = 0;
            foreach (TalentElement each in talentDefense.talents)
            {
                nDamage += each.Damage;
            }

            return nDamage;
        }

        public int GetOffenseDPS()
        {
            int nDamage = GetOffenseDamage();
            if (nDamage <= 0)
                return 0;

            if (CombatTime <= 0)
                return 0;

            return nDamage / CombatTime;
        }

        public float GetCriticalRate()
        {
            int nTotalHitCount = 0;
            int nTotalCriCount = 0;
            foreach (TalentElement each in talentOffense.talents)
            {
                nTotalHitCount += each.Hit;
                nTotalCriCount += each.CriHit;
            }

            if (nTotalHitCount <= 0)
                return 0.0f;

            return Global.ToPercent((float)nTotalCriCount / (float)nTotalHitCount);
        }

        public float GetHitRate()
        {
            int nTotalHitSuccess = 0;
            int nTotalHitSegmentQty = 0;
            int nTotalAct = 0;
            foreach (TalentElement each in talentOffense.talents)
            {
                nTotalHitSuccess += each.HitSuccess;
                nTotalHitSegmentQty += each.HitSegmentQty;
                nTotalAct += each.Act;
            }

            if (nTotalAct <= 0)
                return 0.0f;

            return Global.ToPercent((float)nTotalHitSuccess / (float)nTotalHitSegmentQty / (float)nTotalAct);
        }
    }
    
    public class Actor<ACTOR_TYPE>
    {
        private Dictionary<int, ACTOR_TYPE> Index;
        public ArrayList dataList;

        public ACTOR_TYPE this[int nIndex]
        {
            get
            {
                ACTOR_TYPE e;
                if (!Index.TryGetValue(nIndex, out e))
                    return default(ACTOR_TYPE);

                return e;
            }
        }

        public Actor()
        {
            dataList = new ArrayList();
            Index = new Dictionary<int, ACTOR_TYPE>();
        }

        public bool Has(int nIndex)
        {
            ACTOR_TYPE e;
            if (!Index.TryGetValue(nIndex, out e))
                return false;

            return true;
        }

        public bool Add(int key, ACTOR_TYPE i)
        {
            ACTOR_TYPE dummy;
            if (Index.TryGetValue(key, out dummy))
            {
                // already exist
                return false;
            }

            dataList.Add(i);
            Index[key] = i;
            return true;
        }

        public void Clear()
        {
            dataList.Clear();
            Index.Clear();
        }
    }
}
