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
	/// Summary description for ContentCollection.
	/// </summary>
	public class ContentWindowCollection : ReadOnlyCollectionBase
	{
		internal ContentWindowCollection()
		{
		}

		public ContentWindow this[int index]
		{
			get {  return InnerList[index] as ContentWindow;  }
		}

		internal int Add(ContentWindow cw)
		{
			if (InnerList.Contains(cw))
				return InnerList.IndexOf(cw);

			return InnerList.Add(cw);
		}

		internal void AddAt(ContentWindow cw, int index)
		{
			if (index < 0 || index > InnerList.Count - 1)
				return;
			
			if (Contains(cw))
				return;

			InnerList.Insert(index, cw);
		}

		internal void AddAt(ContentWindow cw, ContentWindow beforeContentWindow)
		{
			AddAt(cw, IndexOf(beforeContentWindow));
		}

		internal void Add(ContentWindow cw, ContentWindow beforeContentWindow)
		{
			if (beforeContentWindow == null)
				Add(cw);
			else
				InnerList.Insert(IndexOf(beforeContentWindow), cw);
		}

		public bool Contains(ContentWindow cw)
		{
			return InnerList.Contains(cw);
		}

		internal void Dispose()
		{
			for (int i=Count - 1; i>=0; i--)
				this[i].Close();
		}

		public int IndexOf(ContentWindow cw)
		{
			return InnerList.IndexOf(cw);
		}

		internal void Remove(ContentWindow cw)
		{
			InnerList.Remove(cw);
		}
	}
}
