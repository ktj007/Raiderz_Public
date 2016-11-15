using System;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.ComponentModel;
using System.Drawing.Design;

namespace SoulHunt
{
    class VCETextEditor : UITypeEditor
    {        
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            if (context == null || provider == null) return base.EditValue(context, provider, value);

            IWindowsFormsEditorService editorService = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
            if (editorService != null)
            {
                SHCE_TextEditor editor = new SHCE_TextEditor();
                if (value != null)
                {
                    editor.SetText((String)value);
                }
                if (editorService.ShowDialog(editor) == DialogResult.OK)
                {
                    return editor.GetText();
                }
            }

            return base.EditValue(context, provider, value);
        }

        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            if(context != null) return UITypeEditorEditStyle.Modal;
            return base.GetEditStyle(context);
        }
    }

    class VCETalentEditor : UITypeEditor
    {
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            if (context == null || provider == null) return base.EditValue(context, provider, value);

            IWindowsFormsEditorService editorService = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
            if (editorService != null)
            {
                SHCE_ObjBrowser editor = new SHCE_ObjBrowser();
                editor.SetDataType(VOBType.Talent);

                if (value != null) editor.SetTag(value.ToString());

                if (editorService.ShowDialog(editor) == DialogResult.OK)
                {
                    int nRet;
                    return (editor.GetTag(out nRet)) ? nRet : 0;
                }
            }

            return base.EditValue(context, provider, value);
        }

        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            if (context != null) return UITypeEditorEditStyle.Modal;
            return base.GetEditStyle(context);
        }
    }

    class VCEItemEditor : UITypeEditor
    {
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            if (context == null || provider == null) return base.EditValue(context, provider, value);

            IWindowsFormsEditorService editorService = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
            if (editorService != null)
            {
                SHCE_ObjBrowser editor = new SHCE_ObjBrowser();
                editor.SetDataType(VOBType.Item);

                if (value != null) editor.SetTag(value.ToString());

                if (editorService.ShowDialog(editor) == DialogResult.OK)
                {
                    int nRet;
                    return (editor.GetTag(out nRet)) ? nRet : 0;
                }
            }

            return base.EditValue(context, provider, value);
        }

        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            if (context != null) return UITypeEditorEditStyle.Modal;
            return base.GetEditStyle(context);
        }
    }

    class VCEQuestEditor : UITypeEditor
    {
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            if (context == null || provider == null) return base.EditValue(context, provider, value);

            IWindowsFormsEditorService editorService = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
            if (editorService != null)
            {
                SHCE_ObjBrowser editor = new SHCE_ObjBrowser();
                editor.SetDataType(VOBType.Quest);

                if (value != null) editor.SetTag(value.ToString());

                if (editorService.ShowDialog(editor) == DialogResult.OK)
                {
                    int nRet;
                    return (editor.GetTag(out nRet)) ? nRet : 0;
                }
            }

            return base.EditValue(context, provider, value);
        }

        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            if (context != null) return UITypeEditorEditStyle.Modal;
            return base.GetEditStyle(context);
        }
    }
}