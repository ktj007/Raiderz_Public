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
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.ComponentModel;
using System.IO;
using System.Text;
using System.Xml;

namespace WeifenLuo.WinFormsUI
{
	public delegate Content GetContentCallback(string persistString);

	public class DockManager : Panel
	{
		private const string ConfigFormatVersion = "1.2";

		private static StringFormat StringFormatTabHorizontal;
		private static StringFormat StringFormatTabVertical;
		private static Matrix MatrixIdentity;
		private static DockState[] AutoHideDockStates;

		static DockManager()
		{
			StringFormatTabHorizontal = new StringFormat();
			StringFormatTabHorizontal.Alignment = StringAlignment.Near;
			StringFormatTabHorizontal.LineAlignment = StringAlignment.Center;
			StringFormatTabHorizontal.FormatFlags = StringFormatFlags.NoWrap;

			StringFormatTabVertical = new StringFormat();
			StringFormatTabVertical.Alignment = StringAlignment.Near;
			StringFormatTabVertical.LineAlignment = StringAlignment.Center;
			StringFormatTabVertical.FormatFlags = StringFormatFlags.NoWrap | StringFormatFlags.DirectionVertical;

			MatrixIdentity = new Matrix();

			AutoHideDockStates = new DockState[4];
			AutoHideDockStates[0] = DockState.DockLeftAutoHide;
			AutoHideDockStates[1] = DockState.DockRightAutoHide;
			AutoHideDockStates[2] = DockState.DockTopAutoHide;
			AutoHideDockStates[3] = DockState.DockBottomAutoHide;
		}

		public DockManager()
		{
			m_contentWindows = new ContentWindowCollection();
			m_floatWindows = new FloatWindowCollection();

			SetStyle(ControlStyles.ResizeRedraw |
				ControlStyles.UserPaint |
				ControlStyles.AllPaintingInWmPaint |
				ControlStyles.DoubleBuffer, true);

			m_autoHideWindow = new Control();
			m_autoHideWindow.Visible = false;
			Controls.Add(m_autoHideWindow);

			m_dummyControl = new Control();
			m_dummyControl.Bounds = Rectangle.Empty;
			Controls.Add(m_dummyControl);
		}

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				FloatWindows.Dispose();
				ContentWindows.Dispose();
			}
			base.Dispose(disposing);
		}

		private Content m_activeAutoHideContent = null;
		[Browsable(false)]
		public Content ActiveAutoHideContent
		{
			get	{	return m_activeAutoHideContent;	}
			set
			{
				if (value == m_activeAutoHideContent)
					return;

				if (value != null)
				{
					if (!DockHelper.IsDockStateAutoHide(value.DockState) || value.DockManager != this)
						throw(new InvalidOperationException(ResourceHelper.GetString("DockManager.ActiveAutoHideContent.InvalidValue")));
				}

				if (m_activeAutoHideContent != null)
				{
					DockWindow dw = m_activeAutoHideContent.TabWindow as DockWindow;
					if (dw != null)
						dw.AnimateHide();
					else
						AutoHideWindow.Hide();
				}

				Content oldValue = m_activeAutoHideContent;
				m_activeAutoHideContent = value;

				if (oldValue != null)
					if (oldValue.ContentWindow != null)
						oldValue.ContentWindow.SetParent(null);
				if (value != null)
					if (value.ContentWindow != null)
						value.ContentWindow.SetParent(AutoHideWindow);

				if (m_activeAutoHideContent != null)
					((DockWindow)(m_activeAutoHideContent.TabWindow)).AnimateShow();

				Invalidate();
			}
		}

		private Content m_activeDocument = null;
		[Browsable(false)]
		public Content ActiveDocument
		{
			get	{	return m_activeDocument;	}
		}
		internal void SetActiveDocument(Content value)
		{
			if (m_activeDocument == value)
				return;

			m_activeDocument = value;

			if (m_activeDocument != null)
				if (m_activeDocument.HiddenMdiChild != null)
					m_activeDocument.HiddenMdiChild.Activate();

			OnActiveDocumentChanged(EventArgs.Empty);
		}

		private ContentWindow m_activeDocumentWindow = null;
		[Browsable(false)]
		public ContentWindow ActiveDocumentWindow
		{
			get	{	return m_activeDocumentWindow;	}
		}
		internal void SetActiveDocumentWindow(ContentWindow value)
		{
			if (m_activeDocumentWindow == value)
				return;

			ContentWindow lastActive = m_activeDocumentWindow;
			m_activeDocumentWindow = value;
			SetActiveDocument(value != null ? value.ActiveContent : null);
			if (lastActive != null)
				lastActive.IsActivated = false;
		}

		private bool m_allowRedocking = true;
		[LocalizedCategory("Category.Docking")]
		[LocalizedDescription("DockManager.AllowRedocking.Description")]
		[DefaultValue(true)]
		public bool AllowRedocking
		{
			get	{	return m_allowRedocking;	}
			set	{	m_allowRedocking = value;	}
		}

		private bool m_animateAutoHide = true;
		internal bool AnimateAutoHide
		{
			get	{	return m_animateAutoHide;	}
			set	{	m_animateAutoHide = value;	}
		}

		private Control m_autoHideWindow;
		internal Control AutoHideWindow
		{
			get	{	return m_autoHideWindow;	}
		}

		private ContentCollection m_contents = new ContentCollection();
		[Browsable(false)]
		public ContentCollection Contents
		{
			get	{	return m_contents;	}
		}

		private ContentWindowCollection m_contentWindows;
		[Browsable(false)]
		public ContentWindowCollection ContentWindows
		{
			get	{	return m_contentWindows;	}
		}

		internal Rectangle DockArea
		{
			get
			{
				return new Rectangle(DockPadding.Left, DockPadding.Top,
					ClientRectangle.Width - DockPadding.Left - DockPadding.Right,
					ClientRectangle.Height - DockPadding.Top - DockPadding.Bottom);
			}
		}

		private double m_dockBottomPortion = 0.25;
		[LocalizedCategory("Category.Docking")]
		[LocalizedDescription("DockManager.DockBottomPortion.Description")]
		[DefaultValue(0.25)]
		public double DockBottomPortion
		{
			get	{	return m_dockBottomPortion;	}
			set
			{
				if (value <= 0 || value >= 1)
					throw new ArgumentOutOfRangeException();

				if (value == m_dockBottomPortion)
					return;

				m_dockBottomPortion = value;

				if (m_dockTopPortion + m_dockBottomPortion > 1)
					m_dockTopPortion = 1 - m_dockBottomPortion;

				PerformLayout();
			}
		}

		private double m_dockLeftPortion = 0.25;
		[LocalizedCategory("Category.Docking")]
		[LocalizedDescription("DockManager.DockLeftPortion.Description")]
		[DefaultValue(0.25)]
		public double DockLeftPortion
		{
			get	{	return m_dockLeftPortion;	}
			set
			{
				if (value <= 0 || value >= 1)
					throw new ArgumentOutOfRangeException();

				if (value == m_dockLeftPortion)
					return;

				m_dockLeftPortion = value;

				if (m_dockLeftPortion + m_dockRightPortion > 1)
					m_dockRightPortion = 1 - m_dockLeftPortion;
				
				PerformLayout();
			}
		}

		private double m_dockRightPortion = 0.25;
		[LocalizedCategory("Category.Docking")]
		[LocalizedDescription("DockManager.DockRightPortion.Description")]
		[DefaultValue(0.25)]
		public double DockRightPortion
		{
			get	{	return m_dockRightPortion;	}
			set
			{
				if (value <= 0 || value >= 1)
					throw new ArgumentOutOfRangeException();

				if (value == m_dockRightPortion)
					return;

				m_dockRightPortion = value;

				if (m_dockLeftPortion + m_dockRightPortion > 1)
					m_dockLeftPortion = 1 - m_dockRightPortion;

				PerformLayout();
			}
		}

		private double m_dockTopPortion = 0.25;
		[LocalizedCategory("Category.Docking")]
		[LocalizedDescription("DockManager.DockTopPortion.Description")]
		[DefaultValue(0.25)]
		public double DockTopPortion
		{
			get	{	return m_dockTopPortion;	}
			set
			{
				if (value <= 0 || value >= 1)
					throw new ArgumentOutOfRangeException();

				if (value == m_dockTopPortion)
					return;

				m_dockTopPortion = value;

				if (m_dockTopPortion + m_dockBottomPortion > 1)
					m_dockBottomPortion = 1 - m_dockTopPortion;

				PerformLayout();
			}
		}

		[Browsable(false)]
		protected Rectangle DocumentRectangle
		{
			get
			{
				Rectangle rect = DockArea;
				if (GetContentWindow(DockState.DockLeft) != null)
				{
					rect.X += (int)(DockArea.Width * DockLeftPortion);
					rect.Width -= (int)(DockArea.Width * DockLeftPortion);
				}
				if (GetContentWindow(DockState.DockRight) != null)
					rect.Width -= (int)(DockArea.Width * DockRightPortion);
				if (GetContentWindow(DockState.DockTop) != null)
				{
					rect.Y += (int)(DockArea.Height * DockTopPortion);
					rect.Height -= (int)(DockArea.Height * DockTopPortion);
				}
				if (GetContentWindow(DockState.DockBottom) != null)
					rect.Height -= (int)(DockArea.Height * DockBottomPortion);

				return rect;
			}
		}

		[Browsable(false)]
		public Content[] Documents
		{
			get	{	return Contents.Select(ContentStates.Document);	}
		}

		private DragHandler m_dragHandler = new DragHandler();
		internal DragHandler DragHandler
		{
			get	{	return m_dragHandler;	}
		}

		private Control m_dummyControl;
		internal Control DummyControl
		{
			get	{	return m_dummyControl;	}
		}

		private FloatWindowCollection m_floatWindows;
		[Browsable(false)]
		public FloatWindowCollection FloatWindows
		{
			get	{	return m_floatWindows;	}
		}

		private bool m_mdiIntegration = true;
		[LocalizedCategory("Category.Docking")]
		[LocalizedDescription("DockManager.MdiIntegration.Description")]
		[DefaultValue(true)]
		public bool MdiIntegration
		{
			get	{	return m_mdiIntegration;	}
			set
			{
				if (value == m_mdiIntegration)
					return;

				m_mdiIntegration = value;
				RefreshMdiIntegration();
			}
		}

		private bool m_suspendAutoHideWindowLayout = false;
		internal bool SuspendAutoHideWindowLayout
		{
			get	{	return m_suspendAutoHideWindowLayout;	}
			set	{	m_suspendAutoHideWindowLayout = value;	}
		}

		protected override void OnLayout(LayoutEventArgs e)
		{
			CalculateDockPadding();

			int width = ClientRectangle.Width - DockPadding.Left - DockPadding.Right;
			int height = ClientRectangle.Height - DockPadding.Top - DockPadding.Bottom;
			int dockLeftSize = (int)(width * m_dockLeftPortion);
			int dockRightSize = (int)(width * m_dockRightPortion);
			int dockTopSize = (int)(height * m_dockTopPortion);
			int dockBottomSize = (int)(height * m_dockBottomPortion);

			if (dockLeftSize < MeasureContentWindow.MinSize)
				dockLeftSize = MeasureContentWindow.MinSize;
			if (dockRightSize < MeasureContentWindow.MinSize)
				dockRightSize = MeasureContentWindow.MinSize;
			if (dockTopSize < MeasureContentWindow.MinSize)
				dockTopSize = MeasureContentWindow.MinSize;
			if (dockBottomSize < MeasureContentWindow.MinSize)
				dockBottomSize = MeasureContentWindow.MinSize;

			DockWindow dockLeft = GetDockWindow(DockState.DockLeft);
			DockWindow dockRight = GetDockWindow(DockState.DockRight);
			DockWindow dockTop = GetDockWindow(DockState.DockTop);
			DockWindow dockBottom = GetDockWindow(DockState.DockBottom);

			if (dockLeft != null)
			{
				dockLeft.Dock = DockStyle.Left;
				dockLeft.Width = dockLeftSize;
			}
			if (dockRight != null)
			{
				dockRight.Dock = DockStyle.Right;
				dockRight.Width = dockRightSize;
			}
			if (dockTop != null)
			{
				dockTop.Dock = DockStyle.Top;
				dockTop.Height = dockTopSize;
			}
			if (dockBottom != null)
			{
				dockBottom.Dock = DockStyle.Bottom;
				dockBottom.Height = dockBottomSize;
			}

			// Make sure auto hide window at the top of Z-order
			if (ActiveAutoHideContent != null && !SuspendAutoHideWindowLayout)
			{
				AutoHideWindow.BringToFront();
				AutoHideWindow.Bounds = GetAutoHideRectangle(true);
				if (ActiveAutoHideContent.TabWindow != null)
					ActiveAutoHideContent.TabWindow.Bounds = AutoHideWindow.ClientRectangle;
			}

			// Make sure document window at the top of Z-order of any DockWindow
			DockWindow firstDockWindow = null;
			foreach (Control c in Controls)
			{
				if (firstDockWindow == null)
					firstDockWindow = c as DockWindow;
				DocumentWindow dw = c as DocumentWindow;
				if (dw != null)
				{
					dw.Dock = DockStyle.Fill;
					if (firstDockWindow != null)
						Controls.SetChildIndex(dw, Controls.IndexOf(firstDockWindow));
					break;
				}
			}

			base.OnLayout(e);
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown(e);

			if (e.Button != MouseButtons.Left)
				return;

			Content c = GetHitTest();
			if (c == null)
				return;

			if (c != ActiveAutoHideContent)
				ActiveAutoHideContent = c;

			if (!c.ContentWindow.IsActivated)
				c.ContentWindow.Activate();
		}

		protected override void OnMouseHover(EventArgs e)
		{
			base.OnMouseHover(e);

			Content c = GetHitTest();
			if (c != null && ActiveAutoHideContent != c)
			{
				ActiveAutoHideContent = c;
				Invalidate();
			}

			// requires further tracking of mouse hover behavior,
			// call TrackMouseEvent
			Win32.TRACKMOUSEEVENTS tme = new Win32.TRACKMOUSEEVENTS(Win32.TRACKMOUSEEVENTS.TME_HOVER, Handle, Win32.TRACKMOUSEEVENTS.HOVER_DEFAULT);
			User32.TrackMouseEvent(ref tme);
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint(e);

			Graphics g = e.Graphics;
			g.FillRectangle(SystemBrushes.ControlDark, ClientRectangle);

			CalculateTabs(DockState.DockLeftAutoHide);
			CalculateTabs(DockState.DockRightAutoHide);
			CalculateTabs(DockState.DockTopAutoHide);
			CalculateTabs(DockState.DockBottomAutoHide);

			int leftAutoHideWindows = GetCountOfAutoHideWindows(DockState.DockLeftAutoHide);
			int rightAutoHideWindows = GetCountOfAutoHideWindows(DockState.DockRightAutoHide);
			int topAutoHideWindows = GetCountOfAutoHideWindows(DockState.DockTopAutoHide);
			int bottomAutoHideWindows = GetCountOfAutoHideWindows(DockState.DockBottomAutoHide);

			Brush brush = SystemBrushes.ControlLightLight;
			int height = GetTabStripHeight();
			if (leftAutoHideWindows != 0 && topAutoHideWindows != 0)
				g.FillRectangle(brush, 0, 0, height, height);
			if (leftAutoHideWindows != 0 && bottomAutoHideWindows != 0)
				g.FillRectangle(brush, 0, Height - height, height, height);
			if (topAutoHideWindows != 0 && rightAutoHideWindows != 0)
				g.FillRectangle(brush, Width - height, 0, height, height);
			if (rightAutoHideWindows != 0 && bottomAutoHideWindows != 0)
				g.FillRectangle(brush, Width - height, Height - height, height, height);
			
			DrawTabStrip(e.Graphics, DockState.DockLeftAutoHide);
			DrawTabStrip(e.Graphics, DockState.DockRightAutoHide);
			DrawTabStrip(e.Graphics, DockState.DockTopAutoHide);
			DrawTabStrip(e.Graphics, DockState.DockBottomAutoHide);

			PerformLayout();
		}

		internal void AddContent(Content c)
		{
			if (c == null)
				throw(new ArgumentNullException());

			if (!Contents.Contains(c))
			{
				Contents.Add(c);
				OnContentAdded(new ContentEventArgs(c));
			}
		}

		internal void AddContentWindow(ContentWindow contentWindow)
		{
			if (ContentWindows.Contains(contentWindow))
				return;

			ContentWindows.Add(contentWindow);
		}

		internal void AddFloatWindow(FloatWindow floatWindow)
		{
			if (FloatWindows.Contains(floatWindow))
				return;

			FloatWindows.Add(floatWindow);
		}

		private void CalculateDockPadding()
		{
			DockPadding.All = 0;

			foreach (DockState state in AutoHideDockStates)
			{
				int countAutoHideWindows = GetCountOfAutoHideWindows(state);

				if (countAutoHideWindows == 0)
					continue;

				Rectangle rectTabStrip = GetTabStripRectangle(state);

				if (state == DockState.DockLeftAutoHide)
					DockPadding.Left = rectTabStrip.Height;
				else if (state == DockState.DockRightAutoHide)
					DockPadding.Right = rectTabStrip.Height;
				else if (state == DockState.DockTopAutoHide)
					DockPadding.Top = rectTabStrip.Height;
				else if (state == DockState.DockBottomAutoHide)
					DockPadding.Bottom = rectTabStrip.Height;
			}
		}

		private void CalculateTabs(DockState dockState)
		{
			Rectangle rectTabStrip = GetTabStripRectangle(dockState);

			int imageHeight = rectTabStrip.Height - MeasureAutoHideTab.ImageGapTop -
				MeasureAutoHideTab.ImageGapBottom;
			int imageWidth = MeasureAutoHideTab.ImageWidth;
			if (imageHeight > MeasureAutoHideTab.ImageHeight)
				imageWidth = MeasureAutoHideTab.ImageWidth * (imageHeight/MeasureAutoHideTab.ImageHeight);

			int maxWidth = 0;
			using (Graphics g = this.CreateGraphics())
			{
				foreach (ContentWindow cw in ContentWindows)
				{
					if (cw.DockState != dockState)
						continue;

					foreach (Content c in cw.Contents)
					{
						if (cw.IsHidden)
							continue;

						int width = imageWidth + MeasureAutoHideTab.ImageGapLeft +
							MeasureAutoHideTab.ImageGapRight +
							(int)g.MeasureString(c.Text, GetTabFont()).Width + 1 +
							MeasureAutoHideTab.TextGapLeft + MeasureAutoHideTab.TextGapRight;
						if (width > maxWidth)
							maxWidth = width;
					}
				}
			}
			
			int x = MeasureAutoHideTab.TabGapLeft + rectTabStrip.X;
			foreach (ContentWindow cw in ContentWindows)
			{
				if (cw.DockState != dockState)
					continue;

				foreach (Content c in cw.Contents)
				{
					if (c.IsHidden)
						continue;

					c.TabX = x;
					if (c == cw.ActiveContent)
						c.TabWidth = maxWidth;
					else
						c.TabWidth = imageWidth + MeasureAutoHideTab.ImageGapLeft + MeasureAutoHideTab.ImageGapRight;
					x += c.TabWidth;
				}
				x += MeasureAutoHideTab.TabGapBetween;
			}
		}

		private void DrawTab(Graphics g, DockState dockState, ContentWindow cw, Content c)
		{
			Rectangle rectTab = GetTabRectangle(dockState, c);
			if (rectTab.IsEmpty)
				return;

			g.FillRectangle(SystemBrushes.Control, rectTab);

			g.DrawLine(SystemPens.GrayText, rectTab.Left, rectTab.Top, rectTab.Left, rectTab.Bottom);
			g.DrawLine(SystemPens.GrayText, rectTab.Right, rectTab.Top, rectTab.Right, rectTab.Bottom);
			if (dockState == DockState.DockTopAutoHide || dockState == DockState.DockRightAutoHide)
				g.DrawLine(SystemPens.GrayText, rectTab.Left, rectTab.Bottom, rectTab.Right, rectTab.Bottom);
			else
				g.DrawLine(SystemPens.GrayText, rectTab.Left, rectTab.Top, rectTab.Right, rectTab.Top);


			// Set no rotate for drawing icon and text
			Matrix matrixRotate = g.Transform;
			g.Transform = MatrixIdentity;

			// Draw the icon
			Rectangle rectImage = rectTab;
			rectImage.X += MeasureAutoHideTab.ImageGapLeft;
			rectImage.Y += MeasureAutoHideTab.ImageGapTop;
			int imageHeight = rectTab.Height - MeasureAutoHideTab.ImageGapTop -	MeasureAutoHideTab.ImageGapBottom;
			int imageWidth = MeasureAutoHideTab.ImageWidth;
			if (imageHeight > MeasureAutoHideTab.ImageHeight)
				imageWidth = MeasureAutoHideTab.ImageWidth * (imageHeight/MeasureAutoHideTab.ImageHeight);
			rectImage.Height = imageHeight;
			rectImage.Width = imageWidth;
			rectImage = GetTransformedRectangle(dockState, rectImage);
			g.DrawIcon(c.Icon, rectImage);

			// Draw the text
			if (c == cw.ActiveContent)
			{
				Rectangle rectText = rectTab;
				rectText.X += MeasureAutoHideTab.ImageGapLeft + imageWidth + MeasureAutoHideTab.ImageGapRight + MeasureAutoHideTab.TextGapLeft;
				rectText.Width -= MeasureAutoHideTab.ImageGapLeft + imageWidth + MeasureAutoHideTab.ImageGapRight + MeasureAutoHideTab.TextGapLeft;
				rectText = GetTransformedRectangle(dockState, rectText);
				if (dockState == DockState.DockLeftAutoHide || dockState == DockState.DockRightAutoHide)
					g.DrawString(c.Text, GetTabFont(), SystemBrushes.FromSystemColor(SystemColors.GrayText), rectText, StringFormatTabVertical);
				else
					g.DrawString(c.Text, GetTabFont(), SystemBrushes.FromSystemColor(SystemColors.GrayText), rectText, StringFormatTabHorizontal);
			}

			// Set rotate back
			g.Transform = matrixRotate;
		}

		private void DrawTabStrip(Graphics g, DockState dockState)
		{
			Rectangle rectTabStrip = GetTabStripRectangle(dockState);

			if (rectTabStrip.IsEmpty)
				return;

			Matrix matrixIdentity = g.Transform;
			if (dockState == DockState.DockLeftAutoHide || dockState == DockState.DockRightAutoHide)
			{
				Matrix matrixRotated = new Matrix();
				matrixRotated.RotateAt(90, new PointF((float)rectTabStrip.X + (float)rectTabStrip.Height / 2,
					(float)rectTabStrip.Y + (float)rectTabStrip.Height / 2));
				g.Transform = matrixRotated;
			}

			g.FillRectangle(SystemBrushes.ControlLightLight, rectTabStrip);

			foreach (ContentWindow cw in ContentWindows)
			{
				if (cw.DockState != dockState)
					continue;

				foreach (Content c in cw.Contents)
				{
					if (!c.IsHidden)
						DrawTab(g, dockState, cw, c);
				}
			}
			g.Transform = matrixIdentity;
		}

		internal Rectangle GetAutoHideRectangle(bool final)
		{
			if (ActiveAutoHideContent == null)
				return Rectangle.Empty;

			DockState state = ActiveAutoHideContent.DockState;
			Rectangle rectDockArea = DockArea;
			int autoHideWidth = (int)(rectDockArea.Width * ActiveAutoHideContent.AutoHidePortion);
			int autoHideHeight = (int)(rectDockArea.Height * ActiveAutoHideContent.AutoHidePortion);

			Rectangle rectTabStrip = GetTabStripRectangle(state, true);
			Rectangle rect = Rectangle.Empty;
			if (state == DockState.DockLeftAutoHide)
			{
				rect.X = rectTabStrip.X + rectTabStrip.Width;
				rect.Y = rectTabStrip.Y;
				rect.Width = (final ? autoHideWidth : 0);
				rect.Height = rectTabStrip.Height;
			}
			else if (state == DockState.DockRightAutoHide)
			{
				rect.X = rectTabStrip.X - (final ? autoHideWidth : 0);
				rect.Y = rectTabStrip.Y;
				rect.Width = (final ? autoHideWidth : 0);
				rect.Height = rectTabStrip.Height;
			}
			else if (state == DockState.DockTopAutoHide)
			{
				rect.X = rectTabStrip.X;
				rect.Y = rectTabStrip.Y + rectTabStrip.Height;
				rect.Width = rectTabStrip.Width;
				rect.Height = (final ? autoHideHeight : 0);
			}
			else
			{
				rect.X = rectTabStrip.X;
				rect.Y = rectTabStrip.Y - (final ? autoHideHeight : 0);
				rect.Width = rectTabStrip.Width;
				rect.Height = (final ? autoHideHeight : 0);
			}

			return rect;
		}

		private int GetCountOfAutoHideWindows(DockState dockState)
		{
			int result = 0;

			foreach (ContentWindow cw in ContentWindows)
			{
				if (cw.DockState == dockState)
					result ++;
			}

			return result;
		}

		private DockWindow GetDockWindow(DockState dockState)
		{
			ContentWindow first = GetContentWindow(dockState);
			return (first == null ? null : first.TabWindow as DockWindow);
		}

		private DocumentWindow GetDocumentWindow()
		{
			ContentWindow first = GetContentWindow(DockState.Document);
			return (first == null ? null : first.TabWindow as DocumentWindow);
		}

		internal ContentWindow GetContentWindow(DockState dockState)
		{
			foreach (ContentWindow cw in ContentWindows)
			{
				if (cw.DockState == dockState)
					return cw;
			}
			
			return null;
		}

		private Content GetHitTest()
		{
			Point ptMouse = PointToClient(Control.MousePosition);

			foreach(DockState state in AutoHideDockStates)
			{
				Rectangle rectTabStrip = GetTabStripRectangle(state, true);
				if (!rectTabStrip.Contains(ptMouse))
					continue;

				foreach(ContentWindow cw in ContentWindows)
				{
					if (cw.DockState != state)
						continue;

					foreach(Content c in cw.Contents)
					{
						if (c.IsHidden)
							continue;

						Rectangle rectTab = GetTabRectangle(state, c, true);
						rectTab.Intersect(rectTabStrip);
						if (rectTab.Contains(ptMouse))
							return c;
					}
				}
			}
			
			return null;
		}

		private ContentWindow GetLastContentWindow(DockState dockState)
		{
			for (int i=ContentWindows.Count - 1; i>=0; i--)
			{
				if (ContentWindows[i].DockState == dockState)
					return ContentWindows[i];
			}

			return null;
		}

		private Font GetTabFont()
		{
			return SystemInformation.MenuFont;
		}

		private Rectangle GetTabRectangle(DockState dockState, Content c)
		{
			return GetTabRectangle(dockState, c, false);
		}

		private Rectangle GetTabRectangle(DockState dockState, Content c, bool transformed)
		{
			Rectangle rectTabStrip = GetTabStripRectangle(dockState);

			if (rectTabStrip.IsEmpty)
				return Rectangle.Empty;

			int x = c.TabX;
			int y = rectTabStrip.Y + 
				(dockState == DockState.DockTopAutoHide || dockState == DockState.DockRightAutoHide ?
				0 : MeasureAutoHideTab.TabGapTop);
			int width = c.TabWidth;
			int height = rectTabStrip.Height - MeasureAutoHideTab.TabGapTop;

			if (!transformed)
				return new Rectangle(x, y, width, height);
			else
				return GetTransformedRectangle(dockState, new Rectangle(x, y, width, height));
		}

		private int GetTabStripHeight()
		{
			return Math.Max(MeasureAutoHideTab.ImageGapBottom +
				MeasureAutoHideTab.ImageGapTop + MeasureAutoHideTab.ImageHeight,
				GetTabFont().Height) + MeasureAutoHideTab.TabGapTop;
		}

		private Rectangle GetTabStripRectangle(DockState dockState)
		{
			return GetTabStripRectangle(dockState, false);
		}

		internal Rectangle GetTabStripRectangle(DockState dockState, bool transformed)
		{
			if (!DockHelper.IsDockStateAutoHide(dockState))
				return Rectangle.Empty;

			int leftAutoHideWindows = GetCountOfAutoHideWindows(DockState.DockLeftAutoHide);
			int rightAutoHideWindows = GetCountOfAutoHideWindows(DockState.DockRightAutoHide);
			int topAutoHideWindows = GetCountOfAutoHideWindows(DockState.DockTopAutoHide);
			int bottomAutoHideWindows = GetCountOfAutoHideWindows(DockState.DockBottomAutoHide);

			int x, y, width, height;

			height = GetTabStripHeight();
			if (dockState == DockState.DockLeftAutoHide)
			{
				if (leftAutoHideWindows == 0)
					return Rectangle.Empty;

				x = 0;
				y = (topAutoHideWindows == 0) ? 0 : height;
				width = Height - (topAutoHideWindows == 0 ? 0 : height) - (bottomAutoHideWindows == 0 ? 0 :height);
			}
			else if (dockState == DockState.DockRightAutoHide)
			{
				if (rightAutoHideWindows == 0)
					return Rectangle.Empty;

				x = Width - height;
				if (leftAutoHideWindows != 0 && x < height)
					x = height;
				y = (topAutoHideWindows == 0) ? 0 : height;
				width = Height - (topAutoHideWindows == 0 ? 0 : height) - (bottomAutoHideWindows == 0 ? 0 :height);
			}
			else if (dockState == DockState.DockTopAutoHide)
			{
				if (topAutoHideWindows == 0)
					return Rectangle.Empty;

				x = leftAutoHideWindows == 0 ? 0 : height;
				y = 0;
				width = Width - (leftAutoHideWindows == 0 ? 0 : height) - (rightAutoHideWindows == 0 ? 0 : height);
			}
			else
			{
				if (bottomAutoHideWindows == 0)
					return Rectangle.Empty;

				x = leftAutoHideWindows == 0 ? 0 : height;
				y = Height - height;
				if (topAutoHideWindows != 0 && y < height)
					y = height;
				width = Width - (leftAutoHideWindows == 0 ? 0 : height) - (rightAutoHideWindows == 0 ? 0 : height);
			}

			if (!transformed)
				return new Rectangle(x, y, width, height);
			else
				return GetTransformedRectangle(dockState, new Rectangle(x, y, width, height));
		}

		private Rectangle GetTransformedRectangle(DockState dockState, Rectangle rect)
		{
			if (dockState != DockState.DockLeftAutoHide && dockState != DockState.DockRightAutoHide)
				return rect;

			PointF[] pts = new PointF[1];
			// the center of the rectangle
			pts[0].X = (float)rect.X + (float)rect.Width / 2;
			pts[0].Y = (float)rect.Y + (float)rect.Height / 2;
			Rectangle rectTabStrip = GetTabStripRectangle(dockState);
			Matrix matrix = new Matrix();
			matrix.RotateAt(90, new PointF((float)rectTabStrip.X + (float)rectTabStrip.Height / 2,
				(float)rectTabStrip.Y + (float)rectTabStrip.Height / 2));
			matrix.TransformPoints(pts);

			return new Rectangle((int)(pts[0].X - (float)rect.Height / 2 + .5F),
				(int)(pts[0].Y - (float)rect.Width / 2 + .5F),
				rect.Height, rect.Width);
		}

		internal void RefreshContentWindows(DockState dockState)
		{
			if (dockState == DockState.Float)
				return;

			ContentWindow first = GetContentWindow(dockState);
			if (first == null)
				return;

			if (dockState == DockState.Hidden || dockState == DockState.Unknown)
				first.SetParent(null);
			else if (DockHelper.IsDockStateAutoHide(dockState))
				first.SetParent((ActiveAutoHideContent == first.ActiveContent) ? AutoHideWindow : null);
			else
				first.SetParent(this);

			if (first.TabWindow is DocumentWindow)
				first.TabWindow.BorderWidth = 1;

			for (ContentWindow cw = first.Next; cw != null; cw = cw.Next)
			{
				if (DockHelper.IsDockStateAutoHide(dockState))
					cw.SetParent(ActiveAutoHideContent == cw.ActiveContent ? AutoHideWindow : null);
				else if (dockState == DockState.Hidden || dockState == DockState.Unknown)
					cw.SetParent(null);
				else if (DockHelper.IsDockStateAutoHide(dockState))
					cw.SetParent((ActiveAutoHideContent == cw.ActiveContent) ? AutoHideWindow : null);
				else
					cw.SetParent(cw.Previous.TabWindow);

				if (cw.TabWindow is DocumentWindow)
					cw.TabWindow.BorderWidth = 0;
			}
		}

		public void RefreshMdiIntegration()
		{
			foreach (ContentWindow cw in ContentWindows)
			{
				if (cw.DockState == DockState.Document)
				{
					foreach (Content c in cw.Contents)
						c.RefreshMdiIntegration();
				}
			}
		}

		internal void RemoveContent(Content c)
		{
			if (c == null)
				throw(new ArgumentNullException());
			
			if (Contents.Contains(c))
			{
				Contents.Remove(c);
				OnContentRemoved(new ContentEventArgs(c));
			}
		}

		internal void RemoveContentWindow(ContentWindow contentWindow)
		{
			if (!ContentWindows.Contains(contentWindow))
				return;

			ContentWindows.Remove(contentWindow);
		}

		internal void RemoveFloatWindow(FloatWindow floatWindow)
		{
			if (!FloatWindows.Contains(floatWindow))
				return;

			FloatWindows.Remove(floatWindow);
		}

		public void SetContentWindowIndex(ContentWindow cw, int index)
		{
			int oldIndex = ContentWindows.IndexOf(cw);
			if (oldIndex == -1)
				throw(new ArgumentException(ResourceHelper.GetString("DockManager.SetContentWindowIndex.InvalidContentWindow")));

			if (index < 0 || index > ContentWindows.Count - 1)
				if (index != -1)
					throw(new ArgumentOutOfRangeException(ResourceHelper.GetString("DockManager.SetContentWindowIndex.InvalidIndex")));
				
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

			RefreshContentWindows(cw.DockState);
		}

		internal void TestDrop(DragHandler dragHandler, Point pt)
		{
			if (DockArea.Width <=0 || DockArea.Height <= 0)
				return;

			Point ptClient = PointToClient(pt);

			int dragSize = MeasureContentWindow.DragSize;
			Rectangle rectDoc = DocumentRectangle;

			if (ptClient.Y - rectDoc.Top >= 0 && ptClient.Y - rectDoc.Top < dragSize &&
				GetContentWindow(DockState.DockTop) == null &&
				dragHandler.IsDockStateValid(DockState.DockTop))
				dragHandler.DropTarget.SetDropTarget(this, DockStyle.Top);
			else if (rectDoc.Bottom - ptClient.Y >= 0 && rectDoc.Bottom - ptClient.Y < dragSize &&
				GetContentWindow(DockState.DockBottom) == null &&
				dragHandler.IsDockStateValid(DockState.DockBottom))
				dragHandler.DropTarget.SetDropTarget(this, DockStyle.Bottom);
			else if (rectDoc.Right - ptClient.X >= 0 && rectDoc.Right - ptClient.X < dragSize &&
				GetContentWindow(DockState.DockRight) == null &&
				dragHandler.IsDockStateValid(DockState.DockRight))
				dragHandler.DropTarget.SetDropTarget(this, DockStyle.Right);
			else if (ptClient.X - rectDoc.Left >= 0 && ptClient.X - rectDoc.Left < dragSize &&
				GetContentWindow(DockState.DockLeft) == null &&
				dragHandler.IsDockStateValid(DockState.DockLeft))
				dragHandler.DropTarget.SetDropTarget(this, DockStyle.Left);
			else if (((ptClient.Y - rectDoc.Top >= dragSize && ptClient.Y - rectDoc.Top < 2 * dragSize) ||
				(rectDoc.Bottom - ptClient.Y >= dragSize && rectDoc.Bottom - ptClient.Y < 2 * dragSize) ||
				(rectDoc.Right - ptClient.X >= dragSize && rectDoc.Right - ptClient.X < 2 * dragSize) ||
				(ptClient.X - rectDoc.Left >= dragSize && ptClient.X - rectDoc.Left < 2 * dragSize)) &&
				GetContentWindow(DockState.Document) == null &&
				dragHandler.IsDockStateValid(DockState.Document))
				dragHandler.DropTarget.SetDropTarget(this, DockStyle.Fill);
			else
				return;

			if (dragHandler.DropTarget.SameAsOldValue)
				return;

			Rectangle rect = DockArea;
			if (dragHandler.DropTarget.Dock == DockStyle.Top || dragHandler.DropTarget.Dock == DockStyle.Bottom)
				rect.Height = (int)(DockArea.Height * DockTopPortion);
			else if (dragHandler.DropTarget.Dock == DockStyle.Left || dragHandler.DropTarget.Dock == DockStyle.Right)
				rect.Width = (int)(DockArea.Width * DockTopPortion);
			if (dragHandler.DropTarget.Dock == DockStyle.Bottom)
				rect.Y = DockArea.Bottom - rect.Height;
			else if (dragHandler.DropTarget.Dock == DockStyle.Right)
				rect.X = DockArea.Right - rect.Width;
			else if (dragHandler.DropTarget.Dock == DockStyle.Fill)
				rect = DocumentRectangle;

			rect.Location = PointToScreen(rect.Location);
			dragHandler.DragFrame = DrawHelper.CreateDragFrame(rect, dragSize);
		}

		#region Events
		private static readonly object ActiveDocumentChangedEvent = new object();
		[LocalizedCategory("Category.PropertyChanged")]
		[LocalizedDescription("DockManager.ActiveDocumentChanged.Description")]
		public event EventHandler ActiveDocumentChanged
		{
			add	{	Events.AddHandler(ActiveDocumentChangedEvent, value);	}
			remove	{	Events.RemoveHandler(ActiveDocumentChangedEvent, value);	}
		}
		protected virtual void OnActiveDocumentChanged(EventArgs e)
		{
			EventHandler handler = (EventHandler)Events[ActiveDocumentChangedEvent];
			if (handler != null)
				handler(this, e);
		}

		public delegate void ContentEventHandler(object sender, ContentEventArgs e);
		private static readonly object ContentAddedEvent = new object();
		[LocalizedCategory("Category.DockingNotification")]
		[LocalizedDescription("DockManager.ContentAdded.Description")]
		public event ContentEventHandler ContentAdded
		{
			add	{	Events.AddHandler(ContentAddedEvent, value);	}
			remove	{	Events.RemoveHandler(ContentAddedEvent, value);	}
		}
		protected virtual void OnContentAdded(ContentEventArgs e)
		{
			ContentEventHandler handler = (ContentEventHandler)Events[ContentAddedEvent];
			if (handler != null)
				handler(this, e);
		}

		private static readonly object ContentRemovedEvent = new object();
		[LocalizedCategory("Category.DockingNotification")]
		[LocalizedDescription("DockManager.ContentRemoved.Description")]
		public event ContentEventHandler ContentRemoved
		{
			add	{	Events.AddHandler(ContentRemovedEvent, value);	}
			remove	{	Events.RemoveHandler(ContentRemovedEvent, value);	}
		}
		protected virtual void OnContentRemoved(ContentEventArgs e)
		{
			ContentEventHandler handler = (ContentEventHandler)Events[ContentRemovedEvent];
			if (handler != null)
				handler(this, e);
		}
		#endregion
	
		#region Save as/Load from XML
		public void SaveAsXml(string filename)
		{
			SaveAsXml(filename, Encoding.Unicode);
		}

		public void SaveAsXml(string filename, Encoding encoding)
		{
			FileStream fs = new FileStream(filename, FileMode.Create);
			try
			{
				SaveAsXml(fs, encoding);
			}
			finally
			{
				fs.Close();
			}
		}

		public void SaveAsXml(Stream stream, Encoding encoding)
		{
			XmlTextWriter xmlOut = new XmlTextWriter(stream, encoding); 

			// Use indenting for readability
			xmlOut.Formatting = Formatting.Indented;
			
			// Always begin file with identification and warning
			xmlOut.WriteStartDocument();
			xmlOut.WriteComment(" DockManager configuration file. Author: Weifen Luo, all rights reserved. ");
			xmlOut.WriteComment(" !!! AUTOMATICALLY GENERATED FILE. DO NOT MODIFY !!! ");

			// Associate a version number with the root element so that future version of the code
			// will be able to be backwards compatible or at least recognise out of date versions
			xmlOut.WriteStartElement("DockManager");
			xmlOut.WriteAttributeString("FormatVersion", ConfigFormatVersion);
			xmlOut.WriteAttributeString("DockLeftPortion", DockLeftPortion.ToString());
			xmlOut.WriteAttributeString("DockRightPortion", DockRightPortion.ToString());
			xmlOut.WriteAttributeString("DockTopPortion", DockTopPortion.ToString());
			xmlOut.WriteAttributeString("DockBottomPortion", DockBottomPortion.ToString());

			// Contents
			xmlOut.WriteStartElement("Contents");
			xmlOut.WriteAttributeString("Count", Contents.Count.ToString());
			foreach (Content c in Contents)
			{
				xmlOut.WriteStartElement("Content" + Contents.IndexOf(c).ToString());
				xmlOut.WriteAttributeString("PersistString", c.PersistString);
				xmlOut.WriteAttributeString("AutoHidePortion", c.AutoHidePortion.ToString());
				xmlOut.WriteAttributeString("IsHidden", c.IsHidden.ToString());
				xmlOut.WriteEndElement();
			}
			xmlOut.WriteEndElement();

			// FloatWindows
			RectangleConverter rectConverter = new RectangleConverter();
			xmlOut.WriteStartElement("FloatWindows");
			xmlOut.WriteAttributeString("Count", FloatWindows.Count.ToString());
			foreach (FloatWindow fw in FloatWindows)
			{
				xmlOut.WriteStartElement("FloatWindow" + FloatWindows.IndexOf(fw).ToString());
				xmlOut.WriteAttributeString("Bounds", rectConverter.ConvertToString(fw.Bounds));
				xmlOut.WriteAttributeString("AllowRedocking", fw.AllowRedocking.ToString());
				xmlOut.WriteEndElement();
			}
			xmlOut.WriteEndElement();
			
			// ContentWindows
			xmlOut.WriteStartElement("ContentWindows");
			xmlOut.WriteAttributeString("Count", ContentWindows.Count.ToString());
			foreach (ContentWindow cw in ContentWindows)
			{
				xmlOut.WriteStartElement("ContentWindow" + ContentWindows.IndexOf(cw).ToString());
				xmlOut.WriteAttributeString("VisibleState", cw.VisibleState.ToString());
				xmlOut.WriteAttributeString("IsHidden", cw.IsHidden.ToString());
				xmlOut.WriteAttributeString("LayoutDock", cw.LayoutDock.ToString());
				xmlOut.WriteAttributeString("LayoutDocument", cw.LayoutDocument.ToString());
				xmlOut.WriteAttributeString("LayoutFloat", cw.LayoutFloat.ToString());
				xmlOut.WriteAttributeString("SizeDock", cw.SizeDock.ToString());
				xmlOut.WriteAttributeString("SizeDocument", cw.SizeDocument.ToString());
				xmlOut.WriteAttributeString("SizeFloat", cw.SizeFloat.ToString());
				xmlOut.WriteAttributeString("RestoreState", cw.RestoreState.ToString());
				xmlOut.WriteAttributeString("IsActivated", cw.IsActivated.ToString());
				xmlOut.WriteAttributeString("ActiveContent", Contents.IndexOf(cw.ActiveContent).ToString());
				xmlOut.WriteAttributeString("IndexInFloatWindow", cw.FloatWindow == null ? "-1" : cw.FloatWindow.ContentWindows.IndexOf(cw).ToString());
				xmlOut.WriteAttributeString("FloatWindow", FloatWindows.IndexOf(cw.FloatWindow).ToString());
				xmlOut.WriteAttributeString("AllowRedocking", cw.AllowRedocking.ToString());
				xmlOut.WriteStartElement("Contents");
				xmlOut.WriteAttributeString("Count", cw.Contents.Count.ToString());
				foreach (Content c in cw.Contents)
				{
					xmlOut.WriteStartElement("Content" + cw.Contents.IndexOf(c));
					xmlOut.WriteAttributeString("RefIndex", Contents.IndexOf(c).ToString());
					xmlOut.WriteEndElement();
				}
				xmlOut.WriteEndElement();
				xmlOut.WriteEndElement();
			}
			xmlOut.WriteEndElement();

			xmlOut.WriteEndElement();
			xmlOut.WriteEndDocument();

			// This should flush all actions and close the file
			xmlOut.Close();
		}

		public void LoadFromXml(string filename, GetContentCallback getContentCallback)
		{
			FileStream fs = new FileStream(filename, FileMode.Open);
			try
			{
				LoadFromXml(fs, getContentCallback);
			}
			finally
			{
				fs.Close();
			}
		}

		public void LoadFromXml(Stream stream, GetContentCallback getContentCallback)
		{
			if (Contents.Count != 0)
				throw new InvalidOperationException(ResourceHelper.GetString("DockManager.LoadFromXml.AlreadyInitialized"));

			XmlTextReader xmlIn = new XmlTextReader(stream);
			xmlIn.WhitespaceHandling = WhitespaceHandling.None;
			xmlIn.MoveToContent();

			if (xmlIn.Name != "DockManager")
				throw new ArgumentException(ResourceHelper.GetString("DockManager.LoadFromXml.InvalidXmlFormat"));

			string formatVersion = xmlIn.GetAttribute("FormatVersion");
			if (formatVersion != ConfigFormatVersion)
				throw new ArgumentException(ResourceHelper.GetString("DockManager.LoadFromXml.InvalidFormatVersion"));
			DockLeftPortion = Convert.ToDouble(xmlIn.GetAttribute("DockLeftPortion"));
			DockRightPortion = Convert.ToDouble(xmlIn.GetAttribute("DockRightPortion"));
			DockTopPortion = Convert.ToDouble(xmlIn.GetAttribute("DockTopPortion"));
			DockBottomPortion = Convert.ToDouble(xmlIn.GetAttribute("DockBottomPortion"));

			// Load Contents
			MoveToNextElement(xmlIn);
			if (xmlIn.Name != "Contents")
				throw new ArgumentException(ResourceHelper.GetString("DockManager.LoadFromXml.InvalidXmlFormat"));
			int countOfContents = Convert.ToInt16(xmlIn.GetAttribute("Count"));
			for (int i=0; i<countOfContents; i++)
			{
				MoveToNextElement(xmlIn);
				if (xmlIn.Name != "Content" + i.ToString())
					throw new ArgumentException(ResourceHelper.GetString("DockManager.LoadFromXml.InvalidXmlFormat"));

				string persistString = xmlIn.GetAttribute("PersistString");
				Content c = getContentCallback(persistString);
				if (c == null)
					c = new Content.DummyContent();
				c.DockManager = this;
				c.AutoHidePortion = Convert.ToDouble(xmlIn.GetAttribute("AutoHidePortion"));
				c.IsHidden = Convert.ToBoolean(xmlIn.GetAttribute("IsHidden"));
			}

			// Load FloatWindows
			RectangleConverter rectConverter = new RectangleConverter();
			MoveToNextElement(xmlIn);
			if (xmlIn.Name != "FloatWindows")
				throw new ArgumentException(ResourceHelper.GetString("DockManager.LoadFromXml.InvalidXmlFormat"));
			int countOfFloatWindows = Convert.ToInt16(xmlIn.GetAttribute("Count"));
			MoveToNextElement(xmlIn);
			for (int i=0; i<countOfFloatWindows; i++)
			{
				if (xmlIn.Name != "FloatWindow" + i.ToString())
					throw new ArgumentException(ResourceHelper.GetString("DockManager.LoadFromXml.InvalidXmlFormat"));

				Rectangle bounds = (Rectangle)rectConverter.ConvertFrom(xmlIn.GetAttribute("Bounds"));
				FloatWindow fw = new FloatWindow(this, bounds);
				fw.AllowRedocking = Convert.ToBoolean(xmlIn.GetAttribute("AllowRedocking"));

				MoveToNextElement(xmlIn);
			}
		
			// Load ContentWindows
			if (xmlIn.Name != "ContentWindows")
				throw new ArgumentException(ResourceHelper.GetString("DockManager.LoadFromXml.InvalidXmlFormat"));
			int countOfContentWindows = Convert.ToInt16(xmlIn.GetAttribute("Count"));
			bool[] isActivatedContentWindows = new bool[countOfContentWindows];
			int[] indexesInFloatWindow = new int[countOfContentWindows];
			for (int i=0; i<countOfContentWindows; i++)
			{
				MoveToNextElement(xmlIn);
				if (xmlIn.Name != "ContentWindow" + i.ToString())
					throw new ArgumentException(ResourceHelper.GetString("DockManager.LoadFromXml.InvalidXmlFormat"));

				EnumConverter dockStateConverter = new EnumConverter(typeof(DockState));
				EnumConverter layoutStylesConverter = new EnumConverter(typeof(LayoutStyles));
				DockState visibleState = (DockState)dockStateConverter.ConvertFrom(xmlIn.GetAttribute("VisibleState"));
				DockState restoreState = (DockState)dockStateConverter.ConvertFrom(xmlIn.GetAttribute("RestoreState"));
				bool isHidden = Convert.ToBoolean(xmlIn.GetAttribute("IsHidden"));
				LayoutStyles layoutDock = (LayoutStyles)layoutStylesConverter.ConvertFrom(xmlIn.GetAttribute("LayoutDock"));
				LayoutStyles layoutDocument = (LayoutStyles)layoutStylesConverter.ConvertFrom(xmlIn.GetAttribute("LayoutDocument"));
				LayoutStyles layoutFloat = (LayoutStyles)layoutStylesConverter.ConvertFrom(xmlIn.GetAttribute("LayoutFloat"));
				double sizeDock = Convert.ToDouble(xmlIn.GetAttribute("SizeDock"));
				double sizeDocument = Convert.ToDouble(xmlIn.GetAttribute("SizeDocument"));
				double sizeFloat = Convert.ToDouble(xmlIn.GetAttribute("SizeFloat"));
				isActivatedContentWindows[i] = Convert.ToBoolean(xmlIn.GetAttribute("IsActivated"));
				indexesInFloatWindow[i] = Convert.ToInt16(xmlIn.GetAttribute("IndexInFloatWindow"));
				int indexFloatWindow = Convert.ToInt16(xmlIn.GetAttribute("FloatWindow"));
				FloatWindow fw = (indexFloatWindow == -1 ? null : FloatWindows[indexFloatWindow]);
				int indexActiveContent = Convert.ToInt16(xmlIn.GetAttribute("ActiveContent"));
				Content activeContent = (indexActiveContent == -1) ? null : Contents[indexActiveContent];
				bool allowRedocking = Convert.ToBoolean(xmlIn.GetAttribute("AllowDrag"));

				MoveToNextElement(xmlIn);
				if (xmlIn.Name != "Contents")
					throw new ArgumentException(ResourceHelper.GetString("DockManager.LoadFromXml.InvalidXmlFormat"));
				int countOfContentWindowContents = Convert.ToInt16(xmlIn.GetAttribute("Count"));
				ContentWindow cw = null;
				MoveToNextElement(xmlIn);
				for (int j=0; j<countOfContentWindowContents; j++)
				{
					if (xmlIn.Name != "Content" + j.ToString())
						throw new ArgumentException(ResourceHelper.GetString("DockManager.LoadFromXml.InvalidXmlFormat"));

					Content c = Contents[Convert.ToInt16(xmlIn.GetAttribute("RefIndex"))];
					if (j==0)
					{
						if (visibleState == DockState.Float)
							cw = new ContentWindow(c,visibleState, fw);
						else
							cw = new ContentWindow(c, visibleState);

						if (DockHelper.IsValidRestoreState(restoreState))
							cw.RestoreState = restoreState;
						cw.IsHidden = isHidden;
						cw.LayoutDock = layoutDock;
						cw.LayoutDocument = layoutDocument;
						cw.LayoutFloat = layoutFloat;
						cw.SizeDock = sizeDock;
						cw.SizeDocument = sizeDocument;
						cw.SizeFloat = sizeFloat;
						cw.FloatWindow = fw;
						cw.AllowRedocking = allowRedocking;
					}
					else
					{
						c.ContentWindow = cw;
					}
					MoveToNextElement(xmlIn);
				}

				if (fw != null)		// May need to adjust the sequence in the float window
				{
					for (int j=0; j<i; j++)
						if (indexesInFloatWindow[i] < indexesInFloatWindow[j])
						{
							fw.SetContentWindowIndex(cw, fw.ContentWindows.IndexOf(ContentWindows[j]));
							break;
						}
				}

				cw.ActiveContent = activeContent;
			}

			for (int i=0; i<countOfContentWindows; i++)
				if (ContentWindows[i].DockState == DockState.Document && isActivatedContentWindows[i])
					ContentWindows[i].Activate();

			for (int i=0; i<countOfContentWindows; i++)
				if (ContentWindows[i].DockState != DockState.Document && isActivatedContentWindows[i])
					ContentWindows[i].Activate();
				
			for (int i=Contents.Count-1; i>=0; i--)
				if (Contents[i] is Content.DummyContent)
					Contents[i].Close();

			xmlIn.Close();
		}

		private void MoveToNextElement(XmlTextReader xmlIn)
		{
			xmlIn.Read();
			if (xmlIn.NodeType == XmlNodeType.EndElement)
				xmlIn.Read();
		}

		#endregion
	}
}