using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace ExportNameString
{
    class Argument
    {
        private string[] args;
        private string currentKey = "";
        private Dictionary<string, List<string>> commands = new Dictionary<string, List<string>>();        

        public Argument(string[] args)
        {
            this.args = args;
        }

        public bool Parser()
        {         
            foreach(string str in args)
            {
                if (IsKey(str))
                {
                    PushKey(str);
                }
                else
                {
                    PushValue(str);
                }
            }

            return true;
        }
        
        public bool HasKey(string key)
        {
            return commands.ContainsKey(key);
        }

        public string[] GetValue(string key)
        {
            if (!commands.ContainsKey(key))
            {
                return null;
            }

            return commands[key].ToArray(); 
        }

        private bool IsKey(string str)
        {
            if (0 == str.IndexOf("-"))
            {
                return true;
            }

            return false;
        }

        private void PushKey(string key)
        {
            currentKey = key.TrimStart('-');
            commands.Add(currentKey, new List<string>());            
        }

        private void PushValue(string value)
        {
            foreach (string str in value.Split(','))
            {
                if (str != "")
                {
                    if (!commands.ContainsKey(currentKey))
                    {
                        commands.Add(currentKey, new List<string>());            
                    }

                    commands[currentKey].Add(str);
                }
            }
        }        
    }
}
