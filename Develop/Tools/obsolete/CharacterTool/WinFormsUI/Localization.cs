using System;
using System.ComponentModel;

namespace WeifenLuo.WinFormsUI
{
	[AttributeUsage(AttributeTargets.All)]
	internal class LocalizedDescriptionAttribute : DescriptionAttribute
	{
		private bool m_initialized = false;

		public LocalizedDescriptionAttribute(string key) : base(key)
		{
		}

		public override string Description
		{
			get
			{	
				if (!m_initialized)
				{
					string key = base.Description;
					DescriptionValue = ResourceHelper.GetString(key);
					m_initialized = true;
				}

				return DescriptionValue;
			}
		}
	}

	[AttributeUsage(AttributeTargets.All)]
	internal class LocalizedCategoryAttribute : CategoryAttribute
	{
		public LocalizedCategoryAttribute(string key) : base(key)
		{
		}

		protected override string GetLocalizedString(string key)
		{
			return ResourceHelper.GetString(key);
		}
	}
}
