using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using CombatMonitor.Types;

namespace CombatMonitor.Stats
{
    public class PlayerElement : ActorElement
    {
        public Types.Player proto;

        public override ArrayList ExtractReportAnalysis()
        {
            ArrayList arr = new ArrayList();
            ExtractAnalysisImpl(ref arr);
            return arr;
        }

        public override ArrayList ExtractReportBasic()
        {
            ArrayList arr = new ArrayList();
            arr = proto.ExtractReport(ref arr);
            arr.Add(new Report("CombatTime", CombatTime.ToString()));
            return arr;
        }
    }

    class Player : Actor<PlayerElement>
    {
        public ArrayList ExtractReportAnalysis()
        {
            ArrayList arr = new ArrayList();
            foreach (PlayerElement each in dataList)
            {
                arr.Add(new Report(String.Format("{0} ({1})", each.proto.Name, each.proto.UIID.ToString()), each.GetOffenseDPS().ToString(), each.GetOffenseDamage().ToString()));
            }
            return arr;
        }
    }
}
