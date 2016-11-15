/* All code is written my me,Ben Ratzlaff and is available for any free use except where noted. 
 *I assume no responsibility for how anyone uses the information available*/

//uncomment this line to have the program save the emitted assembly to Test.dll
//#define SaveDLL

using System;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Drawing.Design;
using System.Reflection.Emit;
using System.Threading;
using System.Reflection;

namespace CustomPropGrid
{
	/// <summary>
	/// A property grid that dynamically generates a Type to conform to desired input
	/// </summary>
	public class CustomPropertyGrid : System.Windows.Forms.PropertyGrid
	{
		private Hashtable typeHash;
		private string typeName="DefType";
		private Settings settings;
		private bool instantUpdate=true;

		public CustomPropertyGrid()
		{		
			initTypes();
		}

		[Description("Name of the type that will be internally created")]
		[DefaultValue("DefType")]
		public string TypeName
		{
			get{return typeName;}
			set{typeName=value;}
		}

		[DefaultValue(true)]
		[Description("If true, the Setting.Update() event will be called when a property changes")]
		public bool InstantUpdate
		{
			get{return instantUpdate;}
			set{instantUpdate=value;}
		}

		protected override void OnPropertyValueChanged(PropertyValueChangedEventArgs e)
		{
			base.OnPropertyValueChanged(e);

			((Setting)settings[e.ChangedItem.Label]).Value=e.ChangedItem.Value;

			if(instantUpdate)
				((Setting)settings[e.ChangedItem.Label]).FireUpdate(e);
		}

		[Browsable(false)]
		public Settings Settings
		{
			set
			{
				settings=value;

				//Reflection.Emit code below copied and modified from http://longhorn.msdn.microsoft.com/lhsdk/ref/ns/system.reflection.emit/c/propertybuilder/propertybuilder.aspx

				AppDomain myDomain = Thread.GetDomain();
				AssemblyName myAsmName = new AssemblyName();
				myAsmName.Name = "TempAssembly";

				//Only save the custom-type dll while debugging
#if SaveDLL && DEBUG
				AssemblyBuilder assemblyBuilder = myDomain.DefineDynamicAssembly(myAsmName,AssemblyBuilderAccess.RunAndSave);
				ModuleBuilder moduleBuilder = assemblyBuilder.DefineDynamicModule("TempModule","Test.dll");
#else
				AssemblyBuilder assemblyBuilder = myDomain.DefineDynamicAssembly(myAsmName,AssemblyBuilderAccess.Run);
				ModuleBuilder moduleBuilder = assemblyBuilder.DefineDynamicModule("TempModule");
#endif

				//create our type
				TypeBuilder newType = moduleBuilder.DefineType(typeName, TypeAttributes.Public);

				//create the hashtable used to store property values
				FieldBuilder hashField = newType.DefineField("table",typeof(Hashtable),FieldAttributes.Private);            
				createHashMethod(newType.DefineProperty("Hash",PropertyAttributes.None,typeof(Hashtable),new Type[] {}),
					newType,hashField);  
          
				Hashtable h = new Hashtable();
				foreach(string key in settings.Keys)
				{
					Setting s = settings[key];
					h[key]=s.Value;
					emitProperty(newType,hashField,s,key);			
				}

				Type myType = newType.CreateType();
#if SaveDLL && DEBUG
				assemblyBuilder.Save("Test.dll");
#endif
				ConstructorInfo ci = myType.GetConstructor(new Type[]{});
				object o = ci.Invoke(new Object[]{});

				//set the object's hashtable - in the future i would like to do this in the emitted object's constructor
				PropertyInfo pi = myType.GetProperty("Hash");			
				pi.SetValue(o,h,null);

				SelectedObject=o;
			}
		}

		private void createHashMethod(PropertyBuilder propBuild,TypeBuilder typeBuild,FieldBuilder hash)
		{
			// First, we'll define the behavior of the "get" property for Hash as a method.
			MethodBuilder typeHashGet = typeBuild.DefineMethod("GetHash",
				MethodAttributes.Public,	
				typeof(Hashtable),
				new Type[] { });
			ILGenerator ilg = typeHashGet.GetILGenerator();
			ilg.Emit(OpCodes.Ldarg_0);
			ilg.Emit(OpCodes.Ldfld, hash);
			ilg.Emit(OpCodes.Ret);

			// Now, we'll define the behavior of the "set" property for Hash.
			MethodBuilder typeHashSet = typeBuild.DefineMethod("SetHash",
				MethodAttributes.Public,	
				null,
				new Type[] { typeof(Hashtable) });

			ilg = typeHashSet.GetILGenerator();
			ilg.Emit(OpCodes.Ldarg_0);
			ilg.Emit(OpCodes.Ldarg_1);
			ilg.Emit(OpCodes.Stfld, hash);
			ilg.Emit(OpCodes.Ret);

			// map the two methods created above to their property
			propBuild.SetGetMethod(typeHashGet);
			propBuild.SetSetMethod(typeHashSet);

			//add the [Browsable(false)] property to the Hash property so it doesnt show up on the property list
			ConstructorInfo ci = typeof(BrowsableAttribute).GetConstructor(new Type[]{typeof(bool)});
			CustomAttributeBuilder cab = new CustomAttributeBuilder(ci,new object[]{false});
			propBuild.SetCustomAttribute(cab);
		}

		/// <summary>
		/// Initialize a private hashtable with type-opCode pairs so i dont have to write a long if/else statement when outputting msil
		/// </summary>
		private void initTypes()
		{
			typeHash=new Hashtable();
			typeHash[typeof(sbyte)]=OpCodes.Ldind_I1;
			typeHash[typeof(byte)]=OpCodes.Ldind_U1;
			typeHash[typeof(char)]=OpCodes.Ldind_U2;
			typeHash[typeof(short)]=OpCodes.Ldind_I2;
			typeHash[typeof(ushort)]=OpCodes.Ldind_U2;
			typeHash[typeof(int)]=OpCodes.Ldind_I4;
			typeHash[typeof(uint)]=OpCodes.Ldind_U4;
			typeHash[typeof(long)]=OpCodes.Ldind_I8;
			typeHash[typeof(ulong)]=OpCodes.Ldind_I8;
			typeHash[typeof(bool)]=OpCodes.Ldind_I1;
			typeHash[typeof(double)]=OpCodes.Ldind_R8;
			typeHash[typeof(float)]=OpCodes.Ldind_R4;
		}

		/// <summary>
		/// emits a generic get/set property in which the result returned resides in a hashtable whos key is the name of the property
		/// </summary>
		/// <param name="pb">PropertyBuilder used to emit</param>
		/// <param name="tb">TypeBuilder of the class</param>
		/// <param name="hash">FieldBuilder of the hashtable used to store the object</param>
		/// <param name="po">PropertyObject of this property</param>
		private void emitProperty(TypeBuilder tb,FieldBuilder hash,Setting s,string name)
		{
			//to figure out what opcodes to emit, i would compile a small class having the functionality i wanted, and viewed it with ildasm.
			//peverify is also kinda nice to use to see what errors there are. 
			
			//define the property first
			PropertyBuilder pb = tb.DefineProperty(name,PropertyAttributes.None,s.Value.GetType(),new Type[] {});
			Type objType = s.Value.GetType();

			//now we define the get method for the property
			MethodBuilder getMethod = tb.DefineMethod("get_"+name,MethodAttributes.Public,objType,new Type[]{});
			ILGenerator ilg = getMethod.GetILGenerator();
			ilg.DeclareLocal(objType);			
			ilg.Emit(OpCodes.Ldarg_0);			
			ilg.Emit(OpCodes.Ldfld,hash);		
			ilg.Emit(OpCodes.Ldstr,name);		
												
			ilg.EmitCall(OpCodes.Callvirt,typeof(Hashtable).GetMethod("get_Item"),null);
			if(objType.IsValueType)
			{
				ilg.Emit(OpCodes.Unbox,objType);	
				if(typeHash[objType]!=null)
					ilg.Emit((OpCode)typeHash[objType]);	
				else
					ilg.Emit(OpCodes.Ldobj,objType);		
			}
			else
				ilg.Emit(OpCodes.Castclass,objType);		

			ilg.Emit(OpCodes.Stloc_0);			
			ilg.Emit(OpCodes.Br_S,(byte)0);		
			ilg.Emit(OpCodes.Ldloc_0);
			ilg.Emit(OpCodes.Ret);				

			//now we generate the set method for the property
			MethodBuilder setMethod = tb.DefineMethod("set_"+name,MethodAttributes.Public,null,new Type[]{objType});
			ilg = setMethod.GetILGenerator();
			ilg.Emit(OpCodes.Ldarg_0);		
			ilg.Emit(OpCodes.Ldfld,hash);	
			ilg.Emit(OpCodes.Ldstr,name);	
			ilg.Emit(OpCodes.Ldarg_1);		
			if(objType.IsValueType) 
				ilg.Emit(OpCodes.Box,objType);									
			ilg.EmitCall(OpCodes.Callvirt,typeof(Hashtable).GetMethod("set_Item"),null);
			ilg.Emit(OpCodes.Ret);	

			//put the get/set methods in with the property
			pb.SetGetMethod(getMethod);
			pb.SetSetMethod(setMethod);

			//if we specified a description, we will now add the DescriptionAttribute to our property
			if(s.Description!=null)
			{
				ConstructorInfo ci = typeof(DescriptionAttribute).GetConstructor(new Type[]{typeof(string)});
				CustomAttributeBuilder cab = new CustomAttributeBuilder(ci,new object[]{s.Description});
				pb.SetCustomAttribute(cab);
			}
            
            //add a CategoryAttribute if specified
			if(s.Category!=null)
			{
				ConstructorInfo ci = typeof(CategoryAttribute).GetConstructor(new Type[]{typeof(string)});
				CustomAttributeBuilder cab = new CustomAttributeBuilder(ci,new object[]{s.Category});
				pb.SetCustomAttribute(cab);
			}

            if(s.ePropertyEditor == Setting.PROPERTY_EDITOR.PE_FILENAME)
            {
                ConstructorInfo ci = typeof(EditorAttribute).GetConstructor(new Type[] { typeof(Type), typeof(Type) });
                CustomAttributeBuilder cab = new CustomAttributeBuilder(ci, new object[] { typeof(System.Windows.Forms.Design.FileNameEditor), typeof(System.Drawing.Design.UITypeEditor) });
                pb.SetCustomAttribute(cab);
            }
            else if (s.ePropertyEditor == Setting.PROPERTY_EDITOR.PE_COLOR)
            {
                ConstructorInfo ci = typeof(EditorAttribute).GetConstructor(new Type[] { typeof(Type), typeof(Type) });
                CustomAttributeBuilder cab = new CustomAttributeBuilder(ci, new object[] { typeof(System.Drawing.Design.ColorEditor), typeof(System.Drawing.Design.UITypeEditor) });
                pb.SetCustomAttribute(cab);
            }
		}
	}
}
