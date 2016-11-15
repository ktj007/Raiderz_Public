/* All code is written my me,Ben Ratzlaff and is available for any free use except where noted. 
 *I assume no responsibility for how anyone uses the information available*/

using System;
using System.ComponentModel;
using System.Windows.Forms.Design;

namespace CustomPropGrid
{
    /// <summary>
	/// Stores information to be displayed in the CustomPropertyGrid
	/// </summary>
	public class Setting
	{
        public enum PROPERTY_EDITOR
        {
            PE_NONE,
            PE_FILENAME,
            PE_COLOR,
        };

		public object val;
		private string desc,category,name;

		public event EventHandler ValueChanged;

        public PROPERTY_EDITOR ePropertyEditor;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="val">The current value of the setting</param>
		/// <param name="desc">The setting's description</param>
		/// <param name="category">The setting's category</param>
		/// <param name="update">An eventhandler that will be called if CustomPropertyGrid.InstantUpdate is true</param>
        /// <param name="eEditor">Property Editor</param>
		public Setting(object val,string desc,string category,EventHandler update, PROPERTY_EDITOR eEditor)
		{
			this.val=val;
			this.desc=desc;
			this.category=category;
			if(update!=null)
				ValueChanged+=update;

            ePropertyEditor = eEditor;
		}

		#region Other constructors that call the one above
        public Setting(object val, string desc, string category, EventHandler update) : this(val, desc, category, update, PROPERTY_EDITOR.PE_NONE) { }
		public Setting(object val,string desc,string category):this(val,desc,category,null,PROPERTY_EDITOR.PE_NONE){}
		public Setting(object val, string desc):this(val,desc,null,null,PROPERTY_EDITOR.PE_NONE){}
		public Setting(object val):this(val,null,null,null,PROPERTY_EDITOR.PE_NONE){}
		#endregion

		#region get/set properties for the private data
		public object Value
		{
			get{return val;}
			set{val=value;}
		}

		public string Description
		{
			get{return desc;}
			set{desc=value;}
		}

		public string Category
		{
			get{return category;}
			set{category=value;}
		}

		public string Name
		{
			get{return name;}
			set{name=value;}
		}

        #endregion

		/// <summary>
		/// Allows an external object to force calling the event
		/// </summary>
		/// <param name="e"></param>
		public void FireUpdate(EventArgs e)
		{
			//I didnt do this in the Value's set method because sometimes I want to set the Value without firing the event
			//I could do the same thing with a second property, but this works fine.
			if(ValueChanged!=null)
				ValueChanged(this,e);
		}
	}
}
