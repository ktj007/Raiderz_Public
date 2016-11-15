using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Mail;
using System.IO;
using System.Collections;

namespace DumpAnalyzer
{
    class MailSender
    {
        private ArrayList m_EmailIDArrayList;
        private DumpFileNameList m_dumpFileNameList;

        public MailSender(ArrayList emailArrayList, DumpFileNameList dumpFileNameList)
        {
            m_EmailIDArrayList = emailArrayList;
            m_dumpFileNameList = dumpFileNameList;
        }
        public void Send()
        {
            DateTime today = DateTime.Now;

            string text = "최근 ";
            text += XConfig.m_nCheckRecentHour;
            text += "시간 내에 추가된 ";
            text += XConfig.m_strProgName;
            text += " 크래쉬 덤프 파일 목록입니다.\n";
            text += "\n";
            text += "덤프 파일 확인은 ";
            text += XConfig.m_strPath;
            text += " 에서 확인할 수 있습니다.\n";
            text += "------------------------------------------------------------------\n";
            text += m_dumpFileNameList.GetText();
            text += "------------------------------------------------------------------\n";
            string subject = "최신 " + XConfig.m_strProgName + " 크래쉬 덤프 (" + today.ToShortDateString() + ")";

            SendAll(subject, text);
        }

        private void SendAll(string subject, string text)
        {
            for (int i = 0; i < m_EmailIDArrayList.Count; i++)
            {
                string id = (string)(m_EmailIDArrayList[i]);
                SendMail(id, subject, text);
            }
        }
        private void SendMail(string to, string subject, string text)
        {
            try
            {
                MailAddress fromMailAddress = new MailAddress(XConfig.m_strSendEmailAddress, "크래쉬 덤프");
                MailAddress toMailAddress = new MailAddress(to, to);
                MailMessage message = new MailMessage(fromMailAddress, toMailAddress);
                message.Subject = subject;
                message.Body = text;
                message.BodyEncoding = Encoding.UTF8;
                SmtpClient client = new SmtpClient(XConfig.m_strSMTPServer);
                client.UseDefaultCredentials = false;
                client.Port = 25;

                client.Send(message);
            }
            catch (SmtpException ex)
            {
                Console.WriteLine("SendEMail: " + ex.ToString());
            }
        }
    }


}