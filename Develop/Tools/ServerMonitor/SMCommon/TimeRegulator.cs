using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SMCommon
{
    public class TimeRegulator
    {
        private	bool m_bActive = false;
        private float m_fElapsed = 0.0f;
        private float m_fSettedTick = 0.0f;

        public float ElapsedTime { get { return m_fElapsed; } set { m_fElapsed = value; } }
        public float SettedTime { get { return m_fSettedTick; } set { m_fSettedTick = value; } }
        public bool IsActive { get { return m_bActive; } }

        public void Start(float fSettingTick)
        {
            m_bActive = true;
            m_fElapsed = 0.0f;
            m_fSettedTick = fSettingTick;
        }

        public void Stop()
        {
            m_bActive = false;
        }

        public bool IsReady(float fDelta)
        {
            if (!m_bActive) return false;

            m_fElapsed += fDelta;
            if (m_fElapsed >= m_fSettedTick)
            {
                m_fElapsed = 0.0f;
                return true;
            }
            return false;
        }

        public void Reset()
        {
            m_fElapsed = 0.0f;
        }

    }
}
