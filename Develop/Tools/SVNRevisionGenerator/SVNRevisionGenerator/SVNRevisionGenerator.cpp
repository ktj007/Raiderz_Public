#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time_adjustor.hpp"
#include "boost/date_time/local_time/local_time.hpp"
#include "boost/date_time/c_local_time_adjustor.hpp"
#include <sstream>
#include <fstream>
#include <iostream>

#define OUTPUT_VERSIONFILE	"SVNRevision.h"
#define UNKNOWN_STRING		"Unknown"

#define SVN_PATH1 ".svn/entries"
#define SVN_PATH2 "_svn/entries"

bool CheckOldHeader(std::string strStreamCode);
void WriteHeader( std::ostringstream &os );

int main(int argc, char **argv)
{
	std::string strAdditionalPath;
	if(argc >= 1 && argv[1])
	{
		strAdditionalPath = argv[1];
	}

	FILE* EntriesFile = fopen((strAdditionalPath+SVN_PATH1).c_str(), "r");
	if(!EntriesFile)
		EntriesFile = fopen((strAdditionalPath+SVN_PATH2).c_str(), "r");

	std::ostringstream os;
	int nRevision=-1;
	std::string strDate = UNKNOWN_STRING;

	if(EntriesFile)
	{
		char szBuff[200];
		char szYear[8];
		char szMonth[8];
		char szDay[8];
		char szHour[8];
		char szMinute[8];
		char szSecond[8];

		fgets(szBuff,200,EntriesFile);
		fgets(szBuff,200,EntriesFile);
		fgets(szBuff,200,EntriesFile);
		fscanf(EntriesFile,"%i",&nRevision);
		fgets(szBuff,200,EntriesFile);
		fgets(szBuff,200,EntriesFile);
		fgets(szBuff,200,EntriesFile);
		fgets(szBuff,200,EntriesFile);
		fgets(szBuff,200,EntriesFile);
		fscanf(EntriesFile,"%4s-%2s-%2sT%2s:%2s:%2s",szYear,szMonth,szDay,szHour,szMinute,szSecond);
		
		using namespace boost::gregorian;
		using namespace boost::posix_time;
		using namespace boost::local_time;
		using namespace std;

		typedef boost::date_time::c_local_adjustor<ptime> local_adj;
		ptime t10(date(atoi(szYear),atoi(szMonth),atoi(szDay)), time_duration(atoi(szHour),atoi(szMinute),atoi(szSecond))); 
		ptime t11 = local_adj::utc_to_local(t10);

		stringstream ss;
		time_facet* output_facet = new time_facet("%Y-%m-%d %p %I:%M:%S");
		ss.imbue(locale(locale("korean"), output_facet));
		ss << t11;

		strDate = ss.str();

		//delete output_facet;

		fclose(EntriesFile);
	}

	os << "#pragma once" << std::endl << std::endl;
	os << "#define SVN_REVISION \"" << nRevision << "\"" << std::endl;
	os << "#define SVN_DATE \"" << strDate << "\"" << std::endl;

	if (CheckOldHeader(os.str()))
		return -1;

	WriteHeader(os);

	return 0;
}



bool CheckOldHeader(std::string strStreamCode)
{
	std::string strCode;

	if(FILE* fileHeader = fopen(OUTPUT_VERSIONFILE,"rb"))
	{
		while(!feof(fileHeader))
		{
			char c = fgetc(fileHeader);
			if(c == -1) 
				break;
			strCode += c;
		}

		fclose(fileHeader);
	}

	return (strCode == strStreamCode);
}

void WriteHeader( std::ostringstream &os )
{
	if(FILE* OutputFile = fopen(OUTPUT_VERSIONFILE,"wb"))
	{
		fprintf(OutputFile,"%s",os.str().c_str());
		fclose(OutputFile);
	}
}