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

namespace WeifenLuo.WinFormsUI
{
	/// <summary>
	/// Summary description for ContentCollection.
	/// </summary>
	public class ContentCollection : ReadOnlyCollectionBase
	{
		internal ContentCollection()
		{
		}

		public Content this[int index]
		{
			get {  return InnerList[index] as Content;  }
		}

		internal int Add(Content c)
		{
			if (Contains(c))
				return IndexOf(c);

			return InnerList.Add(c);
		}

		internal void AddAt(Content c, int index)
		{
			if (index < 0 || index > InnerList.Count - 1)
				return;

			if (Contains(c))
				return;

			InnerList.Insert(index, c);
		}

		internal void AddAt(Content c, Content beforeContent)
		{
			if (!Contains(beforeContent))
				return;

			if (Contains(c))
				return;

			AddAt(c, IndexOf(beforeContent));
		}

		internal void Clear()
		{
			InnerList.Clear();
		}

		public bool Contains(Content c)
		{
			return InnerList.Contains(c);
		}

		internal void Dispose()
		{
			for (int i=Count - 1; i>=0; i--)
				this[i].Dispose();
		}

		public int IndexOf(Content c)
		{
			if (!Contains(c))
				return -1;
			else
				return InnerList.IndexOf(c);
		}

		internal void Remove(Content c)
		{
			if (!Contains(c))
				return;

			InnerList.Remove(c);
		}

		public Content[] Select(ContentStates stateFilter)
		{
			int count = 0;
			foreach (Content c in this)
				if (DockHelper.IsDockStateValid(c.DockState, stateFilter))
					count ++;

			Content[] contents = new Content[count];

			count = 0;
			foreach (Content c in this)
				if (DockHelper.IsDockStateValid(c.DockState, stateFilter))
					contents[count++] = c;

			return contents;
		}
	}
}
