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
using System.Resources;
using System.Windows.Forms;

namespace WeifenLuo.WinFormsUI
{
	/// <summary>
	/// Descripiton of DockWindow
	/// </summary>
	internal class DockWindow : TabWindow
	{
		static private Bitmap ImageCloseEnabled;
		static private Bitmap ImageCloseDisabled;
		static private Bitmap ImageAutoHideYes;
		static private Bitmap ImageAutoHideNo;
		static private StringFormat StringFormatCaption;
		static private StringFormat StringFormatTab;
		static private string ToolTipClose;
		static private string ToolTipAutoHide;

		private InertButton m_buttonClose;
		private InertButton m_buttonAutoHide;
		private IContainer components = new Container();
		private Timer m_timerMouseTrack;

		static DockWindow()
		{
			ImageCloseEnabled = ResourceHelper.LoadBitmap("DockWindow.CloseEnabled.bmp");
			ImageCloseDisabled = ResourceHelper.LoadBitmap("DockWindow.CloseDisabled.bmp");
			ImageAutoHideYes = ResourceHelper.LoadBitmap("DockWindow.AutoHideYes.bmp");
			ImageAutoHideNo = ResourceHelper.LoadBitmap("DockWindow.AutoHideNo.bmp");

			StringFormatCaption = new StringFormat();
			StringFormatCaption.Trimming = StringTrimming.EllipsisCharacter;
			StringFormatCaption.LineAlignment = StringAlignment.Center;
			StringFormatCaption.FormatFlags = StringFormatFlags.NoWrap;

			StringFormatTab = new StringFormat(StringFormat.GenericTypographic);
			StringFormatTab.Trimming = StringTrimming.EllipsisCharacter;
			StringFormatTab.LineAlignment = StringAlignment.Center;
			StringFormatTab.FormatFlags = StringFormatFlags.NoWrap;

			ToolTipClose = ResourceHelper.GetString("DockWindow.Close");
			ToolTipAutoHide = ResourceHelper.GetString("DockWindow.AutoHide");
		}

		public DockWindow(ContentWindow contentWindow) : base(contentWindow)
		{
			InternalConstruct(contentWindow, false);
		}

		public DockWindow(ContentWindow contentWindow, bool isFloat) : base(contentWindow)
		{
			InternalConstruct(contentWindow, true);
		}

		private void InternalConstruct(ContentWindow contentWindow, bool isFloatStyle)
		{
			IsFloatStyle = isFloatStyle;

			m_buttonClose = new InertButton(ImageCloseEnabled, ImageCloseDisabled);
			m_buttonAutoHide = new InertButton(IsAutoHide ? ImageAutoHideYes : ImageAutoHideNo);

			m_buttonClose.ToolTipText = ToolTipClose;
			m_buttonClose.Anchor = AnchorStyles.Top | AnchorStyles.Right;
			m_buttonClose.Click += new EventHandler(Close_Click);

			m_buttonAutoHide.ToolTipText = ToolTipAutoHide;
			m_buttonAutoHide.Anchor = AnchorStyles.Top | AnchorStyles.Right;
			m_buttonAutoHide.Click += new EventHandler(AutoHide_Click);

			Controls.AddRange(new Control[] {	m_buttonAutoHide, m_buttonClose	});

			m_timerMouseTrack = new Timer(components);
			m_timerMouseTrack.Tick += new EventHandler(TimerMouseTrack_Tick);

			if (isFloatStyle)
				m_buttonAutoHide.Visible = false;
		}

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (components != null)
					components.Dispose();
			}

			base.Dispose(disposing);
		}

		private bool m_hasCaption = true;
		internal override bool HasCaption
		{
			get	{	return m_hasCaption;	}
			set
			{
				if (m_hasCaption == value)
					return;

				m_hasCaption = value;
				m_buttonAutoHide.Visible = (value && !IsFloatStyle);
				m_buttonClose.Visible = value;
				Invalidate();
			}
		}

		private bool m_isAutoHide = false;
		public bool IsAutoHide
		{
			get	{	return m_isAutoHide;	}
			set
			{
				if (m_isAutoHide == value)
					return;

				m_isAutoHide = value;
				m_buttonAutoHide.ImageEnabled = value ? ImageAutoHideYes : ImageAutoHideNo;
			}
		}

		private bool m_isFloatStyle;
		private bool IsFloatStyle
		{
			get	{	return m_isFloatStyle;	}
			set	{	m_isFloatStyle = value;	}
		}

		private bool TabStripHidden
		{
			get	{	return IsAutoHide;	}
		}

		internal void AnimateHide()
		{
			if (DockManager.ActiveAutoHideContent != ActiveContent || ActiveContent == null)
				throw(new InvalidOperationException(""));

			if (DockManager.AnimateAutoHide)
				AnimateWindow(false);
			else
				DockManager.AutoHideWindow.Hide();

			OnAutoHide(false);
		}

		internal void AnimateShow()
		{
			if (DockManager.ActiveAutoHideContent == null)
				throw(new InvalidOperationException(""));

			ActiveContent = DockManager.ActiveAutoHideContent;

			// Set the size and location of DockWindow
			foreach (Control c in DockManager.AutoHideWindow.Controls)
			{
				if (c == this)
				{
					Rectangle rectTarget = DockManager.GetAutoHideRectangle(true);
					Bounds = new Rectangle(0, 0, rectTarget.Width, rectTarget.Height);
					Visible = true;
				}
				else
					c.Visible = false;
			}

			if (DockManager.AnimateAutoHide)
				AnimateWindow(true);
			else
				DockManager.AutoHideWindow.Show();

			OnAutoHide(true);
		}

		private void AnimateWindow(bool show)
		{
			DockManager.SuspendAutoHideWindowLayout = true;

			Control autoHideWindow = DockManager.AutoHideWindow;
			Rectangle rectSource = DockManager.GetAutoHideRectangle(!show);
			Rectangle rectTarget = DockManager.GetAutoHideRectangle(show);
			autoHideWindow.Bounds = rectSource;
			autoHideWindow.BringToFront();
			autoHideWindow.Visible = true;

			TimeSpan time = new TimeSpan(0, 0, 0, 0, 100);
			int dxLoc, dyLoc;
			int dWidth, dHeight;
			dxLoc = dyLoc = dWidth = dHeight = 0;
			if (rectSource.Left == rectTarget.Left &&
				rectSource.Top == rectTarget.Top &&
				rectSource.Right == rectTarget.Right)
			{
				dHeight = (rectTarget.Height > rectSource.Height ? 1 : -1);
			}
			else if (rectSource.Left == rectTarget.Left &&
				rectSource.Top == rectTarget.Top &&
				rectSource.Bottom == rectTarget.Bottom)
			{
				dWidth = (rectTarget.Width > rectSource.Width ? 1 : -1);
			}
			else if (rectSource.Right == rectTarget.Right &&
				rectSource.Top == rectTarget.Top &&
				rectSource.Bottom == rectTarget.Bottom)
			{
				dxLoc = (rectTarget.Width > rectSource.Width ? -1 : 1);
				dWidth = (rectTarget.Width > rectSource.Width ? 1 : -1);
			}
			else if (rectSource.Right == rectTarget.Right &&
				rectSource.Left == rectTarget.Left &&
				rectSource.Bottom == rectTarget.Bottom)
			{
				dyLoc = (rectTarget.Height > rectSource.Height ? -1 : 1);
				dHeight = (rectTarget.Height > rectSource.Height ? 1 : -1);
			}
			else
				throw(new ArgumentException("Invalid rectTarget."));

			int speedFactor = 1;
			int totalPixels = (rectSource.Width != rectTarget.Width) ?
				Math.Abs(rectSource.Width - rectTarget.Width) :
				Math.Abs(rectSource.Height - rectTarget.Height);
			int remainPixels = totalPixels;
			DateTime startingTime = DateTime.Now;
			while (rectSource != rectTarget)
			{
				DateTime startPerMove = DateTime.Now;

				rectSource.X += dxLoc * speedFactor;
				rectSource.Y += dyLoc * speedFactor;
				rectSource.Width += dWidth * speedFactor;
				rectSource.Height += dHeight * speedFactor;
				if (Math.Sign(rectTarget.X - rectSource.X) != Math.Sign(dxLoc))
					rectSource.X = rectTarget.X;
				if (Math.Sign(rectTarget.Y - rectSource.Y) != Math.Sign(dyLoc))
					rectSource.Y = rectTarget.Y;
				if (Math.Sign(rectTarget.Width - rectSource.Width) != Math.Sign(dWidth))
					rectSource.Width = rectTarget.Width;
				if (Math.Sign(rectTarget.Height - rectSource.Height) != Math.Sign(dHeight))
					rectSource.Height = rectTarget.Height;
				autoHideWindow.Bounds = rectSource;
				DockManager.Update();

				remainPixels -= speedFactor;

				while (true)
				{
					TimeSpan elapsedPerMove = DateTime.Now - startPerMove;
					TimeSpan elapsedTime = DateTime.Now - startingTime;
					if ((time - elapsedTime).TotalMilliseconds <= 0)
					{
						speedFactor = remainPixels;
						break;
					}
					else
						speedFactor = remainPixels * (int)elapsedPerMove.TotalMilliseconds / (int)((time - elapsedTime).TotalMilliseconds);
					if (speedFactor >= 1)
						break;
				}
			}

			DockManager.SuspendAutoHideWindowLayout = false;
		}

		private void CalculateTabs()
		{
			if (CountOfVisibleContents <= 1 || TabStripHidden)
				return;

			Rectangle rectTabStrip = GetTabStripRectangle();

			//////////////////////////////////////////////////////////////////////////////
			/// Calculate tab widths
			/// //////////////////////////////////////////////////////////////////////////
			int countOfVisibleContents = CountOfVisibleContents;
			int[] maxWidths = new int[countOfVisibleContents];
			bool[] flags = new bool[countOfVisibleContents];
			for (int i=0; i<countOfVisibleContents; i++)
			{
				maxWidths[i] = GetTabOriginalWidth(i);
				flags[i] = false;
			}

			// Set tab whose max width less than average width
			bool anyWidthWithinAverage = true;
			int totalWidth = rectTabStrip.Width - MeasureNormalTab.StripGapLeft - MeasureNormalTab.StripGapRight;
			int totalAllocatedWidth = 0;
			int averageWidth = totalWidth / countOfVisibleContents;
			int remainedContents = countOfVisibleContents;
			for (anyWidthWithinAverage=true; anyWidthWithinAverage && remainedContents>0;)
			{
				anyWidthWithinAverage = false;
				for (int i=0; i<countOfVisibleContents; i++)
				{
					if (flags[i])
						continue;

					Content c = GetVisibleContent(i);

					if (maxWidths[i] <= averageWidth)
					{
						flags[i] = true;
						c.TabWidth = maxWidths[i];
						totalAllocatedWidth += c.TabWidth;
						anyWidthWithinAverage = true;
						remainedContents--;
					}
				}
				if (remainedContents != 0)
					averageWidth = (totalWidth - totalAllocatedWidth) / remainedContents;
			}

			// If any tab width not set yet, set it to the average width
			if (remainedContents > 0)
			{
				int roundUpWidth = (totalWidth - totalAllocatedWidth) - (averageWidth * remainedContents);
				for (int i=0; i<countOfVisibleContents; i++)
				{
					if (flags[i])
						continue;

					Content c = GetVisibleContent(i);

					flags[i] = true;
					if (roundUpWidth > 0)
					{
						c.TabWidth = averageWidth + 1;
						roundUpWidth --;
					}
					else
						c.TabWidth = averageWidth;
				}
			}

			//////////////////////////////////////////////////////////////////////////////
			/// Set the X position of the tabs
			/////////////////////////////////////////////////////////////////////////////
			int x = rectTabStrip.X + MeasureNormalTab.StripGapLeft;
			for (int i=0; i<countOfVisibleContents; ++i)
			{
				Content c = GetVisibleContent(i);
				c.TabX = x;
				x += c.TabWidth;
			}
		}

		protected override void DrawCaption(Graphics g)
		{
			Rectangle rectCaption = CaptionRectangle;

			if (rectCaption.IsEmpty)
				return;

			Brush brushBackGround = IsActivated ? SystemBrushes.ActiveCaption : SystemBrushes.Control;

			g.FillRectangle(brushBackGround, rectCaption);

			m_buttonClose.BackColor = m_buttonAutoHide.BackColor = (IsActivated ? SystemColors.ActiveCaption : SystemColors.Control);
			m_buttonClose.ForeColor = m_buttonAutoHide.ForeColor = (IsActivated ? SystemColors.ActiveCaptionText : SystemColors.ControlText);
			m_buttonClose.BorderColor = m_buttonAutoHide.BorderColor = (IsActivated ? SystemColors.ActiveCaptionText : Color.Empty);

			if (!IsActivated)
			{
				g.DrawLine(SystemPens.GrayText, rectCaption.X + 1, rectCaption.Y, rectCaption.X + rectCaption.Width - 2, rectCaption.Y);
				g.DrawLine(SystemPens.GrayText, rectCaption.X + 1, rectCaption.Y + rectCaption.Height - 1, rectCaption.X + rectCaption.Width - 2, rectCaption.Y + rectCaption.Height - 1);
				g.DrawLine(SystemPens.GrayText, rectCaption.X, rectCaption.Y + 1, rectCaption.X, rectCaption.Y + rectCaption.Height - 2);
				g.DrawLine(SystemPens.GrayText, rectCaption.X + rectCaption.Width - 1, rectCaption.Y + 1, rectCaption.X + rectCaption.Width - 1, rectCaption.Y + rectCaption.Height - 2);
			}

			int buttonWidth = ImageCloseEnabled.Width;
			int buttonHeight = ImageCloseEnabled.Height;
			int height = rectCaption.Height - MeasureCaption.ButtonGapTop - MeasureCaption.ButtonGapBottom;
			if (buttonHeight < height)
			{
				buttonWidth = buttonWidth * (height / buttonHeight);
				buttonHeight = height;
			}
			Size buttonSize = new Size(buttonWidth, buttonHeight);
			m_buttonClose.Size = m_buttonAutoHide.Size = buttonSize;
			int x = rectCaption.X + rectCaption.Width - 1 - MeasureCaption.ButtonGapRight - m_buttonClose.Width;
			int y = rectCaption.Y + MeasureCaption.ButtonGapTop;
			m_buttonClose.Location = new Point(x, y);
			Point point = m_buttonClose.Location;
			point.Offset(-(m_buttonAutoHide.Width + MeasureCaption.ButtonGapBetween), 0);
			m_buttonAutoHide.Location = point;
				
			Rectangle rectCaptionText = rectCaption;
			rectCaptionText.X += MeasureCaption.TextGapLeft;
			rectCaptionText.Width = rectCaption.Width - MeasureCaption.ButtonGapRight
				- MeasureCaption.ButtonGapLeft
				- MeasureCaption.ButtonGapBetween - 2 * m_buttonClose.Width
				- MeasureCaption.TextGapLeft - MeasureCaption.TextGapRight;
			rectCaptionText.Y += MeasureCaption.TextGapTop;
			rectCaptionText.Height -= MeasureCaption.TextGapTop + MeasureCaption.TextGapBottom;
			Brush brush = IsActivated ? SystemBrushes.FromSystemColor(SystemColors.ActiveCaptionText)
				: SystemBrushes.FromSystemColor(SystemColors.ControlText);
			g.DrawString(CaptionText, Font, brush, rectCaptionText, StringFormatCaption);

			m_buttonClose.Enabled = ActiveContent.CloseButton;
		}

		private void DrawTab(Graphics g, Content c, Rectangle rect)
		{
			Rectangle rectIcon = new Rectangle(
				rect.X + MeasureNormalTab.ImageGapLeft,
				rect.Y + rect.Height - 1 - MeasureNormalTab.ImageGapBottom - MeasureNormalTab.ImageHeight,
				MeasureNormalTab.ImageWidth, MeasureNormalTab.ImageHeight);
			Rectangle rectText = rectIcon;
			rectText.X += rectIcon.Width + MeasureNormalTab.ImageGapRight;
			rectText.Width = rect.Width - rectIcon.Width - MeasureNormalTab.ImageGapLeft - 
				MeasureNormalTab.ImageGapRight - MeasureNormalTab.TextGapRight;

			if (ActiveContent == c)
			{
				g.FillRectangle(SystemBrushes.Control, rect);
				g.DrawLine(SystemPens.ControlText,
					rect.X, rect.Y + rect.Height - 1, rect.X + rect.Width - 1, rect.Y + rect.Height - 1);
				g.DrawLine(SystemPens.ControlText,
					rect.X + rect.Width - 1, rect.Y, rect.X + rect.Width - 1, rect.Y + rect.Height - 1);
				g.DrawString(c.Text, Font, SystemBrushes.ControlText, rectText, StringFormatTab);
			}
			else
			{
				if (GetIndexOfVisibleContents(ActiveContent) !=
					GetIndexOfVisibleContents(c) + 1)
					g.DrawLine(SystemPens.GrayText,
						rect.X + rect.Width - 1, rect.Y + 3, rect.X + rect.Width - 1, rect.Y + rect.Height - 4);
				g.DrawString(c.Text, Font, SystemBrushes.FromSystemColor(SystemColors.GrayText), rectText, StringFormatTab);
			}

			if (rect.Contains(rectIcon))
				g.DrawIcon(c.Icon, rectIcon);
		}

		protected override void DrawTabStrip(Graphics g)
		{
			CalculateTabs();

			Rectangle rectTabStrip = GetTabStripRectangle();

			if (rectTabStrip.IsEmpty)
				return;

			// Paint the background
			g.FillRectangle(SystemBrushes.FromSystemColor(SystemColors.ControlLightLight), rectTabStrip);
			g.DrawLine(SystemPens.ControlText, rectTabStrip.Left, rectTabStrip.Top,
				rectTabStrip.Right, rectTabStrip.Top);

			for (int i=0; i<CountOfVisibleContents; i++)
				DrawTab(g, GetVisibleContent(i), GetTabRectangle(i));
		}

		protected override Rectangle CaptionRectangle
		{
			get
			{
				if (!HasCaption)
					return Rectangle.Empty;

				Rectangle rectTabWindow = TabWindowRectangle;
				int x, y, width;
				x = rectTabWindow.X;
				y = rectTabWindow.Y;
				width = rectTabWindow.Width;

				int height = Font.Height + MeasureCaption.TextGapTop + MeasureCaption.TextGapBottom;

				if (height < ImageCloseEnabled.Height + MeasureCaption.ButtonGapTop + MeasureCaption.ButtonGapBottom)
					height = ImageCloseEnabled.Height + MeasureCaption.ButtonGapTop + MeasureCaption.ButtonGapBottom;

				return new Rectangle(x, y, width, height);
			}
		}

		protected override int GetTabOriginalWidth(int index)
		{
			Content c = GetVisibleContent(index);
			using (Graphics g = CreateGraphics())
			{
				SizeF sizeString = g.MeasureString(c.Text, Font);
				return MeasureNormalTab.ImageWidth + (int)sizeString.Width + 1 + MeasureNormalTab.ImageGapLeft
					+ MeasureNormalTab.ImageGapRight + MeasureNormalTab.TextGapRight;
			}
		}

		protected override Rectangle GetTabRectangle(int index)
		{
			Rectangle rectTabStrip = GetTabStripRectangle();

			Content c = GetVisibleContent(index);
			return new Rectangle(c.TabX, rectTabStrip.Y, c.TabWidth, rectTabStrip.Height);
		}

		protected override Rectangle GetTabStripRectangle()
		{
			return GetTabStripRectangle(false);
		}

		protected override Rectangle GetTabStripRectangle(bool tabOnly)
		{
			if (CountOfVisibleContents <= 1 || TabStripHidden)
				return Rectangle.Empty;

			Rectangle rectTabWindow = TabWindowRectangle;

			int width = rectTabWindow.Width;
			int height = Math.Max(Font.Height, MeasureNormalTab.ImageHeight)
				+ MeasureNormalTab.ImageGapTop
				+ MeasureNormalTab.ImageGapBottom;
			int x = rectTabWindow.X;
			int y = rectTabWindow.Bottom - height;
			Rectangle rectCaption = CaptionRectangle;
			if (rectCaption.Contains(x, y))
				y = rectCaption.Y + rectCaption.Height;

			return new Rectangle(x, y, width, height);
		}

		internal void OnAutoHide(bool show)
		{
			if (show && !IsActivated)
			{
				// start the timer
				uint hovertime = 0;

				User32.SystemParametersInfo(Win32.SystemParametersInfoActions.GetMouseHoverTime,
					0, ref hovertime, 0);

				// assign a default value 400 in case of setting Timer.Interval invalid value exception
				if (((int)hovertime) <= 0)
					hovertime = 400;

				m_timerMouseTrack.Interval = 2 * (int)hovertime;
				m_timerMouseTrack.Enabled = true;
			}
			else
			{
				// stop the timer
				m_timerMouseTrack.Enabled = false;
			}
		}

		private void TimerMouseTrack_Tick(object sender, EventArgs e)
		{
			if (IsAutoHide == false)
			{
				m_timerMouseTrack.Enabled = false;
				return;
			}

			if (DockManager.ActiveAutoHideContent == ActiveContent && !IsActivated)
			{
				Point ptMouseInDockWindow = PointToClient(Control.MousePosition);
				Point ptMouseInDockManager = DockManager.PointToClient(Control.MousePosition);

				Rectangle rectTabStrip = DockManager.GetTabStripRectangle(DockState, true);

				if (!ClientRectangle.Contains(ptMouseInDockWindow) && !rectTabStrip.Contains(ptMouseInDockManager))
				{
					DockManager.ActiveAutoHideContent = null;
					m_timerMouseTrack.Enabled = false;
				}
			}
		}

		private void AutoHide_Click(object sender, EventArgs e)
		{
			ContentWindow.VisibleState = DockHelper.ToggleAutoHideState(DockState);
			if (IsAutoHide)
			{
				bool oldAnimateAutoHide = DockManager.AnimateAutoHide;
				DockManager.AnimateAutoHide = false;
				DockManager.ActiveAutoHideContent = ActiveContent;
				DockManager.AnimateAutoHide = oldAnimateAutoHide;
			}
			ContentWindow.Activate();
		}

		private void Close_Click(object sender, EventArgs e)
		{
			ContentWindow.CloseActiveContent();
		}

		protected override Region GetTestDropDragFrame(DockStyle dock, int contentIndex)
		{
			int dragSize = MeasureContentWindow.DragSize;

			if (dock != DockStyle.Fill)
			{
				Rectangle rect = TabWindowRectangle;
				if (dock == DockStyle.Right)
					rect.X += rect.Width / 2;
				if (dock == DockStyle.Bottom)
					rect.Y += rect.Height / 2;
				if (dock == DockStyle.Left || dock == DockStyle.Right)
					rect.Width -= rect.Width / 2;
				if (dock == DockStyle.Top || dock == DockStyle.Bottom)
					rect.Height -= rect.Height / 2;
				rect.Location = PointToScreen(rect.Location);

				return DrawHelper.CreateDragFrame(rect, dragSize);
			}
			else
			{
				Rectangle[] rects = new Rectangle[3];
				rects[2] = Rectangle.Empty;
				rects[0] = TabWindowRectangle;
				if (contentIndex != -1)
					rects[1] = GetTabRectangle(contentIndex);
				else
					rects[1] = Rectangle.Empty;

				if (!rects[1].IsEmpty)
				{
					rects[0].Height = rects[1].Top - rects[0].Top;
					rects[2].X = rects[1].X + dragSize;
					rects[2].Y = rects[1].Y - dragSize;
					rects[2].Width = rects[1].Width - 2 * dragSize;
					rects[2].Height = 2 * dragSize;
				}

				rects[0].Location = PointToScreen(rects[0].Location);
				rects[1].Location = PointToScreen(rects[1].Location);
				rects[2].Location = PointToScreen(rects[2].Location);
				return DrawHelper.CreateDragFrame(rects, dragSize);
			}
		}

	}
}
