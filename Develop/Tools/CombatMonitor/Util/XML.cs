using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml.Serialization;
using System.Windows.Forms;

namespace CombatMonitor.Util
{
    class XML
    {
        #region XML Serialization
        // XML 데이터를 읽도록 만드는 메소드
        public static T Deserialize<T>(string filename)
        {
            return Deserialize<T>(filename, Encoding.UTF8);
        }

        public static T Deserialize<T>(string filename, Encoding encoding)
        {
            TextReader reader = null;
            T data = default(T);
            try
            {
                reader = new StreamReader(filename, encoding);
                XmlSerializer serializer = new XmlSerializer(typeof(T));
                data = (T)serializer.Deserialize(reader);
            }
            catch (Exception e)
            {
                MessageBox.Show("에러: [" + filename + "] " + e.Message + "/" + e.InnerException + "XML Deserializer 에러");
            }
            finally
            {
                if (reader != null)
                {
                    reader.Close();
                }
            }
                        
            return data;
        }

        public static void Serialize<T>(string filename, ref T o)
        {
            Serialize<T>(filename, ref o, Encoding.UTF8);
        }

        public static void Serialize<T>(string filename, ref T o, Encoding encoding)
        {
            TextWriter writer = null;
            try
            {
                writer = new StreamWriter(filename, false, encoding);
                XmlSerializer serializer = new XmlSerializer(typeof(T));
                serializer.Serialize(writer, o);
            }
            catch (Exception e)
            {
                MessageBox.Show("에러: [" + filename + "] " + e.Message + "/" + e.InnerException + "XML Serializer 에러");
            }
            finally
            {
                if (writer != null)
                {
                    writer.Close();
                }
            }
        }

        #endregion
    }
}
