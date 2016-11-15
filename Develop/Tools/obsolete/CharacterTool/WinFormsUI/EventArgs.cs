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

namespace WeifenLuo.WinFormsUI
{
	public class ContentEventArgs : EventArgs
	{
		public Content m_content = null;

		public ContentEventArgs(Content c)
		{
			m_content = c;
		}

		public Content Content
		{
			get	{	return m_content;	}
		}
	}
}
