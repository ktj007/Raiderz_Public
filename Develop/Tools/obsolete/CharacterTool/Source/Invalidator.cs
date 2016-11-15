using System;
using System.Collections;
using System.Diagnostics;
using System.Windows.Forms;
using RSManaged;

namespace CharacterTool
{
	/// <summary>
	/// class Invalidator
	/// </summary>
	public class Invalidator
	{
		private static Invalidator ms_pkThis = null;
		public static Invalidator Instance
		{
			get
			{
				Debug.Assert(ms_pkThis != null,
					"Static Invalidator pointer is null");
				return ms_pkThis;
			}
		}

		public static void Init()
		{
			ms_pkThis = new Invalidator();
		}

		public static void Final()
		{
			ms_pkThis.Dispose();
			ms_pkThis = null;
		}

		private System.Windows.Forms.Timer m_Timer;
		private ArrayList m_aControls = new ArrayList();

		public Invalidator()
		{
			m_Timer = new System.Windows.Forms.Timer();
			m_Timer.Interval = 1;
			m_Timer.Tick += new EventHandler(this.OnTick);
			m_Timer.Start();
		}

		public void Dispose()
		{
			m_Timer.Stop();
			m_Timer.Dispose();
			m_Timer = null;
		}

		private void OnTick(object sender, EventArgs e)
		{
			Update(false);
		}

		public void RegisterControl(Control control)
		{
			m_aControls.Add(control);
		}

		public void UnregisterControl(Control control)
		{
			m_aControls.Remove(control);
		}

		// 프레임을 최대 100프레임으로 제한
		private const long ms_nMinFrameTime = 10;
		private long m_nLastTime = 0;

		public void Update(bool bForceUpdate)
		{
			long nCurrentTime = DateTime.Now.Ticks;

			if (m_nLastTime == 0)
			{
				m_nLastTime = nCurrentTime;
			}
			if (!bForceUpdate && nCurrentTime - m_nLastTime < ms_nMinFrameTime)
			{
				return;
			}

			m_nLastTime = nCurrentTime;

			foreach(Control control in m_aControls)
			{
				control.Invalidate();
			}
		}
	}
}
