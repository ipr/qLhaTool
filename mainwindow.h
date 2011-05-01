#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class QLhALib;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
	void FileSelection(QString szFile);
	
private:
    Ui::MainWindow *ui;
	QLhALib *m_pLhaLib;
	
	QString m_szBaseTitle;
	QString m_szCurrentArchive;

	void ClearAll();
	
private slots:
    void on_actionAbout_triggered();
    void on_actionFile_triggered();
	
	void onFileSelected(QString szArchiveFile);
	//void onFileLocated();
};

#endif // MAINWINDOW_H
