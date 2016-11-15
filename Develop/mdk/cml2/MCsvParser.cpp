#include "stdafx.h"
#include "MCsvParser.h"


MCsvParser::MCsvParser()
{
	m_pBuffer = NULL;
	m_pOffset = NULL;
	m_nLine = 0;
}

MCsvParser::~MCsvParser()
{
	Clear();
}


char* MCsvParser::ReadFile(const char* fname)
{
	FILE *fp;
	char * ptr;
	int len;

	fp = fopen(fname, "rb");
	if (fp == NULL) return NULL;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	ptr = new char [len+1];
	fread(ptr, 1, len, fp);
	ptr[len] = '\0';
	fclose(fp);

	return ptr;
}

bool MCsvParser::Load(const char* fname)
{
//	파일 전체를 읽는다
//
	m_pBuffer = ReadFile(fname);

//	라인수를 센 후 각 오프셋을 구한다
//
	m_nLine = CountLine(m_pBuffer, NULL);
	m_pOffset = new int[m_nLine];
	CountLine(m_pBuffer, m_pOffset);

	return true;
}

bool MCsvParser::LoadFromStream(const char* szStream)
{
	int len = strlen(szStream) + 1;
	m_pBuffer = new char[len];
	strcpy_s(m_pBuffer, len, szStream);

	m_nLine = CountLine(m_pBuffer, NULL);
	m_pOffset = new int[m_nLine];
	CountLine(m_pBuffer, m_pOffset);

	return true;
}

int MCsvParser::CountLine(const char* buffer, int* offset)
{
	const char* p = buffer;
	int line;

	for(line=0; p; line++) 
	{
		if (line > 0) p++;

		if (offset)
			offset[line] = (int)(p - buffer);

		p = strchr(p, '\n');
	}

	return line;
}

int MCsvParser::PassToken(const char* str)
{
	int i = 0;

	if (str[0] == '\"') 
	{
		for(i=1; ;) 
		{
			if (str[i] == '\"') 
			{
				if (str[i+1] == '\"') i += 2;
				else break;
			}	
			else
			{
				i += 1;
			}
		}
		return i+1;
	}	
	else 
	{
		//for(i=0; !strchr(",\n", str[i]); i++) ;

		for(i=0; ; i++)
		{
			if (strchr(",\n", str[i]))
			{
				if (str[i-1] == '\"' && str[i+1] == '\"')
				{
					continue;
				}

				break;
			}
		}

		return i;
	}
}

int MCsvParser::GetData(int col, int row, char* outptr, int outlen)
{
	int i, off;

	if (row >= m_nLine)
		return 0;

	for(i=0, off=m_pOffset[row]; i<col; off++, i++) 
	{
		off += PassToken(m_pBuffer + off);
		if (strchr("\n", m_pBuffer[off])) 
		{
			outptr[0] = '\0';
			return 0;
		}
	}

	if (m_pBuffer[off] == '\"') 
	{
		for(i=0, off+=1; ; i++) 
		{
			if (m_pBuffer[off] == '\"' && m_pBuffer[off+1] == '\"') 
			{
				if (i < outlen-1)
					outptr[i] = m_pBuffer[off];
				off += 2;
			}
			else
			{
				if (m_pBuffer[off] != '\"') 
				{
					outptr[i] = m_pBuffer[off];
					off += 1;
                }	
				else
				{
					break;
				}
			}
		}
	}	
	else 
	{
		//for(i=0; !strchr(",\n", m_pBuffer[off]); i++, off++) 
		//{
		//	if (i < outlen-1)
		//		outptr[i] = m_pBuffer[off];
		//}

		//""로 쌓인 , 분리하기.
		for (i=0; ;i++, off++)
		{
			if (strchr(",\n", m_pBuffer[off]))
			{
				if (m_pBuffer[off-1] == '\"' && m_pBuffer[off+1] == '\"')
				{
					if (i < outlen-1)
						outptr[i] = m_pBuffer[off];
					continue;
				}

				break;
			}

			if (m_pBuffer[off] == '\"')
			{
				i--;
				continue;
			}

			if (i < outlen-1)
				outptr[i] = m_pBuffer[off];
		}

	}

	if (i >= outlen-1) outptr[outlen-1] = '\0';
	else outptr[i] = '\0';


	return i;
}


int	MCsvParser::GetColCount(int row)
{
	int count = 0;
	int i, off;

	if (row >= m_nLine) return 0;
	if (m_pBuffer[m_pOffset[row]] == '\0') return 0;

	for(i=0, off=m_pOffset[row]; i < 1000; off++, i++) 
	{
		off += PassToken(m_pBuffer + off);
		count++;
		if (strchr("\n", m_pBuffer[off])) 
		{
			return count;
		}

	}

	return count;
}

void MCsvParser::Clear()
{
	if (m_pBuffer)
		delete [] m_pBuffer;
	if (m_pOffset)
		delete [] m_pOffset;

	m_pBuffer = NULL;
	m_pOffset = NULL;
	m_nLine = 0;
}

// 스트링에 ,가 있으면 ""로 싸기
void MCsvParser::TransComma( char* outptr )
{
	int i, off = 0;

	for (i=0; ;i++, off++)
	{
		if (m_pBuffer[off] == ',')
		{
			outptr[i] = '\"';			i++;
			outptr[i] = m_pBuffer[off]; i++;
			outptr[i] = '\"';
			continue;
		}

		if (strchr("\n", m_pBuffer[off]))
		{
			break;
		}

		outptr[i] = m_pBuffer[off];
	}
}