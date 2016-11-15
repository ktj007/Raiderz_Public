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
using System.Resources;
using System.Windows.Forms;

namespace WeifenLuo.WinFormsUI
{
	/// <summary>
	/// Description of DocumentWindow
	/// </summary>
	internal class DocumentWindow : TabWindow
	{
		static private Bitmap ImageCloseEnabled;
		static private Bitmap ImageCloseDisabled;
		static private Bitmap ImageScrollLeftEnabled;
		static private Bitmap ImageScrollLeftDisabled;
		static private Bitmap ImageScrollRightEnabled;
		static private Bitmap ImageScrollRightDisabled;
		static private string ToolTipClose;
		static private string ToolTipScrollLeft;
		static private string ToolTipScrollRight;
		static private StringFormat StringFormatTab;

		private InertButton m_buttonClose, m_buttonScrollLeft, m_buttonScrollRight;
		private int m_offset = 0;

		static DocumentWindow()
		{
			ImageCloseEnabled = ResourceHelper.LoadBitmap("DocumentWindow.CloseEnabled.bmp");
			ImageCloseDisabled = ResourceHelper.LoadBitmap("DocumentWindow.CloseDisabled.bmp");
			ImageScrollLeftEnabled = ResourceHelper.LoadBitmap("DocumentWindow.ScrollLeftEnabled.bmp");
			ImageScrollLeftDisabled = ResourceHelper.LoadBitmap("DocumentWindow.ScrollLeftDisabled.bmp");
			ImageScrollRightEnabled = ResourceHelper.LoadBitmap("DocumentWindow.ScrollRightEnabled.bmp");
			ImageScrollRightDisabled = ResourceHelper.LoadBitmap("DocumentWindow.ScrollRightDisabled.bmp");
			ToolTipClose = ResourceHelper.GetString("DocumentWindow.Close");
			ToolTipScrollLeft = ResourceHelper.GetString("DocumentWindow.ScrollLeft");
			ToolTipScrollRight = ResourceHelper.GetString("DocumentWindow.ScrollRight");

			StringFormatTab = new StringFormat(StringFormat.GenericTypographic);
			StringFormatTab.Alignment = StringAlignment.Center;
			StringFormatTab.Trimming = StringTrimming.EllipsisPath;
			StringFormatTab.LineAlignment = StringAlignment.Center;
			StringFormatTab.FormatFlags = StringFormatFlags.NoWrap;
		}

		public DocumentWindow(ContentWindow contentWindow) : base(contentWindow)
		{
			m_buttonClose = new InertButton(ImageCloseEnabled, ImageCloseDisabled);
			m_buttonScrollLeft = new InertButton(ImageScrollLeftEnabled, ImageScrollLeftDisabled);
			m_buttonScrollRight = new InertButton(ImageScrollRightEnabled, ImageScrollRightDisabled);

			m_buttonClose.ToolTipText = ToolTipClose;
			m_buttonClose.Anchor = AnchorStyles.Top | AnchorStyles.Right;
			m_buttonClose.Click += new EventHandler(Close_Click);

			m_buttonScrollLeft.Enabled = false;
			m_buttonScrollLeft.ToolTipText = ToolTipScrollLeft;
			m_buttonScrollLeft.Anchor = AnchorStyles.Top | AnchorStyles.Right;
			m_buttonScrollLeft.Click += new EventHandler(ScrollLeft_Click);

			m_buttonScrollRight.Enabled = false;
			m_buttonScrollRight.ToolTipText = ToolTipScrollRight;
			m_buttonScrollRight.Anchor = AnchorStyles.Top | AnchorStyles.Right;
			m_buttonScrollRight.Click += new EventHandler(ScrollRight_Click);

			Controls.AddRange(new Control[] {	m_buttonScrollLeft,
												m_buttonScrollRight,
												m_buttonClose	});
		}

		private void CalculateTabs()
		{
			int countOfVisibleContents = CountOfVisibleContents;
			if (countOfVisibleContents == 0)
				return;

			Rectangle rectTabStrip = GetTabStripRectangle();
			int x = rectTabStrip.X + MeasureDocListTab.TabGapLeft + m_offset;
			for (int i=0; i<countOfVisibleContents; i++)
			{
				Content c = GetVisibleContent(i);
				c.TabX = x;
				c.TabWidth = Math.Min(GetTabOriginalWidth(i), MeasureDocListTab.TabMaxWidth);
				x += c.TabWidth;
			}
		}

		private void Close_Click(object sender, EventArgs e)
		{
			ContentWindow.CloseActiveContent();
		}

		private void ScrollLeft_Click(object sender, EventArgs e)
		{
			Rectangle rectTabStrip = GetTabStripRectangle(true);

			int index;
			for (index=0; index<CountOfVisibleContents; index++)
				if (GetTabRectangle(index).IntersectsWith(rectTabStrip))
					break;

			Rectangle rectTab = GetTabRectangle(index);
			if (rectTab.Left < rectTabStrip.Left)
				m_offset += rectTabStrip.Left - rectTab.Left;
			else if (index == 0)
				m_offset = 0;
			else
				m_offset += rectTabStrip.Left - GetTabRectangle(index - 1).Left;

			Invalidate();
		}
	
		private void ScrollRight_Click(object sender, EventArgs e)
		{
			Rectangle rectTabStrip = GetTabStripRectangle(true);

			int index;
			int countOfVisibleContents = CountOfVisibleContents;
			for (index=0; index<countOfVisibleContents; index++)
				if (GetTabRectangle(index).IntersectsWith(rectTabStrip))
					break;

			if (index + 1 < countOfVisibleContents)
			{
				m_offset -= GetTabRectangle(index + 1).Left - rectTabStrip.Left;
				CalculateTabs();
			}

			Rectangle rectLastTab = GetTabRectangle(countOfVisibleContents - 1);
			if (rectLastTab.Right < rectTabStrip.Right)
				m_offset += rectTabStrip.Right - rectLastTab.Right;

			Invalidate();
		}

		private void DrawTab(Graphics g, Content c, Rectangle rect)
		{
			Rectangle rectText = rect;
			rectText.X += MeasureDocListTab.TextExtraWidth / 2;
			rectText.Width -= MeasureDocListTab.TextExtraWidth;
			if (ActiveContent == c)
			{
				g.FillRectangle(SystemBrushes.Control, rect);
				g.DrawLine(SystemPens.ControlText,
					rect.X + rect.Width - 1, rect.Y,
					rect.X + rect.Width - 1, rect.Y + rect.Height - 1);
				if (IsActivated)
				{
					using (Font boldFont = new Font(this.Font, FontStyle.Bold))
					{
						g.DrawString(c.Text, boldFont, SystemBrushes.ControlText, rectText,StringFormatTab);
					}
				}
				else
					g.DrawString(c.Text, Font, SystemBrushes.FromSystemColor(SystemColors.GrayText), rectText, StringFormatTab);
			}
			else
			{
				if (GetIndexOfVisibleContents(ActiveContent) != GetIndexOfVisibleContents(c) + 1)
					g.DrawLine(SystemPens.GrayText,
						rect.X + rect.Width - 1, rect.Y,
						rect.X + rect.Width - 1, rect.Y + rect.Height - 1 - MeasureDocListTab.TabGapTop);

				g.DrawString(c.Text, Font, SystemBrushes.FromSystemColor(SystemColors.GrayText), rectText, StringFormatTab);
			}
		}

		protected override void DrawTabStrip(Graphics g)
		{
			CalculateTabs();

			int countOfVisibleContents = CountOfVisibleContents;
			if (countOfVisibleContents == 0)
				return;

			Rectangle rectTabStrip = GetTabStripRectangle();

			// Paint the background
			g.FillRectangle(SystemBrushes.FromSystemColor(SystemColors.ControlLightLight), rectTabStrip);

			// Set position and size of the buttons
			int buttonWidth = ImageCloseEnabled.Width;
			int buttonHeight = ImageCloseEnabled.Height;
			int height = rectTabStrip.Height - MeasureDocListTab.ButtonGapTop - MeasureDocListTab.ButtonGapBottom;
			if (buttonHeight < height)
			{
				buttonWidth = buttonWidth * (height / buttonHeight);
				buttonHeight = height;
			}
			Size buttonSize = new Size(buttonWidth, buttonHeight);
			m_buttonClose.Size = m_buttonScrollLeft.Size = m_buttonScrollRight.Size = buttonSize;
			int x = rectTabStrip.X + rectTabStrip.Width - MeasureDocListTab.TabGapLeft
				- MeasureDocListTab.ButtonGapRight - buttonWidth;
			int y = rectTabStrip.Y + MeasureDocListTab.ButtonGapTop;
			m_buttonClose.Location = new Point(x, y);
			Point point = m_buttonClose.Location;
			point.Offset(-(MeasureDocListTab.ButtonGapBetween + buttonWidth), 0);
			m_buttonScrollRight.Location = point;
			point.Offset(-(MeasureDocListTab.ButtonGapBetween + buttonWidth), 0);
			m_buttonScrollLeft.Location = point;

			m_buttonClose.BackColor = m_buttonScrollRight.BackColor	= m_buttonScrollLeft.BackColor = SystemColors.ControlLightLight;
			m_buttonClose.ForeColor = m_buttonScrollRight.ForeColor	= m_buttonScrollLeft.ForeColor = SystemColors.GrayText;
			m_buttonClose.BorderColor = m_buttonScrollRight.BorderColor	= m_buttonScrollLeft.BorderColor = SystemColors.GrayText;

			// Draw the tabs
			Rectangle rectTabOnly = GetTabStripRectangle(true);
			Rectangle rectTab = Rectangle.Empty;
			g.SetClip(rectTabOnly, CombineMode.Replace);
			for (int i=0; i<countOfVisibleContents; i++)
			{
				rectTab = GetTabRectangle(i);
				if (rectTab.IntersectsWith(rectTabOnly))
					DrawTab(g, GetVisibleContent(i), rectTab);
			}

			m_buttonScrollLeft.Enabled = (m_offset < 0);
			m_buttonScrollRight.Enabled = rectTab.Right > rectTabOnly.Right;
			m_buttonClose.Enabled = ActiveContent.CloseButton;
		}

		public override void EnsureTabVisible(Content content)
		{
			CalculateTabs();

			Rectangle rectTabStrip = GetTabStripRectangle(true);
			Rectangle rectTab = GetTabRectangle(GetIndexOfVisibleContents(content));

			if (rectTab.Right > rectTabStrip.Right)
			{
				m_offset -= rectTab.Right - rectTabStrip.Right;
				rectTab.X -= rectTab.Right - rectTabStrip.Right;
			}

			if (rectTab.Left < rectTabStrip.Left)
				m_offset += rectTabStrip.Left - rectTab.Left;

			Invalidate();
		}

		protected override int GetTabOriginalWidth(int index)
		{
			Content c = GetVisibleContent(index);

			using (Graphics g = CreateGraphics())
			{
				SizeF sizeText;
				if (c == ActiveContent && IsActivated)
				{
					using (Font boldFont = new Font(this.Font, FontStyle.Bold))
					{
						sizeText = g.MeasureString(c.Text, boldFont, MeasureDocListTab.TabMaxWidth, StringFormatTab);
					}
				}
				else
					sizeText = g.MeasureString(c.Text, Font, MeasureDocListTab.TabMaxWidth, StringFormatTab);

				return (int)sizeText.Width + 1 + MeasureDocListTab.TextExtraWidth;
			}
		}

		protected override Rectangle GetTabRectangle(int index)
		{
			Rectangle rectTabStrip = GetTabStripRectangle();
			Content c = GetVisibleContent(index);

			return new Rectangle(c.TabX, rectTabStrip.Y + MeasureDocListTab.TabGapTop,
				c.TabWidth, rectTabStrip.Height - MeasureDocListTab.TabGapTop);
		}

		protected override Rectangle GetTabStripRectangle()
		{
			return GetTabStripRectangle(false);
		}

		protected override Rectangle GetTabStripRectangle(bool tabOnly)
		{
			if (CountOfVisibleContents == 0)
				return Rectangle.Empty;

			Rectangle rectTabWindow = TabWindowRectangle;
			int x = rectTabWindow.X;
			int y = rectTabWindow.Y;
			int width = rectTabWindow.Width;
			int height = Math.Max(Font.Height + MeasureDocListTab.TabGapTop + MeasureDocListTab.TextExtraHeight,
				ImageCloseEnabled.Height + MeasureDocListTab.ButtonGapTop + MeasureDocListTab.ButtonGapBottom);

			if (tabOnly)
			{
				x += MeasureDocListTab.TabGapLeft;
				width -= MeasureDocListTab.TabGapLeft + 
					MeasureDocListTab.TabGapRight +
					MeasureDocListTab.ButtonGapRight +
					m_buttonClose.Width +
					m_buttonScrollRight.Width +
					m_buttonScrollLeft.Width +
					2 * MeasureDocListTab.ButtonGapBetween;
			}

			return new Rectangle(x, y, width, height);
		}

		protected override Region GetTestDropDragFrame(DockStyle dock, int contentIndex)
		{
			int dragSize = MeasureContentWindow.DragSize;

			Rectangle[] rects = new Rectangle[3];

			rects[0] = TabWindowRectangle;
			if (dock == DockStyle.Right)
				rects[0].X += rects[0].Width / 2;
			else if (dock == DockStyle.Bottom)
				rects[0].Y += rects[0].Height / 2;
			if (dock == DockStyle.Left || dock == DockStyle.Right)
				rects[0].Width -= rects[0].Width / 2;
			else if (dock == DockStyle.Top || dock == DockStyle.Bottom)
				rects[0].Height -= rects[0].Height / 2;

			if (dock != DockStyle.Fill)
				rects[1] = new Rectangle(rects[0].X + MeasureDocListTab.TabGapLeft, rects[0].Y,
					rects[0].Width - 2 * MeasureDocListTab.TabGapLeft, GetTabStripRectangle().Height);
			else if (contentIndex != -1)
				rects[1] = GetTabRectangle(contentIndex);
			else
				rects[1] = GetTabRectangle(0);

			rects[0].Y = rects[1].Top + rects[1].Height;
			rects[0].Height -= rects[1].Height;
			rects[2] = new Rectangle(rects[1].X + dragSize, rects[0].Y - dragSize,
				rects[1].Width - 2 * dragSize, 2 * dragSize);

			rects[0].Location = PointToScreen(rects[0].Location);
			rects[1].Location = PointToScreen(rects[1].Location);
			rects[2].Location = PointToScreen(rects[2].Location);
			return DrawHelper.CreateDragFrame(rects, dragSize);
		}
	}
}
