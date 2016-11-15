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
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace WeifenLuo.WinFormsUI
{
	/// <summary>
	/// Description of class ContentWindow
	/// </summary>
	public class ContentWindow : IDisposable
	{
		public ContentWindow(Content content, DockState dockState)
		{
			InternalConstruct(content, dockState, null, false, Rectangle.Empty);
		}

		public ContentWindow(Content content, DockState dockState, FloatWindow floatWindow)
		{
			InternalConstruct(content, dockState, floatWindow, false, Rectangle.Empty);
		}

		public ContentWindow(Content content, Rectangle floatWindowBounds)
		{
			InternalConstruct(content, DockState.Float, null, true, floatWindowBounds);
		}

		private void InternalConstruct(Content content, DockState dockState, FloatWindow floatWindow, bool flagBounds, Rectangle floatWindowBounds)
		{
			if (content == null)
				throw(new ArgumentNullException(ResourceHelper.GetString("ContentWindow.Constructor.NullContent")));

			if (dockState == DockState.Unknown || dockState == DockState.Hidden)
				throw(new ArgumentException(ResourceHelper.GetString("ContentWindow.Constructor.InvalidDockState")));

			if (content.DockManager == null)
				throw(new ArgumentException(ResourceHelper.GetString("ContentWindow.Constructor.InvalidDockManager")));

			m_events = new EventHandlerList();
			m_contents = new ContentCollection();

			m_dockManager = content.DockManager;
			m_dockManager.AddContentWindow(this);

			if (floatWindow != null)
				FloatWindow = floatWindow;
			else if (flagBounds)
				FloatWindow = new FloatWindow(DockManager, this, floatWindowBounds);

			VisibleState = dockState;
			content.ContentWindow = this;
		}

		~ContentWindow()
		{
			Dispose(false);
		}

		public void Dispose()
		{
			Dispose(true);
			GC.SuppressFinalize(this);
		}

		protected virtual void Dispose(bool disposing)
		{
			if (m_disposed)
				return;

			m_disposed = true;

			if (disposing)
			{
				Contents.Dispose();
				if (TabWindowDock != null)
					TabWindowDock.Dispose();
				if (TabWindowFloat != null)
					TabWindowFloat.Dispose();
				if (TabWindowDocument != null)
					TabWindowDocument.Dispose();
				Events.Dispose();

				if (DockManager != null)
				{
					DockManager.RemoveContentWindow(this);
					m_dockManager = null;
				}
			}
		}

		private Content m_activeContent = null;
		public Content ActiveContent
		{
			get	{	return m_activeContent;	}
			set
			{
				if (ActiveContent == value)
					return;

				if (value != null)
				{
					if (GetIndexOfVisibleContents(value) == -1)
						throw(new InvalidOperationException(ResourceHelper.GetString("ContentWindow.ActiveContent.InvalidValue")));
				}
				else
				{
					if (this.CountOfVisibleContents != 0)
						throw(new InvalidOperationException(ResourceHelper.GetString("ContentWindow.ActiveContent.InvalidValue")));
				}

				Content oldValue = m_activeContent;

				m_activeContent = value;
				if (m_activeContent != null)
					TabWindow.EnsureTabVisible(m_activeContent);

				if (DockManager.ActiveDocumentWindow == this)
					DockManager.SetActiveDocument(m_activeContent);

				if (FloatWindow != null)
					FloatWindow.SetText();

				Refresh();
			}
		}
		
		private bool m_allowRedocking = true;
		public bool AllowRedocking
		{
			get	{	return m_allowRedocking;	}
			set	{	m_allowRedocking = value;	}
		}

		public bool ContainsFocus
		{
			get
			{
				foreach (ContentWindow cw in DockManager.ContentWindows)
				{
					if (cw.TabWindow == null)
						continue;

					if (cw.TabWindow.ContainsFocus && Contains(cw))
						return false;
				}

				if (TabWindow == null)
					return false;
				else
					return TabWindow.ContainsFocus;
			}
		}

		private ContentCollection m_contents;
		public ContentCollection Contents
		{
			get	{	return m_contents;	}
		}

		internal int CountOfVisibleContents
		{
			get
			{
				int count = 0;
				foreach (Content c in Contents)
				{
					if (!c.IsHidden)
						count ++;
				}
				return count;
			}
		}

		private DockManager m_dockManager;
		public DockManager DockManager
		{
			get	{	return m_dockManager;	}
		}

		private DockState m_dockState = DockState.Unknown;
		public DockState DockState
		{
			get	{	return m_dockState;	}
		}
		internal void SetDockState()
		{
			DockState value;

			if (CountOfVisibleContents == 0)
				value = DockState.Unknown;
			else if (IsHidden)
				value = DockState.Hidden;
			else
				value = VisibleState;

			if (m_dockState == value)
				return;

			if (DockManager.ActiveAutoHideContent == ActiveContent &&
				!DockHelper.IsDockStateAutoHide(value))
			{
				bool oldAnimateAutoHide = DockManager.AnimateAutoHide;
				DockManager.AnimateAutoHide = false;
				DockManager.ActiveAutoHideContent = null;
				DockManager.AnimateAutoHide = oldAnimateAutoHide;
			}

			DockState oldDockState = m_dockState;
			m_dockState = value;

			if (DockManager.ActiveDocumentWindow == this && value != DockState.Document)
				DockManager.SetActiveDocumentWindow(DockManager.GetContentWindow(DockState.Document));

			if (DockState == DockState.Float)
			{
				if (FloatWindow == null)
					m_floatWindow = new FloatWindow(DockManager, this);
			}
			else if (DockState == DockState.Document)
			{
				if (TabWindowDocument == null)
					m_tabWindowDocument = new DocumentWindow(this);
			}
			else if (DockState != DockState.Hidden && DockState != DockState.Unknown)
			{
				if (TabWindowDock == null)
					m_tabWindowDock = new DockWindow(this);
				TabWindowDock.IsAutoHide = DockHelper.IsDockStateAutoHide(DockState);
			}

			// Temporarily make DockManager invisible to avoid screen flicks
			bool dockManagerVisible = DockManager.Visible;
			DockManager.Hide();
			SetParent(null);

			if (oldDockState == DockState.Float)
				FloatWindow.RefreshContentWindows();
			else
				DockManager.RefreshContentWindows(oldDockState);

			if (DockState == DockState.Float)
				FloatWindow.RefreshContentWindows();
			else
				DockManager.RefreshContentWindows(DockState);

			DockManager.Visible = dockManagerVisible;
			Refresh();

			foreach (Content c in Contents)
				c.SetDockState();

			if (DockHelper.IsValidRestoreState(DockState))
				RestoreState = DockState;

			OnDockStateChanged(EventArgs.Empty);
		}

		private EventHandlerList m_events;
		protected EventHandlerList Events
		{
			get	{	return m_events;	}
		}

		public ContentWindow First
		{
			get
			{
				for (int i=0; i<DockManager.ContentWindows.Count; i++)
				{
					if (DockManager.ContentWindows[i].DockState == DockState)
						return DockManager.ContentWindows[i];
				}

				return null;
			}
		}

		private bool m_flagMouseActivate = false;
		internal bool FlagMouseActivate
		{
			get	{	return m_flagMouseActivate;	}
			set
			{
				for (ContentWindow cw=this; cw!=null; cw = cw.Previous)
					cw.m_flagMouseActivate = value;
			}
		}

		private FloatWindow m_floatWindow = null;
		public FloatWindow FloatWindow
		{
			get	{	return m_floatWindow;	}
			set
			{
				if (m_floatWindow == value)
					return;

				if (TabWindowFloat == null)
					m_tabWindowFloat = new DockWindow(this, true);

				FloatWindow oldValue = m_floatWindow;

				m_floatWindow = value;

				if (oldValue != null)
					oldValue.RemoveContentWindow(this);
				if (value != null)
					value.AddContentWindow(this);
			}
		}

		private bool m_isActivated = false;
		public bool IsActivated
		{
			get	{	return m_isActivated;	}
			set
			{
				if (m_isActivated == value)
					return;

				if (value == false && DockManager.ActiveDocumentWindow == this)
					return;

				if (value == true)
					Show();

				if (value && TabWindow == null)
					return;

				m_isActivated = value;
				
				if (!m_isActivated && ContainsFocus)
					DockManager.AutoHideWindow.Focus();
				else if (m_isActivated)
				{
					for (ContentWindow cwPrev = Previous; cwPrev != null; cwPrev = cwPrev.Previous)
						if (cwPrev.Contains(this) && cwPrev.IsActivated)
							cwPrev.IsActivated = false;
					if (IsAutoHide)
						DockManager.ActiveAutoHideContent = ActiveContent;
					if (DockState == DockState.Document)
						DockManager.SetActiveDocumentWindow(this);

					TabWindow.Focus();

					if (DockState == DockState.Float)
						FloatWindow.Activate();
				}

				Refresh();
			}
		}

		public bool IsAutoHide
		{
			get	{	return DockHelper.IsDockStateAutoHide(DockState);	}
		}

		private bool m_disposed = false;
		public bool IsDisposed
		{
			get	{	return m_disposed;	}
		}
		
		public bool IsDockStateValid(DockState dockState)
		{
			foreach (Content c in Contents)
				if (!c.IsDockStateValid(dockState))
					return false;

			return true;
		}

		private bool m_isHidden = false;
		public bool IsHidden
		{
			get	{	return m_isHidden;	}
			set
			{
				m_isHidden = value;
				SetDockState();
			}
		}

		public ContentWindow Last
		{
			get
			{
				for (int i=DockManager.ContentWindows.Count-1; i>=0; i--)
				{
					if (DockManager.ContentWindows[i].DockState == DockState)
						return DockManager.ContentWindows[i];
				}

				return null;
			}
		}

		private LayoutStyles m_layoutDock = LayoutStyles.Vertical;
		public LayoutStyles LayoutDock
		{
			get	{	return m_layoutDock;	}
			set
			{
				if (m_layoutDock == value)
					return;

				m_layoutDock = value;
				if (DockHelper.IsDockStateDocked(DockState))
					PerformLayout();
			}
		}

		private LayoutStyles m_layoutDocument = LayoutStyles.Vertical;
		public LayoutStyles LayoutDocument
		{
			get	{	return m_layoutDocument;	}
			set
			{
				if (m_layoutDocument == value)
					return;

				m_layoutDocument = value;
				if (DockState == DockState.Document)
					PerformLayout();
			}
		}
		
		private LayoutStyles m_layoutFloat = LayoutStyles.Horizontal;
		public LayoutStyles LayoutFloat
		{
			get	{	return m_layoutFloat;	}
			set
			{
				if (m_layoutFloat == value)
					return;

				m_layoutFloat = value;
				if (DockState == DockState.Float)
					PerformLayout();
			}
		}
		
		public ContentWindow Next
		{
			get
			{
				ContentWindowCollection contentWindows;

				if (DockState == DockState.Float)
					contentWindows = FloatWindow.ContentWindows;
				else
					contentWindows = DockManager.ContentWindows;

				for (int i=contentWindows.IndexOf(this)+1; i<contentWindows.Count; i++)
				{
					if (contentWindows[i].DockState == DockState)
						return contentWindows[i];
				}

				return null;
			}
		}

		public ContentWindow Previous
		{
			get
			{
				ContentWindowCollection contentWindows;

				if (DockState == DockState.Float)
					contentWindows = FloatWindow.ContentWindows;
				else
					contentWindows = DockManager.ContentWindows;

				for (int i=contentWindows.IndexOf(this)-1; i>=0; i--)
				{
					if (contentWindows[i].DockState == DockState)
						return contentWindows[i];
				}

				return null;
			}
		}

		private DockState m_restoreState = DockState.Unknown;
		public DockState RestoreState
		{
			get	{	return m_restoreState;	}
			set
			{
				if (!DockHelper.IsValidRestoreState(value))
					throw(new InvalidOperationException(ResourceHelper.GetString("ContentWindow.RestoreState.InvalidValue")));

				m_restoreState = value;
			}
		}

		private double m_sizeDock = 0.5;
		public double SizeDock
		{
			get	{	return m_sizeDock;	}
			set
			{
				if (value <= 0 || value >= 1)
					throw(new ArgumentOutOfRangeException());

				if (m_sizeDock == value)
					return;

				m_sizeDock = value;
				if (DockHelper.IsDockStateDocked(DockState))
					PerformLayout();
			}
		}

		private double m_sizeDocument = 0.5;
		public double SizeDocument
		{
			get	{	return m_sizeDocument;	}
			set
			{
				if (value <= 0 || value >= 1)
					throw(new ArgumentOutOfRangeException());

				if (m_sizeDocument == value)
					return;

				m_sizeDocument = value;
				if (DockState == DockState.Document)
					PerformLayout();
			}
		}

		private double m_sizeFloat = 0.5;
		public double SizeFloat
		{
			get	{	return m_sizeFloat;	}
			set
			{
				if (value <= 0 || value >= 1)
					throw(new ArgumentOutOfRangeException());

				if (m_sizeFloat == value)
					return;

				m_sizeFloat = value;
				if (DockState == DockState.Float)
					PerformLayout();
			}
		}

		internal double SizeLayout
		{
			get
			{
				if (DockState == DockState.Float)
					return SizeFloat;
				else if (DockState == DockState.Document)
					return SizeDocument;
				else
					return SizeDock;
			}
		}

		internal TabWindow TabWindow
		{
			get	{	return GetTabWindow(DockState);	}
		}

		private DockWindow m_tabWindowDock = null;
		private DockWindow TabWindowDock
		{
			get	{	return m_tabWindowDock;	}
		}

		private DocumentWindow m_tabWindowDocument = null;
		private DocumentWindow TabWindowDocument
		{
			get	{	return m_tabWindowDocument;	}
		}

		private DockWindow m_tabWindowFloat = null;
		internal DockWindow TabWindowFloat
		{
			get	{	return m_tabWindowFloat;	}
		}

		public string Text
		{
			get	{	return (ActiveContent==null ? string.Empty : ActiveContent.Text);	}
		}

		private DockState m_visibleState;
		public DockState VisibleState
		{
			get	{	return m_visibleState;	}
			set
			{
				if (value == DockState.Unknown || value == DockState.Hidden || !IsDockStateValid(value))
					throw(new InvalidOperationException(ResourceHelper.GetString("ContentWindow.VisibleState.InvalidVisibleState")));

				m_visibleState = value;
				SetDockState();
			}
		}

		public void Activate()
		{
			IsActivated = true;
		}

		internal void AddContent(Content c)
		{
			if (Contents.Contains(c))
				return;

			Contents.Add(c);
			SetDockState();
			if (!c.IsHidden && CountOfVisibleContents == 1)
			{
				ActiveContent = c;
				Refresh();
			}
		}

		public void Close()
		{
			Dispose();
		}

		internal void CloseActiveContent()
		{
			Content c = ActiveContent;

			if (c.HideOnClose)
				c.Hide();
			else
				c.Close();
		}

		public bool Contains(ContentWindow contentWindow)
		{
			if (contentWindow.DockState != DockState ||
				DockState == DockState.Hidden || DockState == DockState.Unknown ||
				DockHelper.IsDockStateAutoHide(DockState))
				return false;

			if (DockState == DockState.Float)
			{
				if (FloatWindow != contentWindow.FloatWindow)
					return false;
				else
					return (FloatWindow.ContentWindows.IndexOf(contentWindow) > FloatWindow.ContentWindows.IndexOf(this));
			}
			else
				return (DockManager.ContentWindows.IndexOf(contentWindow) > DockManager.ContentWindows.IndexOf(this));
		}

		private TabWindow GetTabWindow(DockState state)
		{
			if (state == DockState.Document)
				return TabWindowDocument;
			else if (state == DockState.Float)
				return TabWindowFloat;
			else if (state != DockState.Unknown && state != DockState.Hidden)
				return TabWindowDock;
			else
				return null;
		}

		internal int GetIndexOfVisibleContents(Content content)
		{
			if (content == null)
				return -1;

			if (content.IsHidden)
				return -1;

			int index = -1;
			foreach (Content c in Contents)
			{
				if (!c.IsHidden)
					index++;

				if (c == content)
					return index;
			}
			return -1;
		}

		internal Content GetVisibleContent(int index)
		{
			int currentIndex = -1;
			foreach (Content c in Contents)
			{
				if (!c.IsHidden)
					currentIndex ++;

				if (currentIndex == index)
					return c;
			}
			throw(new ArgumentOutOfRangeException());
		}

		public void Hide()
		{
			IsHidden = true;
		}

		public void PerformLayout()
		{
			TabWindow.PerformLayout();
		}

		public void Refresh()
		{
			if (DockState == DockState.Unknown || DockState == DockState.Hidden)
				return;

			if (IsAutoHide)
				DockManager.Invalidate();

			if (TabWindow != null)
			{
				TabWindow.Invalidate();
				PerformLayout();
				if (TabWindow is DockWindow)
					((DockWindow)TabWindow).OnAutoHide(DockManager.ActiveAutoHideContent == ActiveContent);
			}
		}

		internal void RemoveContent(Content c)
		{
			if (!Contents.Contains(c))
				return;
			
			int index = GetIndexOfVisibleContents(c);

			Contents.Remove(c);
			if (TabWindow != null)
				if (TabWindow.Contains(c))
					c.SetParent(null);

			SetDockState();

			ValidateActiveContent();

			if (Contents.Count == 0)
				Close();

			if (index != -1)
				Refresh();
		}

		public void SetContentIndex(Content c, int index)
		{
			int oldIndex = Contents.IndexOf(c);
			if (oldIndex == -1)
				throw(new ArgumentException(ResourceHelper.GetString("ContentWindow.SetContentIndex.InvalidContent")));

			if (index < 0 || index > Contents.Count - 1)
				if (index != -1)
					throw(new ArgumentOutOfRangeException(ResourceHelper.GetString("ContentWindow.SetContentIndex.InvalidIndex")));
				
			if (oldIndex == index)
				return;
			if (oldIndex == Contents.Count - 1 && index == -1)
				return;

			Contents.Remove(c);
			if (index == -1)
				Contents.Add(c);
			else if (oldIndex < index)
				Contents.AddAt(c, index - 1);
			else
				Contents.AddAt(c, index);

			Refresh();
		}

		internal void SetParent(Control control)
		{
			TabWindow tabWindow = TabWindow;
			if (TabWindowDock != null)
				TabWindowDock.SetParent(null);
			if (TabWindowDocument != null)
				TabWindowDocument.SetParent(null);
			if (TabWindowFloat != null)
				TabWindowFloat.SetParent(null);

			if (tabWindow != null)
				tabWindow.SetParent(control);
		}

		public void Show()
		{
			IsHidden = false;
		}

		internal void ValidateActiveContent()
		{
			if (ActiveContent == null)
				return;

			if (GetIndexOfVisibleContents(ActiveContent) >= 0)
				return;

			Content prevVisible = null;
			for (int i=Contents.IndexOf(ActiveContent)-1; i>=0; i--)
				if (!Contents[i].IsHidden)
				{
					prevVisible = Contents[i];
					break;
				}

			Content nextVisible = null;
			for (int i=Contents.IndexOf(ActiveContent)+1; i<Contents.Count; i++)
				if (!Contents[i].IsHidden)
				{
					nextVisible = Contents[i];
					break;
				}

			if (prevVisible != null)
				ActiveContent = prevVisible;
			else if (nextVisible != null)
				ActiveContent = nextVisible;
			else
				ActiveContent = null;
		}

		private static readonly object DockStateChangedEvent = new object();
		public event EventHandler DockStateChanged
		{
			add	{	Events.AddHandler(DockStateChangedEvent, value);	}
			remove	{	Events.RemoveHandler(DockStateChangedEvent, value);	}
		}
		protected virtual void OnDockStateChanged(EventArgs e)
		{
			EventHandler handler = (EventHandler)Events[DockStateChangedEvent];
			if (handler != null)
				handler(this, e);
		}
	}
}
