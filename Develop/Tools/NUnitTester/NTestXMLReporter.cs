using System;
using System.Globalization;
using System.IO;
using System.Xml;
using System.Reflection;
using NUnit.Core;
using NUnit.Util;

namespace NUnitTester
{
    class NTestXMLReporter : ResultVisitor
    {
        private XmlTextWriter m_xmlWriter;
        private TextWriter m_writer;
        private MemoryStream m_memoryStream;

        private string m_strSuiteName;

        public NTestXMLReporter(TextWriter writer, TestResult result)
		{
			this.m_memoryStream = new MemoryStream();
			this.m_writer = writer;
            this.m_xmlWriter = new XmlTextWriter(new StreamWriter(m_memoryStream, System.Text.Encoding.UTF8));
			Initialize( result );
		}

		private void Initialize(TestResult result) 
		{
			ResultSummarizer summaryResults = new ResultSummarizer(result);

			m_xmlWriter.Formatting = Formatting.Indented;
			m_xmlWriter.WriteStartDocument(false);
			m_xmlWriter.WriteStartElement("unittest-results");

			m_xmlWriter.WriteAttributeString("tests", summaryResults.ResultCount.ToString());
			m_xmlWriter.WriteAttributeString("failedtests", summaryResults.FailureCount.ToString());
            m_xmlWriter.WriteAttributeString("failures", summaryResults.FailureCount.ToString());

            float fTime = DateTime.Now.Millisecond * 0.001f;
            m_xmlWriter.WriteAttributeString("time", fTime.ToString());
		}

        #region String 처리 유틸리티 메소드
        /// <summary>
        /// Makes string safe for xml parsing, replacing control chars with '?'
        /// </summary>
        /// <param name="encodedString">string to make safe</param>
        /// <returns>xml safe string</returns>
        private static string CharacterSafeString(string encodedString)
        {
            /*The default code page for the system will be used.
            Since all code pages use the same lower 128 bytes, this should be sufficient
            for finding uprintable control characters that make the xslt processor error.
            We use characters encoded by the default code page to avoid mistaking bytes as
            individual characters on non-latin code pages.*/
            char[] encodedChars = System.Text.Encoding.Default.GetChars(System.Text.Encoding.Default.GetBytes(encodedString));

            System.Collections.ArrayList pos = new System.Collections.ArrayList();
            for (int x = 0; x < encodedChars.Length; x++)
            {
                char currentChar = encodedChars[x];
                //unprintable characters are below 0x20 in Unicode tables
                //some control characters are acceptable. (carriage return 0x0D, line feed 0x0A, horizontal tab 0x09)
                if (currentChar < 32 && (currentChar != 9 && currentChar != 10 && currentChar != 13))
                {
                    //save the array index for later replacement.
                    pos.Add(x);
                }
            }
            foreach (int index in pos)
            {
                encodedChars[index] = '?';//replace unprintable control characters with ?(3F)
            }
            return System.Text.Encoding.Default.GetString(System.Text.Encoding.Default.GetBytes(encodedChars));
        }

        private string EncodeCData(string text)
        {
            return CharacterSafeString(text).Replace("]]>", "]]&gt;");
        }
        #endregion

        public void Visit(TestSuiteResult suiteResult)
        {
            m_strSuiteName = suiteResult.Name;

            foreach (TestResult result in suiteResult.Results)
            {
                result.Accept(this);
            }
        }

        public void Visit(TestCaseResult caseResult)
        {
            if (caseResult.Executed == false) return;
            
            m_xmlWriter.WriteStartElement("test");

            m_xmlWriter.WriteAttributeString("suite", m_strSuiteName);
            string[] strArrName = caseResult.Name.Split('.');
            m_xmlWriter.WriteAttributeString("name", strArrName[strArrName.Length-1]);
            m_xmlWriter.WriteAttributeString("time", caseResult.Time.ToString("#####0.000", NumberFormatInfo.InvariantInfo));

            if (caseResult.IsFailure)
            {
                m_xmlWriter.WriteStartElement("failure");

                string[] strArrStack = caseResult.StackTrace.Split('\\');
                string strComma = ", ";
                string strNextLine = "\n";

                string strMessage = caseResult.Message.Trim(strComma.ToCharArray());
                strMessage = strMessage.Trim(strNextLine.ToCharArray());
                m_xmlWriter.WriteAttributeString("message", strArrStack[strArrStack.Length - 1] + " : " + strMessage);

                m_xmlWriter.WriteEndElement();
            }

            m_xmlWriter.WriteEndElement();
        }

        public void Write()
        {
            m_xmlWriter.WriteEndElement();
            m_xmlWriter.WriteEndDocument();
            m_xmlWriter.Flush();

            if (m_memoryStream != null && m_writer != null)
            {
                m_memoryStream.Position = 0;
                using (StreamReader rdr = new StreamReader(m_memoryStream))
                {
                    m_writer.Write(rdr.ReadToEnd());
                }
            }

            m_xmlWriter.Close();
        }
    }
}
