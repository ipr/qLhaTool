#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QTreeWidgetItem>

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

	// key: path (without filename)
	// value: top-level item
	//
	QMap<QString, QTreeWidgetItem*> m_PathToItem;
	
	//QString GetPath(const QString &szName);
	bool SplitPathFileName(const QString &szName, QString &szPath, QString &szFile);
	
	void ClearAll();
	
private slots:
    void on_actionExtractAll_triggered();
    void on_actionAbout_triggered();
    void on_actionFile_triggered();
	
	void onFileSelected(QString szArchiveFile);
	//void onFileLocated();
	
	void onMessage(QString szData);
	void onWarning(QString szData);
};

#endif // MAINWINDOW_H
