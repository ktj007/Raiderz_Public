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
using System.Windows.Forms;

namespace WeifenLuo.WinFormsUI
{
	internal class DropTarget
	{
		private Control m_dropTo = null;
		private DockStyle m_dock = DockStyle.None;
		private int m_contentIndex = -1;

		private Control m_oldDropTo = null;
		private DockStyle m_oldDock = DockStyle.None;
		private int m_oldContentIndex = -1;

		public DropTarget()
		{
			Clear();
		}

		public Control DropTo
		{
			get	{	return m_dropTo;	}
		}

		public DockStyle Dock
		{
			get	{	return m_dock;	}
		}

		public int ContentIndex
		{
			get	{	return m_contentIndex;	}
		}

		public bool SameAsOldValue
		{
			get	{	return (m_dropTo!=null && m_dropTo==m_oldDropTo && m_dock==m_oldDock && m_contentIndex==m_oldContentIndex);	}
		}

		public void Clear()
		{
			Clear(false);
		}

		public void Clear(bool saveOldValue)
		{
			if (saveOldValue)
			{
				m_oldDropTo = m_dropTo;
				m_oldDock = m_dock;
				m_oldContentIndex = m_contentIndex;
			}
			else
			{
				m_oldDropTo = null;
				m_oldDock = DockStyle.None;
				m_oldContentIndex = -1;
			}

			m_dropTo = null;
			m_dock = DockStyle.None;
			m_contentIndex = -1;
		}

		public void SetDropTarget(TabWindow tabWindow, DockStyle dock)
		{
			m_dropTo = tabWindow;
			m_dock = dock;
			m_contentIndex = -1;
		}

		public void SetDropTarget(TabWindow tabWindow, int contentIndex)
		{
			m_dropTo = tabWindow;
			m_dock = DockStyle.Fill;
			m_contentIndex = contentIndex;
		}

		public void SetDropTarget(DockManager dockManager, DockStyle dock)
		{
			m_dropTo = dockManager;
			m_dock = dock;
			m_contentIndex = -1;
		}
	}

	/// <summary>
	/// Summary description for DragHandler.
	/// </summary>
	internal sealed class DragHandler : DragHandlerBase
	{
		internal enum DragSources
		{
			Content,
			ContentWindow,
			FloatWindow,
			TabWindowSplitter
		}

		private const int DragBorderWidth = MeasureContentWindow.DragSize;

		private Point m_mouseOffset = Point.Empty;

		public DragHandler()
		{
			m_dropTarget = new DropTarget();
		}

		private DragSources m_dragSource;
		internal DragSources DragSource
		{
			get	{	return m_dragSource;	}
		}

		private Region m_dragFrame = null;
		internal Region DragFrame
		{
			get	{	return m_dragFrame;	}
			set
			{
				if (m_dragFrame != null)
				{
					DrawHelper.DrawDragFrame(m_dragFrame);
					m_dragFrame.Dispose();
				}

				m_dragFrame = value;
				if (m_dragFrame != null)
					DrawHelper.DrawDragFrame(m_dragFrame);
			}
		}

		private DropTarget m_dropTarget;
		public DropTarget DropTarget
		{
			get	{	return m_dropTarget;	}
		}

		public void BeginDragContent(TabWindow tabWindow, Rectangle rectTabWindow)
		{
			if (!InitDrag(tabWindow, DragSources.Content))
				return;

			Content_BeginDrag(rectTabWindow);
		}

		public void BeginDragContentWindow(TabWindow tabWindow, Point captionLocation)
		{
			if (!InitDrag(tabWindow, DragSources.ContentWindow))
				return;

			ContentWindow_BeginDrag(captionLocation);
		}

		public void BeginDragTabWindowSplitter(TabWindow tabWindow, Point splitterLocation)
		{
			if (!InitDrag(tabWindow, DragSources.TabWindowSplitter))
				return;

			TabWindowSplitter_BeginDrag(splitterLocation);
		}

		public void BeginDragFloatWindow(FloatWindow floatWindow)
		{
			if (!InitDrag(floatWindow, DragSources.FloatWindow))
				return;

			FloatWindow_BeginDrag();
		}

		private bool InitDrag(Control c, DragSources dragSource)
		{
			if (!base.BeginDrag(c))
				return false;

			m_dragSource = dragSource;
			DropTarget.Clear();
			return true;
		}

		protected override void OnDragging()
		{
			DropTarget.Clear(true);
			
			if (m_dragSource == DragSources.Content)
				Content_OnDragging();
			else if (m_dragSource == DragSources.ContentWindow)
				ContentWindow_OnDragging();
			else if (m_dragSource == DragSources.TabWindowSplitter)
				TabWindowSplitter_OnDragging();
			else if (m_dragSource == DragSources.FloatWindow)
				FloatWindow_OnDragging();
		}

		protected override void OnEndDrag(bool abort)
		{
			DragFrame = null;

			if (m_dragSource == DragSources.Content)
				Content_OnEndDrag(abort);
			else if (m_dragSource == DragSources.ContentWindow)
				ContentWindow_OnEndDrag(abort);
			else if (m_dragSource == DragSources.TabWindowSplitter)
				TabWindowSplitter_OnEndDrag(abort);
			else if (m_dragSource == DragSources.FloatWindow)
				FloatWindow_OnEndDrag(abort);
		}

		private void Content_BeginDrag(Rectangle rectTabWindow)
		{
			TabWindow tabWindow = (TabWindow)DragControl;

			Point pt;
			if (tabWindow is DocumentWindow)
				pt = new Point(rectTabWindow.Top, rectTabWindow.Left);
			else
				pt = new Point(rectTabWindow.Left, rectTabWindow.Bottom);

			pt = DragControl.PointToScreen(pt);

			m_mouseOffset.X = pt.X - StartMousePosition.X;
			m_mouseOffset.Y = pt.Y - StartMousePosition.Y;
		}

		private void Content_OnDragging()
		{
			Point ptMouse = Control.MousePosition;
			ContentWindow cw = ((TabWindow)DragControl).ContentWindow;

			if (!TestDrop(ptMouse))
				return;

			if (DropTarget.DropTo == null && IsDockStateValid(DockState.Float))
			{
				Size size = FloatWindow.DefaultWindowSize;
				Point location;
				if (cw.DockState == DockState.Document)
					location = new Point(ptMouse.X + m_mouseOffset.X, ptMouse.Y + m_mouseOffset.Y);
				else
					location = new Point(ptMouse.X + m_mouseOffset.X, ptMouse.Y + m_mouseOffset.Y - size.Height);

				if (ptMouse.X > location.X + size.Width)
					location.X += ptMouse.X - (location.X + size.Width) + DragBorderWidth;

				DragFrame = DrawHelper.CreateDragFrame(new Rectangle(location, size), DragBorderWidth);
			}

			if (DragFrame == null)
				User32.SetCursor(Cursors.No.Handle);
			else
				User32.SetCursor(DragControl.Cursor.Handle);
		}
		
		private void Content_OnEndDrag(bool abort)
		{
			User32.SetCursor(DragControl.Cursor.Handle);

			if (abort)
				return;

			Content content = ((TabWindow)DragControl).ActiveContent;

			if (DropTarget.DropTo is TabWindow)
			{
				TabWindow tabWindow = DropTarget.DropTo as TabWindow;

				if (DropTarget.Dock == DockStyle.Fill)
				{
					bool sameContentWindow = (content.ContentWindow == tabWindow.ContentWindow);
					if (!sameContentWindow)
						content.ContentWindow = tabWindow.ContentWindow;

					if (DropTarget.ContentIndex == -1 || !sameContentWindow)
						tabWindow.ContentWindow.SetContentIndex(content, DropTarget.ContentIndex);
					else
					{
						ContentCollection contents = content.ContentWindow.Contents;
						int oldIndex = contents.IndexOf(content);
						int newIndex = DropTarget.ContentIndex;
						if (oldIndex < newIndex)
						{
							newIndex += 1;
							if (newIndex > contents.Count -1)
								newIndex = -1;
						}
						tabWindow.ContentWindow.SetContentIndex(content, newIndex);
					}

					content.Activate();
				}
				else
				{
					ContentWindow cw = new ContentWindow(content, tabWindow.DockState);
					if (!tabWindow.ContentWindow.IsDisposed)
					{
						if (DropTarget.Dock == DockStyle.Left)
							tabWindow.InsertContentWindowBefore(cw, LayoutStyles.Horizontal, 0.5);
						else if (DropTarget.Dock == DockStyle.Right) 
							tabWindow.InsertContentWindowAfter(cw, LayoutStyles.Horizontal, 0.5);
						else if (DropTarget.Dock == DockStyle.Top)
							tabWindow.InsertContentWindowBefore(cw, LayoutStyles.Vertical, 0.5);
						else if (DropTarget.Dock == DockStyle.Bottom) 
							tabWindow.InsertContentWindowAfter(cw, LayoutStyles.Vertical, 0.5);
					}

					cw.Activate();
				}
			}
			else if (DropTarget.DropTo is DockManager)
			{
				ContentWindow cw;
				if (DropTarget.Dock == DockStyle.Top)
					cw = new ContentWindow(content, DockState.DockTop);
				else if (DropTarget.Dock == DockStyle.Bottom)
					cw = new ContentWindow(content, DockState.DockBottom);
				else if (DropTarget.Dock == DockStyle.Left)
					cw = new ContentWindow(content, DockState.DockLeft);
				else if (DropTarget.Dock == DockStyle.Right)
					cw = new ContentWindow(content, DockState.DockRight);
				else if (DropTarget.Dock == DockStyle.Fill)
					cw = new ContentWindow(content, DockState.Document);
				else
					return;
					
				cw.DockManager.SetContentWindowIndex(cw, 0);
				cw.Activate();
			}
			else if (IsDockStateValid(DockState.Float))
			{
				Point ptMouse = Control.MousePosition;
				ContentWindow cw = content.ContentWindow;

				Size size = FloatWindow.DefaultWindowSize;
				Point location;
				if (cw.DockState == DockState.Document)
					location = new Point(ptMouse.X + m_mouseOffset.X, ptMouse.Y + m_mouseOffset.Y);
				else
					location = new Point(ptMouse.X + m_mouseOffset.X, ptMouse.Y + m_mouseOffset.Y - size.Height);

				if (ptMouse.X > location.X + size.Width)
					location.X += ptMouse.X - (location.X + size.Width) + DragBorderWidth;

				cw = new ContentWindow(content, new Rectangle(location, size));
				cw.Activate();
			}
		}

		private void ContentWindow_BeginDrag(Point captionLocation)
		{
			Point pt = captionLocation;
			pt = DragControl.PointToScreen(pt);

			m_mouseOffset.X = pt.X - StartMousePosition.X;
			m_mouseOffset.Y = pt.Y - StartMousePosition.Y;
		}

		private void ContentWindow_OnDragging()
		{
			Point ptMouse = Control.MousePosition;
			ContentWindow cw = ((TabWindow)DragControl).ContentWindow;

			if (!TestDrop(ptMouse))
				return;

			if (DropTarget.DropTo == null && IsDockStateValid(DockState.Float))
			{
				Point location = new Point(ptMouse.X + m_mouseOffset.X, ptMouse.Y + m_mouseOffset.Y);
				Size size;
				if (cw.FloatWindow == null)
					size = FloatWindow.DefaultWindowSize;
				else if (cw.FloatWindow.ContentWindows.Count == 1)
					size = ((TabWindow)DragControl).ContentWindow.FloatWindow.Size;
				else
					size = FloatWindow.DefaultWindowSize;

				if (ptMouse.X > location.X + size.Width)
					location.X += ptMouse.X - (location.X + size.Width) + DragBorderWidth;

				DragFrame = DrawHelper.CreateDragFrame(new Rectangle(location, size), DragBorderWidth);
			}

			if (DragFrame == null)
				User32.SetCursor(Cursors.No.Handle);
			else
				User32.SetCursor(DragControl.Cursor.Handle);
		}

		private void ContentWindow_OnEndDrag(bool abort)
		{
			User32.SetCursor(DragControl.Cursor.Handle);

			if (abort)
				return;

			ContentWindow cw = ((TabWindow)DragControl).ContentWindow;

			if (DropTarget.DropTo is TabWindow)
			{
				TabWindow tabWindow = DropTarget.DropTo as TabWindow;

				if (DropTarget.Dock == DockStyle.Fill)
				{
					for (int i=cw.Contents.Count - 1; i>=0; i--)
					{
						Content c = cw.Contents[i];
						c.ContentWindow = tabWindow.ContentWindow;
						if (DropTarget.ContentIndex != -1)
							tabWindow.ContentWindow.SetContentIndex(c, DropTarget.ContentIndex);
						c.Activate();
					}
				}
				else
				{
					if (DropTarget.Dock == DockStyle.Left)
						tabWindow.InsertContentWindowBefore(cw, LayoutStyles.Horizontal, 0.5);
					else if (DropTarget.Dock == DockStyle.Right) 
						tabWindow.InsertContentWindowAfter(cw, LayoutStyles.Horizontal, 0.5);
					else if (DropTarget.Dock == DockStyle.Top)
						tabWindow.InsertContentWindowBefore(cw, LayoutStyles.Vertical, 0.5);
					else if (DropTarget.Dock == DockStyle.Bottom) 
						tabWindow.InsertContentWindowAfter(cw, LayoutStyles.Vertical, 0.5);

					cw.Activate();
				}
			}
			else if (DropTarget.DropTo is DockManager)
			{
				if (DropTarget.Dock == DockStyle.Top)
					cw.VisibleState = DockState.DockTop;
				else if (DropTarget.Dock == DockStyle.Bottom)
					cw.VisibleState = DockState.DockBottom;
				else if (DropTarget.Dock == DockStyle.Left)
					cw.VisibleState = DockState.DockLeft;
				else if (DropTarget.Dock == DockStyle.Right)
					cw.VisibleState = DockState.DockRight;
				else if (DropTarget.Dock == DockStyle.Fill)
					cw.VisibleState = DockState.Document;
					
				cw.DockManager.SetContentWindowIndex(cw, 0);
				cw.Activate();
			}
			else if (IsDockStateValid(DockState.Float))
			{
				Point ptMouse = Control.MousePosition;

				Point location = new Point(ptMouse.X + m_mouseOffset.X, ptMouse.Y + m_mouseOffset.Y);
				Size size;
				bool createFloatWindow = true;
				if (cw.FloatWindow == null)
					size = FloatWindow.DefaultWindowSize;
				else if (cw.FloatWindow.ContentWindows.Count == 1)
				{
					size = ((TabWindow)DragControl).ContentWindow.FloatWindow.Size;
					createFloatWindow = false;
				}
				else
					size = FloatWindow.DefaultWindowSize;

				if (ptMouse.X > location.X + size.Width)
					location.X += ptMouse.X - (location.X + size.Width) + DragBorderWidth;

				if (createFloatWindow)
					cw.FloatWindow = new FloatWindow(cw.DockManager, cw, new Rectangle(location, size));
				else
					cw.FloatWindow.Bounds = new Rectangle(location, size);

				cw.VisibleState = DockState.Float;
				cw.Activate();
			}
		}

		private Point m_splitterLocation;
		private void TabWindowSplitter_BeginDrag(Point ptSplitter)
		{
			m_splitterLocation = ptSplitter;
			m_splitterLocation = DragControl.PointToScreen(m_splitterLocation);
			Point ptMouse = StartMousePosition;

			m_mouseOffset.X = m_splitterLocation.X - ptMouse.X;
			m_mouseOffset.Y = m_splitterLocation.Y - ptMouse.Y;

			Rectangle rect = TabWindowSplitter_GetDragRectangle();
			DragFrame = DrawHelper.CreateDragFrame(rect, DragBorderWidth);
		}
		private void TabWindowSplitter_OnDragging()
		{
			Rectangle rect = TabWindowSplitter_GetDragRectangle();
			DragFrame = DrawHelper.CreateDragFrame(rect, DragBorderWidth);
		}
		private void TabWindowSplitter_OnEndDrag(bool abort)
		{
			if (abort)
				return;

			Point pt = m_splitterLocation;
			Rectangle rect = TabWindowSplitter_GetDragRectangle();
			TabWindow tabWindow = DragControl as TabWindow;
			DockManager dockManager = tabWindow.DockManager;
			DockState state = tabWindow.DockState;
			if (tabWindow.IsTopLevel)
			{
				Rectangle rectDockArea = dockManager.DockArea;
				Content c = dockManager.ActiveAutoHideContent;
				if (state == DockState.DockLeft && rectDockArea.Width > 0)
					dockManager.DockLeftPortion += ((double)rect.X - (double)pt.X) / (double)rectDockArea.Width;
				else if (state == DockState.DockRight && rectDockArea.Width > 0)
					dockManager.DockRightPortion += ((double)pt.X - (double)rect.X) / (double)rectDockArea.Width;
				else if (state == DockState.DockBottom && rectDockArea.Height > 0)
					dockManager.DockBottomPortion += ((double)pt.Y - (double)rect.Y) / (double)rectDockArea.Height;
				else if (state == DockState.DockTop && rectDockArea.Height > 0)
					dockManager.DockTopPortion += ((double)rect.Y - (double)pt.Y) / (double)rectDockArea.Height;
				else if (state == DockState.DockLeftAutoHide && rectDockArea.Width > 0)
					c.AutoHidePortion += ((double)rect.X - (double)pt.X) / (double)rectDockArea.Width;
				else if (state == DockState.DockRightAutoHide && rectDockArea.Width > 0)
					c.AutoHidePortion += ((double)pt.X - (double)rect.X) / (double)rectDockArea.Width;
				else if (state == DockState.DockBottomAutoHide && rectDockArea.Height > 0)
					c.AutoHidePortion += ((double)pt.Y - (double)rect.Y) / (double)rectDockArea.Height;
				else if (state == DockState.DockTopAutoHide && rectDockArea.Height > 0)
					c.AutoHidePortion += ((double)rect.Y - (double)pt.Y) / (double)rectDockArea.Height;
			}
			else
			{
				TabWindow tabWindowPrev = tabWindow.Previous;
				double sizeAdjust;
				if (tabWindowPrev.LayoutStyle == LayoutStyles.Horizontal && tabWindowPrev.ClientRectangle.Width > 0)
					sizeAdjust = ((double)rect.X - (double)pt.X) / (double)tabWindowPrev.ClientRectangle.Width;
				else
					sizeAdjust = ((double)rect.Y - (double)pt.Y) / (double)tabWindowPrev.ClientRectangle.Height;

				tabWindowPrev.LayoutSize += sizeAdjust;
				tabWindowPrev.Invalidate();
				tabWindowPrev.PerformLayout();
			}
		}
		private Rectangle TabWindowSplitter_GetDragRectangle()
		{
			TabWindow tabWindow = DragControl as TabWindow;
			DockManager dockManager = tabWindow.DockManager;

			DockStyle splitterDockStyle = tabWindow.SplitterDockStyle;
			Rectangle rectParentClient;
			if (tabWindow.IsTopLevel)
			{
				rectParentClient = dockManager.ClientRectangle;
				if (splitterDockStyle == DockStyle.Left || splitterDockStyle == DockStyle.Right)
				{
					rectParentClient.X += dockManager.DockPadding.Left + MeasureContentWindow.MinSize;
					rectParentClient.Width -= dockManager.DockPadding.Left + dockManager.DockPadding.Right + 2 * MeasureContentWindow.MinSize;
				}
				else if (splitterDockStyle == DockStyle.Top || splitterDockStyle == DockStyle.Bottom)
				{
					rectParentClient.Y += dockManager.DockPadding.Top + MeasureContentWindow.MinSize;
					rectParentClient.Height -= dockManager.DockPadding.Top + dockManager.DockPadding.Bottom + 2 * MeasureContentWindow.MinSize;
				}
				rectParentClient.Location = dockManager.PointToScreen(rectParentClient.Location);
			}
			else
			{
				TabWindow tabWindowPrev = tabWindow.Previous;
				rectParentClient = tabWindowPrev.ClientRectangle;
				rectParentClient.Location = tabWindowPrev.PointToScreen(rectParentClient.Location);
				if (tabWindowPrev.LayoutStyle == LayoutStyles.Horizontal)
				{
					rectParentClient.X += MeasureContentWindow.MinSize;
					rectParentClient.Width -= 2 * MeasureContentWindow.MinSize;
				}
				else
				{
					rectParentClient.Y += MeasureContentWindow.MinSize;
					rectParentClient.Height -= 2 * MeasureContentWindow.MinSize;
				}
			}

			Point ptMouse = Control.MousePosition;
			Point pt = m_splitterLocation;
			Rectangle rect = Rectangle.Empty;
			if (splitterDockStyle == DockStyle.Left || splitterDockStyle == DockStyle.Right)
			{
				rect.X = ptMouse.X + m_mouseOffset.X;
				rect.Y = pt.Y;
				rect.Width = DragBorderWidth;
				rect.Height = DragControl.ClientRectangle.Height;
			}
			else if (splitterDockStyle == DockStyle.Top || splitterDockStyle == DockStyle.Bottom)
			{
				rect.X = pt.X;
				rect.Y = ptMouse.Y + m_mouseOffset.Y;
				rect.Width = DragControl.ClientRectangle.Width;
				rect.Height = DragBorderWidth;
			}

			if (rectParentClient.Width <= 0 || rectParentClient.Height <= 0)
			{
				rect.X = pt.X;
				rect.Y = pt.Y;
				return rect;
			}

			if (rect.Left < rectParentClient.Left)
				rect.X = rectParentClient.X;
			if (rect.Top < rectParentClient.Top)
				rect.Y = rectParentClient.Y;
			if (rect.Right > rectParentClient.Right)
				rect.X -= rect.Right - rectParentClient.Right;
			if (rect.Bottom > rectParentClient.Bottom)
				rect.Y -= rect.Bottom - rectParentClient.Bottom;

			return rect;
		}

		private void FloatWindow_BeginDrag()
		{
			m_mouseOffset.X = DragControl.Bounds.X - StartMousePosition.X;
			m_mouseOffset.Y = DragControl.Bounds.Y - StartMousePosition.Y;
		}
		private void FloatWindow_OnDragging()
		{
			Point ptMouse = Control.MousePosition;

			if (!TestDrop(ptMouse))
				return;

			if (DropTarget.DropTo == null)
			{
				Rectangle rect = DragControl.Bounds;
				rect.X = ptMouse.X + m_mouseOffset.X;
				rect.Y = ptMouse.Y + m_mouseOffset.Y;
				DragFrame = DrawHelper.CreateDragFrame(rect, DragBorderWidth);
			}
		}
		private void FloatWindow_OnEndDrag(bool abort)
		{
			if (abort)
				return;

			FloatWindow floatWindow = (FloatWindow)DragControl;

			if (DropTarget.DropTo == null)
			{
				Rectangle rect = DragControl.Bounds;
				rect.X = Control.MousePosition.X + m_mouseOffset.X;
				rect.Y = Control.MousePosition.Y + m_mouseOffset.Y;
				DragControl.Bounds = rect;
			}
			else if (DropTarget.DropTo is TabWindow)
			{
				TabWindow tabWindow = DropTarget.DropTo as TabWindow;

				if (DropTarget.Dock == DockStyle.Fill)
				{
					for (int i=floatWindow.ContentWindows.Count-1; i>=0; i--)
					{
						ContentWindow cw = floatWindow.ContentWindows[i];
						for (int j=cw.Contents.Count - 1; j>=0; j--)
						{
							Content c = cw.Contents[j];
							c.ContentWindow = tabWindow.ContentWindow;
							if (DropTarget.ContentIndex != -1)
								tabWindow.ContentWindow.SetContentIndex(c, DropTarget.ContentIndex);
							c.Activate();
						}
					}
				}
				else
				{
					TabWindow lastTabWindow = null;
					for (int i=floatWindow.ContentWindows.Count-1; i>=0; i--)
					{
						ContentWindow cw = floatWindow.ContentWindows[i];
						if (i==floatWindow.ContentWindows.Count-1)
						{
							if (DropTarget.Dock == DockStyle.Left)
								tabWindow.InsertContentWindowBefore(cw, LayoutStyles.Horizontal, 0.5);
							else if (DropTarget.Dock == DockStyle.Right) 
								tabWindow.InsertContentWindowAfter(cw, LayoutStyles.Horizontal, 0.5);
							else if (DropTarget.Dock == DockStyle.Top)
								tabWindow.InsertContentWindowBefore(cw, LayoutStyles.Vertical, 0.5);
							else if (DropTarget.Dock == DockStyle.Bottom) 
								tabWindow.InsertContentWindowAfter(cw, LayoutStyles.Vertical, 0.5);
						}
						else
							lastTabWindow.InsertContentWindowBefore(cw, cw.TabWindow.LayoutStyle, cw.TabWindow.LayoutSize);

						lastTabWindow = cw.TabWindow;
					}
				}
			}
			else if (DropTarget.DropTo is DockManager)
			{
				for (int i=floatWindow.ContentWindows.Count - 1; i>=0; i--)
				{
					ContentWindow cw = floatWindow.ContentWindows[i];
					if (DropTarget.Dock == DockStyle.Top)
						cw.VisibleState = DockState.DockTop;
					else if (DropTarget.Dock == DockStyle.Bottom)
						cw.VisibleState = DockState.DockBottom;
					else if (DropTarget.Dock == DockStyle.Left)
						cw.VisibleState = DockState.DockLeft;
					else if (DropTarget.Dock == DockStyle.Right)
						cw.VisibleState = DockState.DockRight;
					else if (DropTarget.Dock == DockStyle.Fill)
						cw.VisibleState = DockState.Document;
					
					cw.DockManager.SetContentWindowIndex(cw, 0);
					cw.LayoutDock = cw.LayoutFloat;
					cw.SizeDock = cw.SizeFloat;
					floatWindow.ContentWindows[i].Activate();
				}
			}
		}

		private TabWindow TabWindowAtPoint(Point pt)
		{
			Control control;

			for (control = ControlAtPoint(pt); control != null; control = control.Parent)
			{
				if (control is TabWindow)
					return control as TabWindow;
			}

			return null;
		}

		private DockManager DockManagerAtPoint(Point pt)
		{
			return ControlAtPoint(pt) as DockManager;
		}

		private Control ControlAtPoint(Point pt)
		{
			Win32.POINT pt32;
			pt32.x = pt.X;
			pt32.y = pt.Y;

			return Control.FromChildHandle(User32.WindowFromPoint(pt32));
		}

		public bool IsDockStateValid(DockState dockState)
		{
			if (DragSource == DragSources.FloatWindow)
				return ((FloatWindow)DragControl).IsDockStateValid(dockState);
			else if (DragSource == DragSources.ContentWindow)
				return ((TabWindow)DragControl).ContentWindow.IsDockStateValid(dockState);
			else if (DragSource == DragSources.Content)
				return ((TabWindow)DragControl).ActiveContent.IsDockStateValid(dockState);
			else
				return false;
		}

		private bool TestDrop(Point ptMouse)
		{
			if (DockManagerAtPoint(ptMouse) != null)
				DockManagerAtPoint(ptMouse).TestDrop(this, ptMouse);

			if (DropTarget.DropTo == null)
				if (TabWindowAtPoint(ptMouse) != null)
					TabWindowAtPoint(ptMouse).TestDrop(this, ptMouse);

			return (!DropTarget.SameAsOldValue);
		}
	}
}
