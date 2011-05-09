#include "dialogselecttextcodec.h"
#include "ui_dialogselecttextcodec.h"

#include <QTextCodec>
#include <QList>
#include <QByteArray>

DialogSelectTextCodec::DialogSelectTextCodec(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectTextCodec)
	//m_szCurrent()
{
    ui->setupUi(this);
	
	connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(codecChanged(QString)));
	
	QTextCodec *pLocale = QTextCodec::codecForLocale();
	QString szLocaleName(pLocale->name());
	
	QStringList lstCodecs;
	QList<QByteArray> lstBCodec = QTextCodec::availableCodecs();
	
	auto it = lstBCodec.begin();
	auto itEnd = lstBCodec.end();
	while (it != itEnd)
	{
		lstCodecs << QString((*it));
		++it;
	}
	lstCodecs.sort();
	
	int iLocale = lstCodecs.indexOf(szLocaleName);
	
	ui->comboBox->addItems(lstCodecs);
	ui->comboBox->setCurrentIndex(iLocale);
}

DialogSelectTextCodec::~DialogSelectTextCodec()
{
    delete ui;
}

/*
void DialogSelectTextCodec::comboIndexChanged(int iNewIndex)
{
	if (iNewIndex == -1)
	{
		// invalidated
		return;
	}

	QString szCodec = ui->comboBox->itemText(iNewIndex);
	if (m_szCurrent != szCodec)
	{
		emit codecChanged(szCodec);
		m_szCurrent = szCodec;
	}
	
}
*/



