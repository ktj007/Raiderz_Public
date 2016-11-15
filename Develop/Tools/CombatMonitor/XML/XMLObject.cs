/*
 * SoulHunt.cs
 * SoulHunt와 관련된 각종 자료형들을 구현
 */

using System;
using System.Windows.Forms;
using System.Configuration;
using System.Collections.Generic;
using System.IO;
using System.ComponentModel;

namespace CombatMonitor.XML
{
    /// <summary>
    /// ISHCompilable : 컴파일해서 메시지를 생성할 것들을 정의한 인터페이스
    /// </summary>
    public abstract class XMLObject
    {
        protected String _strMessage;
        protected bool _bPassed;

        [Browsable(false)]
        public string CompiledMessage { get { return _strMessage; } }
        [Browsable(false)]
        public bool Passed { get { return _bPassed; } }

        protected bool Error(String message) { if (message != null) { _strMessage = message; _bPassed = false; } return _bPassed; }

        public abstract bool Cooking();
    }
}