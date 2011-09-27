#ifndef DIALOGSELECTTEXTCODEC_H
#define DIALOGSELECTTEXTCODEC_H

#include <QDialog>

namespace Ui {
    class DialogSelectTextCodec;
}

class DialogSelectTextCodec : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectTextCodec(QWidget *parent = 0);
    ~DialogSelectTextCodec();

private:
    Ui::DialogSelectTextCodec *ui;
	//QString m_szCurrent;
	
signals:
	void codecChanged(QString);
	
protected slots:
	//void comboIndexChanged(int iNewIndex);
	//void comboIndexChanged(QString szNewText);
};

#endif // DIALOGSELECTTEXTCODEC_H
