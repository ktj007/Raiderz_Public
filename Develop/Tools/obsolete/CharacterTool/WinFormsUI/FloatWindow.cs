// *****************************************************************************
// 
//  Copyright 2003, Weifen Luo
//  All rights reserved. The software and associated documentation 
//  supplied hereunder are the proprietary information of Weifen Luo
//  and are supplied subject to licence terms.
// 
//  WinFormsUI Library Version 1.0
// *****************************************************************************

using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;

namespace WeifenLuo.WinFormsUI
{
	/// <summary>
	/// Descripiton of class FloatWindow
	/// </summary>
	public class FloatWindow : Form
	{
		internal static Size DefaultWindowSize = new Size(300, 300);

		public FloatWindow(DockManager dockManager, ContentWindow contentWindow)
		{
			if (dockManager == null)
				throw(new ArgumentNullException(ResourceHelper.GetString("FloatWindow.Constructor.NullDockManager")));

			if (contentWindow == null)
				throw(new ArgumentNullException(ResourceHelper.GetString("FloatWindow.Constructor.NullContentWindow")));

			InternalConstruct(dockManager, contentWindow, false, Rectangle.Empty);
		}

		public FloatWindow(DockManager dockManager, ContentWindow contentWindow, Rectangle bounds)
		{
			if (dockManager == null)
				throw(new ArgumentNullException(ResourceHelper.GetString("FloatWindow.Constructor.NullDockManager")));

			if (contentWindow == null)
				throw(new ArgumentNullException(ResourceHelper.GetString("FloatWindow.Constructor.NullContentWindow")));

			InternalConstruct(dockManager, contentWindow, true, bounds);
		}

		internal FloatWindow(DockManager dockManager, Rectangle bounds)
		{
			InternalConstruct(dockManager, null, true, bounds);
		}

		private void InternalConstruct(DockManager dockManager, ContentWindow contentWindow, bool boundsSpecified, Rectangle bounds)
		{
			m_contentWindows = new ContentWindowCollection();

			FormBorderStyle = FormBorderStyle.SizableToolWindow;
			ShowInTaskbar = false;
			if (boundsSpecified)
			{
				Bounds = bounds;
				StartPosition = FormStartPosition.Manual;
			}
			else
				StartPosition = FormStartPosition.WindowsDefaultLocation;

			m_dummyControl = new Control();
			m_dummyControl.Bounds = Rectangle.Empty;
			Controls.Add(m_dummyControl);

			m_dockManager = dockManager;
			Owner = DockManager.FindForm();
			DockManager.AddFloatWindow(this);
			if (contentWindow != null)
				contentWindow.FloatWindow = this;
		}

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				ContentWindows.Dispose();
				if (DockManager != null)
					DockManager.RemoveFloatWindow(this);
				m_dockManager = null;
			}
			base.Dispose(disposing);
		}

		private bool m_allowRedocking = true;
		public bool AllowRedocking
		{
			get	{	return m_allowRedocking;	}
			set	{	m_allowRedocking = value;	}
		}

		private ContentWindowCollection m_contentWindows;
		public ContentWindowCollection ContentWindows
		{
			get	{	return m_contentWindows;	}
		}
		
		private int CountOfVisibleContentWindows
		{
			get
			{
				int count = 0;
				foreach (ContentWindow cw in ContentWindows)
					if (cw.DockState == DockState.Float)
						count ++;

				return count;
			}
		}

		protected override Size DefaultSize
		{
			get	{	return FloatWindow.DefaultWindowSize;	}
		}

		private DockManager m_dockManager;
		public DockManager DockManager
		{
			get	{	return m_dockManager;	}
		}

		private Control m_dummyControl;
		internal Control DummyControl
		{
			get	{	return m_dummyControl;	}
		}

		internal void AddContentWindow(ContentWindow cw)
		{
			m_contentWindows.Add(cw);
			RefreshContentWindows();
		}

		public bool IsDockStateValid(DockState dockState)
		{
			foreach (ContentWindow cw in ContentWindows)
				foreach (Content c in cw.Contents)
					if (!DockHelper.IsDockStateValid(dockState, c.AllowedStates))
						return false;

			return true;
		}

		internal void RefreshContentWindows()
		{
			int visibleWindows = CountOfVisibleContentWindows;
			if (visibleWindows == 0)
			{
				if (Owner != null)
					Owner.Activate();
				Hide();
				return;
			}

			ContentWindow first = null;
			ContentWindow prev = null;
			foreach (ContentWindow cw in ContentWindows)
			{
				if (cw.DockState != DockState.Float)
				{
					if (cw.TabWindowFloat != null)
						cw.TabWindowFloat.SetParent(null);
					continue;
				}

				if (first == null)
				{
					first = cw;
					cw.SetParent(this);
					cw.TabWindow.Dock = DockStyle.Fill;
				}
				else
				{
					cw.SetParent(prev.TabWindow);
					cw.TabWindow.Dock = DockStyle.None;
				}
				((DockWindow)cw.TabWindow).HasCaption = (visibleWindows > 1);
				cw.Refresh();
				prev = cw;
			}

			SetText();
			Show();
		}

		internal void RemoveContentWindow(ContentWindow cw)
		{
			m_contentWindows.Remove(cw);
			if (Contains(cw.TabWindow))
				cw.SetParent(null);

			if (ContentWindows.Count == 0)
			{
				if (Owner != null)
					Owner.Activate();
				Close();
			}
			else
				RefreshContentWindows();
		}

		public void SetContentWindowIndex(ContentWindow cw, int index)
		{
			int oldIndex = ContentWindows.IndexOf(cw);
			if (oldIndex == -1)
				throw(new ArgumentException(ResourceHelper.GetString("FloatWindow.SetContentWindowIndex.InvalidContentWindow")));

			if (index < 0 || index > ContentWindows.Count - 1)
				if (index != -1)
					throw(new ArgumentOutOfRangeException(ResourceHelper.GetString("FloatWindow.SetContentWindowIndex.InvalidIndex")));
				
			if (oldIndex == index)
				return;
			if (oldIndex == ContentWindows.Count - 1 && index == -1)
				return;

			ContentWindows.Remove(cw);
			if (index == -1)
				ContentWindows.Add(cw);
			else if (oldIndex < index)
				ContentWindows.AddAt(cw, index - 1);
			else
				ContentWindows.AddAt(cw, index);

			RefreshContentWindows();
		}

		internal void SetText()
		{
			ContentWindow first = null;
			int visibleWindows = 0;
			
			foreach (ContentWindow cw in ContentWindows)
			{
				if (cw.DockState != DockState.Float)
					continue;

				if (first == null)
					first = cw;
				visibleWindows ++;
			}

			if (visibleWindows == 1)
				Text = (first.ActiveContent == null ? string.Empty : first.ActiveContent.Text);
			else
				Text = string.Empty;
		}

		protected override void WndProc(ref Message m)
		{
			if (m.Msg == (int)Win32.Msgs.WM_NCLBUTTONDOWN)
			{
				if (IsDisposed)
					return;

				uint result = User32.SendMessage(this.Handle, (int)Win32.Msgs.WM_NCHITTEST, 0, (uint)m.LParam);
				if (result == 2 && DockManager.AllowRedocking && this.AllowRedocking)	// HITTEST_CAPTION
				{
					Activate();
					m_dockManager.DragHandler.BeginDragFloatWindow(this);
				}
				else
					base.WndProc(ref m);

				return;
			}
			else if (m.Msg == (int)Win32.Msgs.WM_CLOSE)
			{
				if (ContentWindows.Count == 0)
				{
					base.WndProc(ref m);
					return;
				}
				
				for (int i=ContentWindows.Count - 1; i>=0; i--)
				{
					if (ContentWindows[i].DockState != DockState.Float)
						continue;

					ContentCollection contents = ContentWindows[i].Contents;
					for (int j=contents.Count - 1; j>=0; j--)
					{
						Content c = contents[j];
						if (c.HideOnClose)
							c.Hide();
						else
							c.Close();
					}
				}
				
				return;
			}
			else if (m.Msg == (int)Win32.Msgs.WM_NCLBUTTONDBLCLK)
			{
				uint result = User32.SendMessage(this.Handle, (int)Win32.Msgs.WM_NCHITTEST, 0, (uint)m.LParam);
				if (result != 2)	// HITTEST_CAPTION
				{
					base.WndProc(ref m);
					return;
				}
				
				// Show as RestoreState
				foreach(ContentWindow cw in ContentWindows)
				{
					if (cw.DockState != DockState.Float)
						continue;

					if (cw.RestoreState != DockState.Unknown)
					{
						cw.VisibleState = cw.RestoreState;
						cw.Activate();
					}
				}

				return;
			}

			base.WndProc(ref m);
		}
	}
}
