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
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Resources;
using System.Reflection;
using System.ComponentModel;
using System.Runtime.InteropServices;

namespace WeifenLuo.WinFormsUI
{
	/// <summary>
	/// Summary description for ContentWindow.
	/// </summary>
	internal class TabWindow : Panel
	{
		private enum HitTestArea
		{
			Splitter,
			Caption,
			TabStrip,
			Content,
			None
		}

		private struct HitTestResult
		{
			public HitTestArea HitArea;
			public int Index;

			public HitTestResult(HitTestArea hitTestArea)
			{
				HitArea = hitTestArea;
				Index = -1;
			}

			public HitTestResult(HitTestArea hitTestArea, int index)
			{
				HitArea = hitTestArea;
				Index = index;
			}
		}

		private IContainer m_components;
		private ToolTip m_toolTip;
		private TabWindowSplitter m_splitter;
		private Size m_sizeFrame;	// The size of the window including non client area. This variable
									// will be set in WM_NCCALCSIZE and used in WM_NCPAINT.

		public TabWindow(ContentWindow contentWindow)
		{
			m_contentWindow = contentWindow;
			
			SetStyle(ControlStyles.ResizeRedraw, true);
			SetStyle(ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.AllPaintingInWmPaint, true);
			SetStyle(ControlStyles.DoubleBuffer, true);
			SetStyle(ControlStyles.Selectable, true);
			Font = SystemInformation.MenuFont;

			m_components = new Container();
			m_toolTip = new ToolTip(m_components);
			m_splitter = new TabWindowSplitter();
			Controls.Add(m_splitter);
		}

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (Components != null)
					Components.Dispose();
			}

			base.Dispose(disposing);
		}

		public Content ActiveContent
		{
			get	{	return ContentWindow.ActiveContent;	}
			set	{	ContentWindow.ActiveContent = value;	}
		}

		private int m_borderWidth = 0;
		public int BorderWidth
		{
			get	{	return m_borderWidth;	}
			set
			{
				if (m_borderWidth == value)
					return;

				m_borderWidth = value;
				User32.SetWindowPos(Handle, IntPtr.Zero, 0, 0, 0, 0,
					Win32.FlagsSetWindowPos.SWP_NOMOVE |
					Win32.FlagsSetWindowPos.SWP_NOSIZE |
					Win32.FlagsSetWindowPos.SWP_NOZORDER |
					Win32.FlagsSetWindowPos.SWP_DRAWFRAME);
			}
		}
		
		protected string CaptionText
		{
			get	{	return ContentWindow.ActiveContent == null ? string.Empty : ContentWindow.ActiveContent.Text;	}
		}

		protected IContainer Components
		{
			get	{	return m_components;	}
		}

		protected int CountOfVisibleContents
		{
			get	{	return ContentWindow.CountOfVisibleContents;	}
		}

		protected override Size DefaultSize
		{
			get	{	return Size.Empty;	}
		}

		public DockManager DockManager
		{
			get	{	return ContentWindow.DockManager;	}
		}

		public DockState DockState
		{
			get	{	return ContentWindow.DockState;	}
		}

		public TabWindow First
		{
			get
			{	
				ContentWindow cwFirst = ContentWindow.First;
				return cwFirst == null ? null : cwFirst.TabWindow;
			}			
		}

		public FloatWindow FloatWindow
		{
			get	{	return ContentWindow.FloatWindow;	}
		}

		public bool IsActivated
		{
			get	{	return ContentWindow.IsActivated;	}
		}

		public TabWindow Last
		{
			get	{	return ContentWindow.Last.TabWindow;	}
		}

		public double LayoutSize
		{
			get
			{	
				if (DockState == DockState.Float)
					return ContentWindow.SizeFloat;
				else if (DockState == DockState.Document)
					return ContentWindow.SizeDocument;
				else
					return ContentWindow.SizeDock;
			}
			set
			{
				if (DockState == DockState.Float)
					ContentWindow.SizeFloat = value;
				else if (DockState == DockState.Document)
					ContentWindow.SizeDocument = value;
				else
					ContentWindow.SizeDock = value;
			}
		}

		public LayoutStyles LayoutStyle
		{
			get
			{	
				if (DockState == DockState.Float)
					return ContentWindow.LayoutFloat;
				else if (DockState == DockState.Document)
					return ContentWindow.LayoutDocument;
				else
					return ContentWindow.LayoutDock;
			}
			set
			{
				if (DockState == DockState.Float)
					ContentWindow.LayoutFloat = value;
				else if (DockState == DockState.Document)
					ContentWindow.LayoutDocument = value;
				else
					ContentWindow.LayoutDock = value;
			}
		}

		public TabWindow Next
		{
			get
			{
				ContentWindow cwNext = ContentWindow.Next;
				return (cwNext == null ? null : cwNext.TabWindow);
			}
		}

		public TabWindow Previous
		{
			get
			{
				ContentWindow cwPrev = ContentWindow.Previous;
				return (cwPrev == null ? null : cwPrev.TabWindow);
			}
		}

		protected virtual void Close()
		{
			Dispose();
		}

		private ContentWindow m_contentWindow;
		internal ContentWindow ContentWindow
		{
			get	{	return m_contentWindow;	}
		}

		private ContentCollection Contents
		{
			get	{	return ContentWindow.Contents;	}
		}

		public bool Contains(TabWindow tabWindow)
		{
			return (tabWindow == null ? false : ContentWindow.Contains(tabWindow.ContentWindow));
		}

		public virtual void EnsureTabVisible(Content content)
		{
		}

		protected int GetIndexOfVisibleContents(Content c)
		{
			return ContentWindow.GetIndexOfVisibleContents(c);
		}

		protected Content GetVisibleContent(int index)
		{
			return ContentWindow.GetVisibleContent(index);
		}

		public bool IsTopLevel
		{
			get
			{
				if (Previous != null)
					if (Previous.Contains(this))
						return false;

				return true;
			}
		}

		protected override void OnLeave(EventArgs e)
		{
			base.OnLeave(e);
			ContentWindow.IsActivated = false;
		}

		protected override void OnLayout(LayoutEventArgs e)
		{
			if (ContentWindow.TabWindow != this)
				return;

			m_splitter.Dock = SplitterDockStyle;

			if (Contains(Next))
			{
				Next.Dock = DockStyle.None;
				Next.Bounds = NextTabWindowBounds;
			}

			Rectangle rectContent = ContentRectangle;

			foreach (Content c in Contents)
			{
				c.Parent = this;
				c.Visible = (c == ActiveContent);
				c.Bounds = rectContent;
			}
			base.OnLayout(e);
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown(e);

			if (e.Button == MouseButtons.Left)
			{
				HitTestResult hitTestResult = GetHitTest();
				if (hitTestResult.HitArea == HitTestArea.TabStrip && hitTestResult.Index != -1)
				{
					Content c = GetVisibleContent(hitTestResult.Index);
					if (ActiveContent != c)
					{
						ActiveContent = c;
						this.Update();
					}
					if (DockManager.AllowRedocking && ContentWindow.AllowRedocking && ActiveContent.AllowRedocking)
						DockManager.DragHandler.BeginDragContent(this, TabWindowRectangle);
				}
				else if (hitTestResult.HitArea == HitTestArea.Caption && ContentWindow.TabWindow == this &&
					DockManager.AllowRedocking && ContentWindow.AllowRedocking &&
					(DockState == DockState.DockLeft || DockState == DockState.DockRight ||
					DockState == DockState.DockTop || DockState == DockState.DockBottom ||
					DockState == DockState.Float))
					DockManager.DragHandler.BeginDragContentWindow(this, CaptionRectangle.Location);
			}
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			HitTestResult hitTestResult = GetHitTest();
			HitTestArea hitArea = hitTestResult.HitArea;
			int index = hitTestResult.Index;
			string toolTip = string.Empty;

			base.OnMouseMove(e);

			if (hitArea == HitTestArea.TabStrip && index != -1)
			{
				Rectangle rectTab = GetTabRectangle(index);
				if (rectTab.Width < GetTabOriginalWidth(index))
					toolTip = GetVisibleContent(index).Text;
			}

			if (m_toolTip.GetToolTip(this) != toolTip)
			{
				m_toolTip.Active = false;
				m_toolTip.SetToolTip(this, toolTip);
				m_toolTip.Active = true;
			}
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint(e);

			DrawCaption(e.Graphics);
			DrawTabStrip(e.Graphics);
			PerformLayout();
		}

		protected override void WndProc(ref Message m)
		{
			if (m.Msg == (int)Win32.Msgs.WM_NCCALCSIZE)
			{
				Win32.RECT rect = (Win32.RECT)m.GetLParam(typeof(Win32.RECT));
				m_sizeFrame = new Size(rect.right-rect.left, rect.bottom-rect.top);
				base.WndProc(ref m);
				rect.left += BorderWidth;
				rect.right -= BorderWidth;
				rect.top += BorderWidth;
				rect.bottom -= BorderWidth;
				Marshal.StructureToPtr(rect, m.LParam, true);
				return;
			}
			else if (m.Msg == (int)Win32.Msgs.WM_NCPAINT)
			{
				if (BorderWidth <= 0)
					return;

				IntPtr hDC = User32.GetWindowDC(m.HWnd);
				Graphics g = Graphics.FromHdc(hDC);
				Rectangle rectFrame = new Rectangle(0, 0, m_sizeFrame.Width, m_sizeFrame.Height);
				using (Region regionBorder = new Region(rectFrame))
				{
					regionBorder.Xor(Rectangle.Inflate(rectFrame, -BorderWidth, -BorderWidth));
					g.FillRegion(SystemBrushes.ControlDark, regionBorder);
				}
				User32.ReleaseDC(m.HWnd, hDC);
				return;
			}
			else if (m.Msg == (int)Win32.Msgs.WM_MOUSEACTIVATE)
			{
				if (!ContentWindow.FlagMouseActivate)
				{
					ContentWindow.FlagMouseActivate = true;
					base.WndProc(ref m);
					ContentWindow.FlagMouseActivate = false;
					ContentWindow.Activate();
				}
				else
					base.WndProc(ref m);
				return;
			}
			else if (m.Msg == (int)Win32.Msgs.WM_LBUTTONDBLCLK)
			{
				base.WndProc(ref m);

				HitTestResult hitTestResult = GetHitTest();
				if (hitTestResult.HitArea != HitTestArea.Caption)
					return;

				if (DockHelper.IsDockStateAutoHide(DockState))
				{
					DockManager.ActiveAutoHideContent = null;
					return;
				}

				if (DockHelper.IsDockStateDocked(DockState) && ContentWindow.IsDockStateValid(DockState.Float))
					ContentWindow.VisibleState = DockState.Float;
				else if (DockState == DockState.Float && ContentWindow.RestoreState != DockState.Unknown &&
					ContentWindow.IsDockStateValid(ContentWindow.RestoreState))
					ContentWindow.VisibleState = ContentWindow.RestoreState;

				return;
			}

			base.WndProc(ref m);
			return;
		}

		protected virtual int GetTabOriginalWidth(int index)
		{
			return 0;
		}

		protected virtual Rectangle GetTabRectangle(int index)
		{
			return Rectangle.Empty;
		}

		protected virtual void DrawCaption(Graphics g)
		{
		}

		protected virtual void DrawTabStrip(Graphics g)
		{
		}

		protected virtual Rectangle CaptionRectangle
		{
			get	{	return Rectangle.Empty;	}
		}

		protected Rectangle ContentRectangle
		{
			get
			{
				Rectangle rectTabWindow = TabWindowRectangle;
				Rectangle rectCaption = CaptionRectangle;
				Rectangle rectTabStrip = GetTabStripRectangle();

				int x = rectTabWindow.X;
				int y = rectTabWindow.Y + (rectCaption.IsEmpty ? 0 : rectCaption.Height) +
					(DockState == DockState.Document ? rectTabStrip.Height : 0);
				int width = rectTabWindow.Width;
				int height = rectTabWindow.Height - rectCaption.Height - rectTabStrip.Height;

				return new Rectangle(x, y, width, height);
			}
		}

		internal virtual bool HasCaption
		{
			get	{	return false;	}
			set	{	}
		}

		private Rectangle NextTabWindowBounds
		{
			get
			{
				if (!Contains(Next))
					return Rectangle.Empty;

				Rectangle rectClient = ClientRectangle;
				Rectangle rectTabWindow = TabWindowRectangle;
				Rectangle rect = Rectangle.Empty;

				if (IsTopLevel && LayoutStyle == LayoutStyles.Horizontal &&
					(SplitterDockStyle == DockStyle.Left || SplitterDockStyle == DockStyle.Right))
					rectClient.Width -= MeasureContentWindow.DragSize;

				if (IsTopLevel && LayoutStyle == LayoutStyles.Vertical &&
					(SplitterDockStyle == DockStyle.Top || SplitterDockStyle == DockStyle.Bottom))
					rectClient.Height -= MeasureContentWindow.DragSize;

				if (LayoutStyle == LayoutStyles.Horizontal)
				{
					rect.X = rectTabWindow.X + rectTabWindow.Width;
					rect.Y = rectTabWindow.Y;
					rect.Width = rectClient.Width - rectTabWindow.Width;
					rect.Height = rectTabWindow.Height;
				}
				else
				{
					rect.X = rectTabWindow.X;
					rect.Y = rectTabWindow.Y + rectTabWindow.Height;
					rect.Width = rectTabWindow.Width;
					rect.Height = rectClient.Height - rectTabWindow.Height;
				}

				return rect;
			}
		}

		public Rectangle TabStripRectangle
		{
			get	{	return GetTabStripRectangle();	}
		}

		protected Rectangle TabWindowRectangle
		{
			get
			{
				Rectangle rect = ClientRectangle;

				if (Contains(Next))
				{
					double sizeLayout = ContentWindow.SizeLayout;

					if (LayoutStyle == LayoutStyles.Horizontal)
						rect.Width = (int)(ClientRectangle.Width * sizeLayout);
					else if (LayoutStyle == LayoutStyles.Vertical)
						rect.Height = (int)(ClientRectangle.Height * sizeLayout);
				}

				DockStyle splitterDockStyle = SplitterDockStyle;
				int splitterSize = MeasureContentWindow.DragSize;
				if (splitterDockStyle == DockStyle.Left)
					rect.X += splitterSize;
				else if (splitterDockStyle == DockStyle.Top)
					rect.Y += splitterSize;
				
				if (splitterDockStyle == DockStyle.Right || splitterDockStyle == DockStyle.Left)
					rect.Width -= splitterSize;
				else if (splitterDockStyle == DockStyle.Bottom || splitterDockStyle == DockStyle.Top)
					rect.Height -= splitterSize;

				return rect;
			}
		}

		internal DockStyle SplitterDockStyle
		{
			get
			{
				DockStyle value;
				if (IsTopLevel)
				{
					if (DockState == DockState.DockLeft || DockState == DockState.DockLeftAutoHide)
						value = DockStyle.Right;
					else if (DockState == DockState.DockRight || DockState == DockState.DockRightAutoHide)
						value = DockStyle.Left;
					else if (DockState == DockState.DockTop || DockState == DockState.DockTopAutoHide)
						value = DockStyle.Bottom;
					else if (DockState == DockState.DockBottom || DockState == DockState.DockBottomAutoHide)
						value = DockStyle.Top;
					else
						value = DockStyle.None;
				}
				else
				{
					LayoutStyles layoutStyle = Previous.LayoutStyle;
					if (layoutStyle == LayoutStyles.Horizontal)
						value = DockStyle.Left;
					else if (layoutStyle == LayoutStyles.Vertical)
						value = DockStyle.Top;
					else
						value = DockStyle.None;
				}

				return value;
			}
		}

		internal Point SplitterLocation
		{
			get
			{
				DockStyle splitterDockStyle = SplitterDockStyle;
			
				Point pt;
				if (splitterDockStyle == DockStyle.Left || splitterDockStyle == DockStyle.Top)
					pt = new Point(0, 0);
				if (splitterDockStyle == DockStyle.Right)
					pt = new Point(ClientRectangle.Width - MeasureContentWindow.DragSize, 0);
				else if (splitterDockStyle == DockStyle.Bottom)
					pt = new Point(0, ClientRectangle.Height - MeasureContentWindow.DragSize);
				else
					pt = Point.Empty;

				return pt;
			}
		}

		private HitTestResult GetHitTest()
		{
			return GetHitTest(Control.MousePosition);
		}

		private HitTestResult GetHitTest(Point ptMouse)
		{
			HitTestResult hitTestResult = new HitTestResult(HitTestArea.None, -1);

			ptMouse = PointToClient(ptMouse);

			if (m_splitter.Bounds.Contains(ptMouse))
				return new HitTestResult(HitTestArea.Splitter, -1);

			Rectangle rectCaption = CaptionRectangle;
			if (rectCaption.Contains(ptMouse))
				return new HitTestResult(HitTestArea.Caption, -1);

			Rectangle rectContent = CaptionRectangle;
			if (rectContent.Contains(ptMouse))
				return new HitTestResult(HitTestArea.Content, -1);

			Rectangle rectTabStrip = GetTabStripRectangle(true);
			if (rectTabStrip.Contains(ptMouse))
			{
				for (int i=0; i<CountOfVisibleContents; i++)
				{
					Rectangle rectTab = GetTabRectangle(i);
					rectTab.Intersect(rectTabStrip);
					if (rectTab.Contains(ptMouse))
						return new HitTestResult(HitTestArea.TabStrip, i);
				}
				return new HitTestResult(HitTestArea.TabStrip, -1);
			}

			return new HitTestResult(HitTestArea.None, -1);
		}

		protected virtual Rectangle GetTabStripRectangle()
		{
			return Rectangle.Empty;
		}

		protected virtual Rectangle GetTabStripRectangle(bool tabOnly)
		{
			return Rectangle.Empty;
		}

		internal void InsertContentWindowAfter(ContentWindow cw, LayoutStyles layoutStyle, double layoutSize)
		{
			if (DockState == DockState.Float)
			{
				cw.FloatWindow = FloatWindow;
				int index = FloatWindow.ContentWindows.IndexOf(ContentWindow);
				if (index == FloatWindow.ContentWindows.Count - 1)
					FloatWindow.SetContentWindowIndex(cw, -1);
				else
					FloatWindow.SetContentWindowIndex(cw, index + 1);
			}
			else
			{
				int index = DockManager.ContentWindows.IndexOf(ContentWindow);
				if (index == DockManager.ContentWindows.Count - 1)
					DockManager.SetContentWindowIndex(cw, -1);
				else
					DockManager.SetContentWindowIndex(cw, index + 1);
			}
			cw.VisibleState = DockState;
			
			cw.TabWindow.LayoutStyle = LayoutStyle;
			if (cw.Next != null)
			{
				if (layoutStyle == cw.TabWindow.LayoutStyle &&  layoutStyle == cw.Next.TabWindow.LayoutStyle)
					cw.TabWindow.LayoutSize = (1 - LayoutSize) * layoutSize;
				else
					cw.TabWindow.LayoutSize = LayoutSize;
			}

			LayoutStyle = layoutStyle;
			if (cw.Next != null)
			{
				if (LayoutStyle == cw.TabWindow.LayoutStyle && LayoutStyle == cw.Next.TabWindow.LayoutStyle)
					LayoutSize *= (1 - layoutSize);
				else
					LayoutSize = 1 - layoutSize;
			}
			else
				LayoutSize = 1 - layoutSize;

			ContentWindow.Refresh();
			cw.Refresh();
		}

		internal void InsertContentWindowBefore(ContentWindow cw, LayoutStyles layoutStyle, double layoutSize)
		{
			if (DockState == DockState.Float)
			{
				cw.FloatWindow = FloatWindow;
				int index = FloatWindow.ContentWindows.IndexOf(ContentWindow);
				FloatWindow.SetContentWindowIndex(cw, index);
			}
			else
			{
				int index = DockManager.ContentWindows.IndexOf(ContentWindow);
				DockManager.SetContentWindowIndex(cw, index);
			}
			cw.VisibleState = DockState;

			cw.TabWindow.LayoutStyle = layoutStyle;
			if (Next != null && layoutStyle == LayoutStyle)
			{
				cw.TabWindow.LayoutSize = (1 - LayoutSize) * layoutSize;
				LayoutSize *= (1 - layoutSize);
			}
			else
				cw.TabWindow.LayoutSize = layoutSize;

			cw.Refresh();
			ContentWindow.Refresh();
		}

		internal void SetParent(Control value)
		{
			if (Parent == value)
				return;

			Control oldParent = Parent;

			///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			/// Workaround for .Net Framework bug: removing control from Form may cause form
			/// unclosable. Set focus to another dummy control.
			///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if (ContainsFocus)
			{
				if (FindForm() is FloatWindow)
					((FloatWindow)FindForm()).DummyControl.Focus();
				else if (DockManager != null)
					DockManager.DummyControl.Focus();
			}
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			Parent = value;

			if (oldParent != null)
				if (oldParent is TabWindow)
					oldParent.Invalidate();
			if (value != null)
				if (value is TabWindow)
					value.Invalidate();
		}

		internal virtual void TestDrop(DragHandler dragHandler, Point pt)
		{
			if (DockState == DockState.Document)
				DockManager.TestDrop(dragHandler, pt);

			if (dragHandler.DropTarget.DropTo != null)
				return;

			if (DockHelper.IsDockStateAutoHide(DockState))
				return;

			if (!dragHandler.IsDockStateValid(DockState))
				return;

			if (dragHandler.DragSource == DragHandler.DragSources.FloatWindow &&
				ContentWindow.FloatWindow == dragHandler.DragControl)
				return;

			if (dragHandler.DragSource == DragHandler.DragSources.ContentWindow &&
				dragHandler.DragControl == this)
				return;

			if (dragHandler.DragSource == DragHandler.DragSources.Content && 
				dragHandler.DragControl == this &&
				DockState == DockState.Document &&
				CountOfVisibleContents == 1)
				return;

			Point ptClient = PointToClient(pt);
			Rectangle rectTabWindow = TabWindowRectangle;
			int dragSize = MeasureContentWindow.DragSize;
			if (ptClient.Y - rectTabWindow.Top >= 0 && ptClient.Y - rectTabWindow.Top < dragSize)
				dragHandler.DropTarget.SetDropTarget(this, DockStyle.Top);
			else if (rectTabWindow.Bottom - ptClient.Y >= 0 && rectTabWindow.Bottom - ptClient.Y < dragSize)
				dragHandler.DropTarget.SetDropTarget(this, DockStyle.Bottom);
			else if (rectTabWindow.Right - ptClient.X >= 0 && rectTabWindow.Right - ptClient.X < dragSize)
				dragHandler.DropTarget.SetDropTarget(this, DockStyle.Right);
			else if (ptClient.X - rectTabWindow.Left >= 0 && ptClient.X - rectTabWindow.Left < dragSize)
				dragHandler.DropTarget.SetDropTarget(this, DockStyle.Left);
			else
			{
				if (rectTabWindow.Height <= GetTabStripRectangle().Height)
					return;

				HitTestResult hitTestResult = GetHitTest(pt);
				if (hitTestResult.HitArea == HitTestArea.Caption)
					dragHandler.DropTarget.SetDropTarget(this, -1);
				else if (hitTestResult.HitArea == HitTestArea.TabStrip && hitTestResult.Index != -1)
					dragHandler.DropTarget.SetDropTarget(this, hitTestResult.Index);
				else if (DockState == DockState.Float && !HasCaption &&
					((ptClient.Y - rectTabWindow.Top >= dragSize && ptClient.Y - rectTabWindow.Top < 2 * dragSize) ||
					(rectTabWindow.Bottom - ptClient.Y >= dragSize && rectTabWindow.Bottom - ptClient.Y < 2 * dragSize) ||
					(rectTabWindow.Right - ptClient.X >= dragSize && rectTabWindow.Right - ptClient.X < 2 * dragSize) ||
					(ptClient.X - rectTabWindow.Left >= dragSize && ptClient.X - rectTabWindow.Left < 2 * dragSize)))
					dragHandler.DropTarget.SetDropTarget(this, -1);
				else
					return;
			}

			if (dragHandler.DropTarget.SameAsOldValue)
				return;

			dragHandler.DragFrame = GetTestDropDragFrame(dragHandler.DropTarget.Dock, dragHandler.DropTarget.ContentIndex);
		}
		protected virtual Region GetTestDropDragFrame(DockStyle dockStyle, int contentIndex)
		{
			return null;
		}
	}
}
