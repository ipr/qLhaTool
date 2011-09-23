#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextCodec>
#include <QDateTime>

#include "dialogselecttextcodec.h"

#include "qlhalib.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
	m_pLhaLib(nullptr),
	m_pCurrentCodec(nullptr),
	m_szCurrentCodec(),
	m_szBaseTitle(),
	m_szCurrentArchive(),
	m_PathToItem()
{
    ui->setupUi(this);
	m_szBaseTitle = windowTitle();
	connect(this, SIGNAL(FileSelection(QString)), this, SLOT(onFileSelected(QString)));
	
	m_pCurrentCodec = QTextCodec::codecForLocale();
	m_szCurrentCodec = QString(m_pCurrentCodec->name());
	
	m_pLhaLib = new QLhALib(this);
	connect(m_pLhaLib, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
	connect(m_pLhaLib, SIGNAL(warning(QString)), this, SLOT(onWarning(QString)));
	connect(m_pLhaLib, SIGNAL(fatal_error(QString)), this, SLOT(onFatalError(QString)));
	
	QStringList treeHeaders;
	treeHeaders << "Name" 
			<< "Unpacked size" 
			<< "Packed size" 
			<< "Time" 
			<< "Date" 
			<< "Pack Mode" 
			<< "Header level";
			//<< "Attributes"
			//<< "CRC (A)" 
			//<< "CRC (D)" 
			//<< "Comment";
	ui->treeWidget->setColumnCount(treeHeaders.size());
	ui->treeWidget->setHeaderLabels(treeHeaders);
	
	// if file given in command line
	QStringList vCmdLine = QApplication::arguments();
	if (vCmdLine.size() > 1)
	{
		emit FileSelection(vCmdLine[1]);
	}
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::SplitPathFileName(const QString &szName, QString &szPath, QString &szFile)
{
	QString szFilePath = szName;
	szFilePath.replace('\\', "/");
	int iPos = szFilePath.lastIndexOf('/');
	if (iPos == -1)
	{
		// no path
		return false;
	}

	szFile = szFilePath.right(szFilePath.length() - iPos -1);
	if (szFilePath.at(0) != '/')
	{
		szPath = "/";
	}
	szPath += szFilePath.left(iPos);
	
	// split done
	return true;
}

void MainWindow::ClearAll()
{
	setWindowTitle(m_szBaseTitle);
	
	m_szCurrentArchive.clear();
	m_PathToItem.clear();
	ui->treeWidget->clear();
}

void MainWindow::on_actionFile_triggered()
{
	QString szFile = QFileDialog::getOpenFileName(this, tr("Open file"));
	if (szFile != NULL)
	{
		emit FileSelection(szFile);
	}
}

void MainWindow::onFileSelected(QString szArchiveFile)
{
	// test
    //char *archive_delim = "\377\\"; /* `\' is for level 0 header and broken archive. */
	
	ClearAll(); // clear previous archive (if any)
	try
	{
		QLhALib::tArchiveEntryList lstArchiveInfo;

		// set text-conversion codec
		m_pLhaLib->SetConversionCodec(m_pCurrentCodec);
		
		// set given file as archive
		m_pLhaLib->SetArchive(szArchiveFile);
	
		// collect list of files
		if (m_pLhaLib->List(lstArchiveInfo) == true)
		{
			QString szMessage;
			szMessage.append(" Total files in archive: ").append(QString::number(m_pLhaLib->GetTotalFileCount()))
					.append(" Total unpacked size: ").append(QString::number(m_pLhaLib->GetTotalSizeUnpacked()))
					.append(" Archive file size: ").append(QString::number(m_pLhaLib->GetArchiveFileSize()));
			
			ui->statusBar->showMessage(szMessage);
		}
		
		setWindowTitle(m_szBaseTitle + " - " + szArchiveFile);
		m_szCurrentArchive = szArchiveFile;
		
		auto it = lstArchiveInfo.begin();
		auto itEnd = lstArchiveInfo.end();
		while (it != itEnd)
		{
			QLhALib::CArchiveEntry &Entry = (*it);
			
			// skip "merge" instances (if any)
			// (LZX only, not in LHa)
			if (Entry.m_szFileName.length() < 1)
			{
				++it;
				continue;
			}

			QString szPath;
			QString szFile;
			
			if (SplitPathFileName(Entry.m_szFileName, szPath, szFile) == false)
			{
				// no path
				szFile = Entry.m_szFileName;
				szPath = "/";
			}
			
			// get top-level item of path
			QTreeWidgetItem *pTopItem = m_PathToItem.value(szPath, nullptr);
			if (pTopItem == nullptr)
			{
				pTopItem = new QTreeWidgetItem((QTreeWidgetItem*)0);
				pTopItem->setText(0, szPath);
				ui->treeWidget->addTopLevelItem(pTopItem);
				m_PathToItem.insert(szPath, pTopItem);
			}

			// add sub-item of file
			QTreeWidgetItem *pSubItem = new QTreeWidgetItem(pTopItem);
			pSubItem->setText(0, szFile);
			pSubItem->setText(1, QString::number(Entry.m_ulUnpackedSize)); // always given
			if (Entry.m_bPackedSizeAvailable == true) // not merged
			{
				pSubItem->setText(2, QString::number(Entry.m_ulPackedSize)); // always given
			}
			else
			{
				// merged? (no packed-size available)
				// not supported by LHa...
				pSubItem->setText(2, "(Merged)");
			}
			
			QTime Time(Entry.m_Stamp.time());
			pSubItem->setText(3, Time.toString("hh:mm:ss"));
			
			QDate Date(Entry.m_Stamp.date());
			pSubItem->setText(4, Date.toString("dd-MM-yyyy"));
			
			// packing mode
			pSubItem->setText(5, Entry.m_szPackMode);
			
			// header level
			pSubItem->setText(6, QString::number(Entry.m_ucHeaderLevel));

			
			/*
			// LZX: file-attributes (Amiga-style: HSPA RWED)
			QString szAttribs;
			szAttribs.sprintf("%c%c%c%c%c%c%c%c", 
							  (Entry.m_Attributes.h) ? 'h' : '-',
							  (Entry.m_Attributes.s) ? 's' : '-',
							  (Entry.m_Attributes.p) ? 'p' : '-',
							  (Entry.m_Attributes.a) ? 'a' : '-',
							  (Entry.m_Attributes.r) ? 'r' : '-',
							  (Entry.m_Attributes.w) ? 'w' : '-',
							  (Entry.m_Attributes.e) ? 'e' : '-',
							  (Entry.m_Attributes.d) ? 'd' : '-');
			pSubItem->setText(7, szAttribs);
			*/
			
			/*
			QString szCrcA; // CRC of entry in archive
			szCrcA.sprintf("%x", Entry.m_uiCrc);
			pSubItem->setText(8, szCrcA);
			*/

			// file-related comment (if any stored)
			//pSubItem->setText(9, QString::fromStdString(Entry.m_szComment));
			
			pTopItem->addChild(pSubItem);
			
			++it;
		}
		
		ui->treeWidget->expandAll();
		ui->treeWidget->resizeColumnToContents(0);
		ui->treeWidget->sortByColumn(0);
	}
	catch (std::exception &exp)
	{
		QMessageBox::warning(this, "Error caught",
							 QString::fromLocal8Bit(exp.what()),
							 QMessageBox::Ok);
	}
}

void MainWindow::on_actionExtractAll_triggered()
{
	QString szCurrentFilePath = m_szCurrentArchive;
	if (szCurrentFilePath.length() > 0)
	{
		szCurrentFilePath.replace('\\', "/");
		int iPos = szCurrentFilePath.lastIndexOf('/');
		if (iPos != -1)
		{
			// open selection in path of current file?
			szCurrentFilePath = szCurrentFilePath.left(iPos);
		}
		else
		{
			szCurrentFilePath.clear();
		}
	}

    QString szDestPath = QFileDialog::getExistingDirectory(this, tr("Select path to extract to.."), szCurrentFilePath);
    if (szDestPath == NULL)
    {
		return;
    }

	try
	{
		if (m_pLhaLib->Extract(szDestPath) == true)
		{
			//QString szOldMessage = ui->statusBar->currentMessage();
			ui->statusBar->showMessage("Extract completed!", 10000);
		}
	}
	catch (std::exception &exp)
	{
		QMessageBox::warning(this, "Error caught",
							 QString::fromLocal8Bit(exp.what()),
							 QMessageBox::Ok);
	}
}

void MainWindow::on_actionTest_triggered()
{
	try
	{
		if (m_pLhaLib->Test() == true)
		{
			//QString szOldMessage = ui->statusBar->currentMessage();
			ui->statusBar->showMessage("Test completed successfully", 10000);
		}
	}
	catch (std::exception &exp)
	{
		QMessageBox::warning(this, "Error caught",
							 QString::fromLocal8Bit(exp.what()),
							 QMessageBox::Ok);
	}
}

void MainWindow::onMessage(QString szData)
{
	ui->statusBar->showMessage(QString("Message: ").append(szData));
}

void MainWindow::onWarning(QString szData)
{
	ui->statusBar->showMessage(QString("Warning: ").append(szData));
}

void MainWindow::onFatalError(QString szData)
{
	ui->statusBar->showMessage(QString("Fatal error: ").append(szData));
}

void MainWindow::on_actionAbout_triggered()
{
	QTextEdit *pTxt = new QTextEdit(this);
	pTxt->setWindowFlags(Qt::Window); //or Qt::Tool, Qt::Dialog if you like
	pTxt->setReadOnly(true);
	pTxt->append("LhaTool by Ilkka Prusi 2011");
	pTxt->append("");
	pTxt->append("This program is free to use and distribute. No warranties of any kind.");
	pTxt->append("Program uses Qt 4.7.2 under LGPL v. 2.1");
	pTxt->append("");
	pTxt->append("Keyboard shortcuts:");
	pTxt->append("");
	pTxt->append("F = open LHa-file");
	pTxt->append("Esc = close");
	pTxt->append("? = about (this dialog)");
	pTxt->append("");
	pTxt->show();
}

void MainWindow::on_actionTextCodec_triggered()
{
	DialogSelectTextCodec *pDlg = new DialogSelectTextCodec(this);
	connect(pDlg, SIGNAL(codecChanged(QString)), this, SLOT(onTextCodec(QString)));
	
	pDlg->show();
}

void MainWindow::onTextCodec(QString szCodec)
{
	if (m_szCurrentCodec == szCodec)
	{
		return;
	}
	
	if (m_pCurrentCodec != nullptr)
	{
		m_pCurrentCodec = nullptr;
	}
	
	m_szCurrentCodec = szCodec;
	
	if (m_szCurrentCodec.isEmpty() == true)
	{
		return;
	}

	m_pCurrentCodec = QTextCodec::codecForName(m_szCurrentCodec.toAscii());
	onFileSelected(m_szCurrentArchive);
}

