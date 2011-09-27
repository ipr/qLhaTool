#include <QtCore/QCoreApplication>

#include "qlhalib.h"

QLhALib *g_pLhaLib;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	QStringList vCmdLine = QApplication::arguments();
	if (vCmdLine.size() == 0)
	{
		return -1;
	}
	
	g_pLhaLib = new QLhALib(this);
	g_pLhaLib->SetArchive(vCmdLine.back());
	
	// TODO: connect somewhere..
	// need to show what went wrong
	//connect(g_pLhaLib, SIGNAL(fatal_error(QString)), this, SLOT());
	
	if (vCmdLine.front().contains("-t"))
	{
		// test mode
		if (g_pLhaLib->Test() == false)
		{
			// failure
			return -1;
		}
		// success
	}
	else if (vCmdLine.front().contains("-e"))
	{
		// extract all
		//vCmdLine.at(1); // extract path
	}
	else if (vCmdLine.front().contains("-x"))
	{
		// extract (pattern)
		//vCmdLine.at(1); // should have pattern
		//vCmdLine.at(2); // extract path
	}
	else if (vCmdLine.front().contains("-l")
			//|| vCmdLine.front().contains("-v")
			//|| vCmdLine.front().contains("-lv")
			)
	{
		// list mode (short)
		// TODO: list mode (verbose)
		
		QLhALib::tArchiveEntryList lstArchiveInfo;
		if (g_pLhaLib->List(lstArchiveInfo) == false)
		{
			// failure listing -> exit
			return -1;
		}
		QString szMessage;
		szMessage.append(" Total files in archive: ").append(QString::number(g_pLhaLib->GetTotalFileCount()))
				.append(" Total unpacked size: ").append(QString::number(g_pLhaLib->GetTotalSizeUnpacked()))
				.append(" Archive file size: ").append(QString::number(g_pLhaLib->GetArchiveFileSize()));
				
		auto it = lstArchiveInfo.begin();
		auto itEnd = lstArchiveInfo.end();
		while (it != itEnd)
		{
			QLhALib::CArchiveEntry &Entry = (*it);


			//SplitPathFileName(Entry.m_szFileName, szPath, szFile);
			
			/*
			std::cout << Entry.m_szFileName;
			std::cout << QString::number(Entry.m_ulUnpackedSize);
			std::cout << QString::number(Entry.m_ulPackedSize);
			
			std::cout << std::endl;
			*/
			
			// if verbose-mode
			// -> list rest
			
			++it;
		}
	}
	else
	{
		// unknown command
		return -2;
	}

    return a.exec();
}
