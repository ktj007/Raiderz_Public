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
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace WeifenLuo.WinFormsUI
{
	internal class TabWindowSplitter : Control
	{
		public TabWindowSplitter()
		{
			SetStyle(ControlStyles.Selectable, false);
		}

		public override DockStyle Dock
		{
			get	{	return base.Dock;	}
			set
			{
				SuspendLayout();
				base.Dock = value;
				
				if (Dock == DockStyle.Left || Dock == DockStyle.Right)
					Width = MeasureContentWindow.DragSize;
				else if (Dock == DockStyle.Top || Dock == DockStyle.Bottom)
					Height = MeasureContentWindow.DragSize;
				else
					Bounds = Rectangle.Empty;

				if (Dock == DockStyle.Left || Dock == DockStyle.Right)
					Cursor = Cursors.VSplit;
				else if (Dock == DockStyle.Top || Dock == DockStyle.Bottom)
					Cursor = Cursors.HSplit;
				else
					Cursor = Cursors.Default;
					
				ResumeLayout();

				Invalidate();
			}
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint(e);

			Graphics g = e.Graphics;

			TabWindow tabWindow = Parent as DocumentWindow;
			if (tabWindow == null)
				return;

			if (Dock == DockStyle.Left)
			{
				Rectangle rectTabStrip = ClientRectangle;
				rectTabStrip.Height = tabWindow.TabStripRectangle.Height;
				g.FillRectangle(SystemBrushes.ControlLightLight, rectTabStrip);
				Point ptRightTop = new Point(ClientRectangle.Right - 1, ClientRectangle.Top);
				Point ptRightBottom = new Point(ClientRectangle.Right - 1, ClientRectangle.Bottom);
				g.DrawLine(SystemPens.ControlDark, ptRightTop, ptRightBottom);
			}
			else if (Dock == DockStyle.Top)
			{
				Point ptLeftBottom = new Point(ClientRectangle.Left, ClientRectangle.Bottom - 1);
				Point ptRightBottom = new Point(ClientRectangle.Right, ClientRectangle.Bottom - 1);
				g.DrawLine(SystemPens.ControlDark, ptLeftBottom, ptRightBottom);
			}
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown(e);

			if (e.Button != MouseButtons.Left)
				return;

			TabWindow tabWindow = Parent as TabWindow;
			if (tabWindow == null)
				return;

			tabWindow.DockManager.DragHandler.BeginDragTabWindowSplitter(tabWindow, tabWindow.SplitterLocation);
		}

		protected override void WndProc(ref Message m)
		{
			if (m.Msg == (int)Win32.Msgs.WM_MOUSEACTIVATE)
			{
				return;
			}
			base.WndProc(ref m);
		}
	}
}
