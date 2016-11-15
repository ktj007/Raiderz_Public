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
using System.Runtime.InteropServices;

namespace WeifenLuo.WinFormsUI
{
	public class Content : Form
	{
		internal class DummyContent : Content
		{
		}

		// Tab width and X position used by ContentWindow and DockManager class
		internal int TabWidth = 0;
		internal int TabX = 0;

		public Content()
		{
			RefreshMdiIntegration();
		}

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (m_hiddenMdiChild != null)
				{
					m_hiddenMdiChild.Close();
					m_hiddenMdiChild = null;
				}

				DockManager = null;
			}

			base.Dispose(disposing);
		}

		private bool m_allowRedocking = true;
		[LocalizedCategory("Category.Docking")]
		[LocalizedDescription("Content.AllowRedocking.Description")]
		[DefaultValue(true)]
		public bool AllowRedocking
		{
			get	{	return m_allowRedocking;	}
			set	{	m_allowRedocking = value;	}
		}

		private ContentStates m_allowedStates = ContentStates.DockLeft | ContentStates.DockRight | ContentStates.DockTop | ContentStates.DockBottom | ContentStates.Document | ContentStates.Float;
		[LocalizedCategory("Category.Docking")]
		[LocalizedDescription("Content.AllowedStates.Description")]
		[DefaultValue(ContentStates.DockLeft|ContentStates.DockRight|ContentStates.DockTop|ContentStates.DockBottom|ContentStates.Document|ContentStates.Float)]
		public ContentStates AllowedStates
		{
			get	{	return m_allowedStates;	}
			set
			{
				if (m_allowedStates == value)
					return;

				if (!DockHelper.IsDockStateValid(DockState, value))
					throw(new InvalidOperationException(ResourceHelper.GetString("Content.AllowedStates.InvalidValue")));

				m_allowedStates = value;

				if (!DockHelper.IsDockStateValid(ShowHint, m_allowedStates))
					ShowHint = DockState.Unknown;
			}
		}

		private double m_autoHidePortion = 0.25;
		[LocalizedCategory("Category.Docking")]
		[LocalizedDescription("Content.AutoHidePortion.Description")]
		[DefaultValue(0.25)]
		public double AutoHidePortion
		{
			get	{	return m_autoHidePortion;	}
			set
			{
				if (value <= 0 || value > 1)
					throw(new ArgumentOutOfRangeException(ResourceHelper.GetString("Content.AutoHidePortion.OutOfRange")));

				if (m_autoHidePortion == value)
					return;

				m_autoHidePortion = value;

				if (DockManager == null)
					return;

				if (DockManager.ActiveAutoHideContent == this)
					DockManager.PerformLayout();
			}
		}

		private bool m_closeButton = true;
		[LocalizedCategory("Category.Docking")]
		[LocalizedDescription("Content.CloseButton.Description")]
		[DefaultValue(true)]
		public bool CloseButton
		{
			get	{	return m_closeButton;	}
			set
			{
				if (m_closeButton == value)
					return;

				m_closeButton = value;
				if (ContentWindow != null)
					if (ContentWindow.ActiveContent == this)
						ContentWindow.Refresh();
			}
		}
		
		private ContentWindow m_contentWindow = null;
		[Browsable(false)]
		public ContentWindow ContentWindow
		{
			get {	return m_contentWindow; }
			set
			{
				if (m_contentWindow == value)
					return;

				ContentWindow oldValue = m_contentWindow;
				m_contentWindow = value;

				if (oldValue != null)
					oldValue.RemoveContent(this);

				if (value != null)
					value.AddContent(this);
				
				SetDockState();
			}
		}

		private DockManager m_dockManager = null;
		[Browsable(false)]
		public DockManager DockManager
		{
			get { return m_dockManager; }
			set
			{
				if (m_dockManager == value)
					return;

				ContentWindow = null;

				if (m_dockManager != null)
					m_dockManager.RemoveContent(this);

				m_dockManager = value;

				if (m_dockManager != null)
				{
					m_dockManager.AddContent(this);
					TopLevel = false;
					FormBorderStyle = FormBorderStyle.None;
					ShowInTaskbar = false;
					Visible = true;
				}

				RefreshMdiIntegration();
			}
		}

		private DockState DefaultShowState
		{
			get
			{
				if (ShowHint != DockState.Unknown)
					return ShowHint;

				if ((AllowedStates & ContentStates.Document) != 0)
					return DockState.Document;
				if ((AllowedStates & ContentStates.DockRight) != 0)
					return DockState.DockRight;
				if ((AllowedStates & ContentStates.DockLeft) != 0)
					return DockState.DockLeft;
				if ((AllowedStates & ContentStates.DockBottom) != 0)
					return DockState.DockBottom;
				if ((AllowedStates & ContentStates.DockTop) != 0)
					return DockState.DockTop;
				if ((AllowedStates & ContentStates.Float) != 0)
					return DockState.Float;

				return DockState.Unknown;
			}
		}

		/// <summary>
		/// DockState Property
		/// </summary>
		private DockState m_dockState = DockState.Unknown;
		[Browsable(false)]
		public DockState DockState
		{
			get	{	return m_dockState;	}
		}
		private DockState GetDockState()
		{
			if (ContentWindow == null)
				return DockState.Unknown;
			else if (IsHidden)
				return DockState.Hidden;
			else
				return ContentWindow.DockState;
		}
		internal void SetDockState()
		{
			DockState value = GetDockState();
			if (DockState == value)
				return;

			m_dockState = value;
			RefreshMdiIntegration();
			OnDockStateChanged(EventArgs.Empty);
		}

		internal string PersistString
		{
			get	{	return GetPersistString();	}
		}
		protected virtual string GetPersistString()
		{
			return GetType().ToString();
		}

		internal TabWindow TabWindow
		{
			get	{	return ContentWindow == null ? null : ContentWindow.TabWindow;	}
		}

		/// <summary>
		/// HiddenMdiChild Property
		/// </summary>
		private HiddenMdiChild m_hiddenMdiChild = null;
		internal HiddenMdiChild HiddenMdiChild
		{
			get	{	return m_hiddenMdiChild;	}
		}

		/// <summary>
		/// HideOnClose Property
		/// </summary>
		private bool m_hideOnClose = false;
		[LocalizedCategory("Category.Docking")]
		[LocalizedDescription("Content.HideOnClose.Description")]
		[DefaultValue(false)]
		public bool HideOnClose
		{
			get	{	return m_hideOnClose;	}
			set	{	m_hideOnClose = value;	}
		}

		/// <summary>
		/// ShowHint Property
		/// </summary>
		private DockState m_showHint = DockState.Unknown;
		[LocalizedCategory("Category.Docking")]
		[LocalizedDescription("Content.ShowHint.Description")]
		[DefaultValue(DockState.Unknown)]
		public DockState ShowHint
		{
			get	{	return m_showHint;	}
			set
			{	
				if (m_showHint == value)
					return;

				if (DockHelper.IsDockStateValid(value, AllowedStates))
					m_showHint = value;
				else
					throw (new InvalidOperationException("Desired state not allowed, check AllowedStates property."));
			}
		}

		[Browsable(false)]
		public bool IsActivated
		{
			get
			{
				if (ContentWindow == null)
					return false;
				else
					return (ContentWindow.ActiveContent == this ? ContentWindow.IsActivated : false);
			}
		}

		private bool m_isHidden = false;
		[Browsable(false)]
		public bool IsHidden
		{
			get	{	return m_isHidden;	}
			set
			{
				if (m_isHidden == value)
					return;

				m_isHidden = value;
				Visible = !IsHidden;
				if (ContentWindow != null)
				{
					ContentWindow.SetDockState();
					ContentWindow.ValidateActiveContent();
				}
				
				if (HiddenMdiChild != null)
					HiddenMdiChild.Visible = (!IsHidden);
			}
		}

		public bool IsDockStateValid(DockState dockState)
		{
			return DockHelper.IsDockStateValid(dockState, AllowedStates);
		}

		internal void SetParent(Control value)
		{
			if (Parent == value)
				return;

			Parent = value;
		}

		public new void Activate()
		{
			if (DockManager == null)
				base.Activate();
			else if (ContentWindow == null)
				Show(DockManager);
			else
			{
				IsHidden = false;
				ContentWindow.ActiveContent = this;
				ContentWindow.Activate();
			}
		}

		public new void Hide()
		{
			IsHidden = true;
		}

		public new void Show()
		{
			if (DockManager == null)
				base.Show();
			else
				Show(DockManager);
		}

		public void Show(DockManager dockManager)
		{
			if (dockManager == null)
				throw(new ArgumentNullException(ResourceHelper.GetString("Content.Show.NullDockManager")));

			if (DockState == DockState.Unknown)
				Show(dockManager, DefaultShowState);
			else			
				Activate();
		}

		public void Show(DockManager dockManager, DockState dockState)
		{
			if (dockManager == null)
				throw(new ArgumentNullException(ResourceHelper.GetString("Content.Show.NullDockManager")));

			if (dockState == DockState.Unknown || dockState == DockState.Hidden)
				throw(new ArgumentException(ResourceHelper.GetString("Content.Show.InvalidDockState")));

			DockManager = dockManager;

			if (ContentWindow == null)
			{
				ContentWindow cwExisting = null;
				foreach (ContentWindow cw in DockManager.ContentWindows)
					if (cw.VisibleState == dockState)
					{
						cwExisting = cw;
						break;
					}

				if (cwExisting == null || dockState == DockState.Float)
					ContentWindow = new ContentWindow(this, dockState);
				else
					ContentWindow = cwExisting;
			}

			Activate();
		}

		protected override void OnTextChanged(EventArgs e)
		{
			if (m_hiddenMdiChild != null)
				m_hiddenMdiChild.Text = this.Text;
			if (ContentWindow != null)
			{
				if (ContentWindow.FloatWindow != null)
					ContentWindow.FloatWindow.SetText();
				ContentWindow.Refresh();
			}

			base.OnTextChanged(e);
		}

		internal void RefreshMdiIntegration()
		{
			Form mdiParent = GetMdiParentForm();

			if (mdiParent != null && HiddenMdiChild == null)
			{
				m_hiddenMdiChild = new HiddenMdiChild(this);
			}
			else if (mdiParent != null && HiddenMdiChild != null)
			{
				m_hiddenMdiChild.SetMdiParent(mdiParent);
			}
			else if (mdiParent == null && HiddenMdiChild != null)
			{
				m_hiddenMdiChild.Close();
				m_hiddenMdiChild = null;
			}
		}
		private Form GetMdiParentForm()
		{
			if (DockManager == null)
				return null;

			if (!DockManager.MdiIntegration)
				return null;

			if (DockState != DockState.Document)
				return null;

			Form parentMdi = DockManager.FindForm();
			if (parentMdi != null)
				if (!parentMdi.IsMdiContainer)
					parentMdi = null;

			return parentMdi;
		}

		#region Events
		private static readonly object DockStateChangedEvent = new object();
		[LocalizedCategory("Category.PropertyChanged")]
		[LocalizedDescription("Content.DockStateChanged.Description")]
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
		#endregion
	}
}
