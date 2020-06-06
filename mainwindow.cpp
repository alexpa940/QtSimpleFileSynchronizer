#include "mainwindow.h"
#include "ui_mainwindow.h"

bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
    QDir sourceDir(fromDir);
    QDir targetDir(toDir);
    if(!targetDir.exists()){    /* if directory don't exists, build it */
        if(!targetDir.mkdir(targetDir.absolutePath()))
            return false;
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList){
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir()){    /* if it is directory, copy recursively*/
            if(!copyDirectoryFiles(fileInfo.filePath(),
                targetDir.filePath(fileInfo.fileName()),
                coverFileIfExist))
                return false;
        }
        else{            /* if coverFileIfExist == true, remove old file first */
            if(coverFileIfExist && targetDir.exists(fileInfo.fileName())){
                targetDir.remove(fileInfo.fileName());
            }

            // files copy
            if(!QFile::copy(fileInfo.filePath(),
                targetDir.filePath(fileInfo.fileName()))){
                    return false;
            }
        }
    }
    return true;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->label->setText(QDir::current().absolutePath());
    ui->label_2->setText(QDir::current().absolutePath());
    dir1 = dir2 =  QDir::current().absolutePath();
    ReloadList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ReloadList()
{
    if( dir1.isEmpty())
        return;
    if( dir2.isEmpty())
        return;
    dirst1 = QDir(dir1).entryList(QDir::Dirs);
    dirst2 = QDir(dir2).entryList(QDir::Dirs);

    dirst1.removeOne(".");
    dirst1.removeOne("..");

    dirst2.removeOne(".");
    dirst2.removeOne("..");

    for (int i =0;i<dirst1.size();i++)
        dirst1[i] = "/" + dirst1[i];

    for (int i =0;i<dirst2.size();i++)
        dirst2[i] = "/" + dirst2[i];


    dirst2 += QDir(dir2).entryList(QDir::Files);
    dirst1 += QDir(dir1).entryList(QDir::Files);


    Model1 = new QStringListModel(this);
    Model1->setStringList(dirst1);
    ui->listView_2->setModel(Model1);

    Model2 = new QStringListModel(this);
    Model2->setStringList(dirst2);
    ui->listView->setModel(Model2);
}

void MainWindow::on_pushButton_clicked()
{
    dir1 = QFileDialog::getExistingDirectory(0, "Выбор папки", "");
    ui->label->setText(dir1);
    ReloadList();
}

void MainWindow::on_pushButton_2_clicked()
{
    dir2 = QFileDialog::getExistingDirectory(0, "Выбор папки", "");
    ui->label_2->setText(dir2);
    ReloadList();
}

void MainWindow::on_pushButton_4_clicked()
{

 ReloadList();

}

void MainWindow::on_pushButton_3_clicked()
{
    foreach(QString word1, dirst1)
    {
        if(!dirst2.contains(word1, Qt::CaseInsensitive))
            {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, QString::fromUtf8("Сообщение"),
                              QString::fromUtf8("Скопировать из ") + dir1  +
                         QString::fromUtf8(" объект ") +word1 + QString::fromUtf8(" ?"),
                              QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (reply == QMessageBox::Yes)
        {
            if(word1[0]=='/')
            {
            word1.remove(0,1);
            copyDirectoryFiles(dir1+"/"+word1,dir2+"/"+word1,true);
            continue;
            }

            QFile::copy(dir1+"/"+word1,dir2+"/"+word1);
            }
        else if (reply == QMessageBox::No)
            continue;
        else
            continue;
        }

    }

    foreach(QString word2, dirst2)
    {


        if(!dirst1.contains(word2, Qt::CaseInsensitive))
            {

            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, QString::fromUtf8("Сообщение"),
                                  QString::fromUtf8("Скопировать из ") + dir2  +
                             QString::fromUtf8(" объект ") +word2 + QString::fromUtf8(" ?"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            if (reply == QMessageBox::Yes)
            {
            if(word2[0]=='/')
            {
            word2.remove(0,1);
            copyDirectoryFiles(dir2+"/"+word2,dir1+"/"+word2,true);
            continue;
            }

            QFile::copy(dir2+"/"+word2,dir1+"/"+word2);
            }
            else if (reply == QMessageBox::No)
                continue;
            else
                continue;

    }
    }

    ReloadList();
}

void MainWindow::on_listView_2_doubleClicked(const QModelIndex &index)
{
        if( dir1.isEmpty())
            return;
        QString word1 = index.data().toString().trimmed();
        if(word1[0]=='/')
        {
        word1.remove(0,1);
        }
        QFileInfo fi(dir1+"/"+word1);
        QString name = fi.fileName();

        QDateTime birthdate = fi.created();
        QString birthdatestr = birthdate.toString("yyyy-MM-dd hh:ss:mm");

        QString lastMod = fi.lastModified().toString("yyyy-MM-dd hh:ss:mm");

        QString isdir = QVariant(fi.isDir()).toString();

        QString ishide = QVariant(fi.isHidden()).toString();

        QString readonly = QVariant(!fi.isWritable()).toString();


        QMessageBox::information(this, "Информация о выбранном элементе ", "Имя объекта: " + name + "\nДата создания: " + birthdatestr
                         + "\nПоследнее изменение: " + lastMod + "\nЯвляется ли папкой: " + isdir +"\nСкрыт: " + ishide + "\nТолько для чтения: " + readonly);
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    if( dir2.isEmpty())
        return;
    QString word1 = index.data().toString().trimmed();
    if(word1[0]=='/')
    {
    word1.remove(0,1);
    }
    QFileInfo fi(dir2+"/"+word1);
    QString name = fi.fileName();

    QDateTime birthdate = fi.created();
    QString birthdatestr = birthdate.toString("yyyy-MM-dd hh:ss:mm");

    QString lastMod = fi.lastModified().toString("yyyy-MM-dd hh:ss:mm");

    QString isdir = QVariant(fi.isDir()).toString();

    QString ishide = QVariant(fi.isHidden()).toString();

    QString readonly = QVariant(!fi.isWritable()).toString();

    QMessageBox::information(this, "Информация о выбранном элементе ", "Имя объекта: " + name + "\nДата создания: " + birthdatestr
                     + "\nПоследнее изменение: " + lastMod + "\nЯвляется ли папкой: " + isdir +"\nСкрыт: " + ishide + "\nТолько для чтения: " + readonly);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Delete)
    {
        if(ui->listView_2->hasFocus())
        {
            //qDebug() << ui->listView_2->currentIndex().data().toString();
            QString word1 = ui->listView_2->currentIndex().data().toString();
            if(word1[0]=='/')
            {
                word1.remove(0,1);
                QDir dir(dir1+ "/"+word1);
                dir.removeRecursively();
            }
            else
            {
                QFile(dir1+ "/"+word1).remove();
            }

        }

        if(ui->listView->hasFocus())
        {
            //qDebug() << ui->listView->currentIndex().data().toString();
            QString word1 = ui->listView->currentIndex().data().toString();
            if(word1[0]=='/')
            {
                word1.remove(0,1);
                QDir dir(dir2 + "/"+ word1);
                dir.removeRecursively();
            }
            else
            {
                QFile(dir2 + "/"+ word1).remove();
            }
        }
        ReloadList();
    }
}

