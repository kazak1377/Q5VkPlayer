#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vkauth.h"
#include "qdebug.h"
#include "musiccontrol.h"
#include <QSettings>
#include <QSystemTrayIcon>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    music = new musicControl;
    netCore = new netWorker;
    ui->lineEdit->setPlaceholderText("Search here");
    ui->seekSlider->setRange(0,0);
    settings = new QSettings(this);
 

    ///tray icon setup
    QAction *next = new QAction(tr("Next"),this);
    QAction *restore = new QAction(tr("Show window"),this);
    QAction *prev = new QAction(tr("Previous"),this);
    QAction *playPause = new QAction(tr("Play/Pause"),this);
    QAction *close = new QAction(tr("Exit"),this);
    connect(next,SIGNAL(triggered()),music,SLOT(playNextSong()));
    connect(prev,SIGNAL(triggered()),music,SLOT(playPrevSong()));
    connect(playPause,SIGNAL(triggered()),music,SLOT(changeState()));
    connect(close,SIGNAL(triggered()),qApp,SLOT(quit()));
    connect(restore,SIGNAL(triggered()),this,SLOT(show()));
    QMenu *trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(restore);
    trayIconMenu->addAction(next);
    trayIconMenu->addAction(prev);
    trayIconMenu->addAction(playPause);
    trayIconMenu->addAction(close);
    trayIcon = new QSystemTrayIcon(QIcon(":/icons/qvk.png"));
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,
            SLOT(trayHandler(QSystemTrayIcon::ActivationReason)));
    ///table setting
    QStringList header;
    ui->musicWidget->setColumnCount(4);
    header <<"Artist"<<"Title"<<"Duration"<<"link";   //in case of unsuccesseful update....
    ui->musicWidget->hideColumn(3);
    ui->musicWidget->setHorizontalHeaderLabels(header);
    ui->musicWidget->verticalHeader()->setVisible(false);
    ui->musicWidget->setShowGrid(false);
    ui->musicWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->musicWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->musicWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->musicWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ////////////////////////////////////////////////Creating actions
    QAction *login = new QAction(tr("Login"),this);
    QAction *About = new QAction(tr("About"),this);
    QAction *refrsh = new QAction(tr("Refresh"),this);
    connect(refrsh,SIGNAL(triggered()), netCore, SLOT(getAudioList()));
    connect(login, SIGNAL(triggered()), netCore, SLOT(loginSlot()));
    connect(About, SIGNAL(triggered()), this, SLOT(about()));
    ////////////////////////////////////////////////Creating menu
    QMenu *gearButtonMenu = new QMenu("Options", ui->toolButton);
    gearButtonMenu->addAction(refrsh);
    gearButtonMenu->addAction(login);
    gearButtonMenu->addSeparator();
    gearButtonMenu->addAction(About);
    gearButtonMenu->addAction(close);
    ///////////////////////////////////////////////making menu button
    ui->toolButton->setMenu(gearButtonMenu);

    ///connection area
    connect(this,SIGNAL(setPlayingOrder(QList<QUrl>)),music,SLOT(setPlayList(QList<QUrl>)));
    connect(ui->volumeSlider,SIGNAL(valueChanged(int)),music,SLOT(volumeSliderSlot(int)));
    connect(ui->musicWidget,SIGNAL(cellDoubleClicked(int,int)),music,SLOT(playThatSong(int,int)));
    connect(ui->shuffButton,SIGNAL(toggled(bool)),music,SLOT(shuffleMode(bool)));
    connect(ui->nextButton,SIGNAL(clicked()),music,SLOT(playNextSong()));
    connect(music,SIGNAL(setIndexToUi(int,int)),this,SLOT(setSongUi(int,int)));
    connect(ui->prevButton,SIGNAL(clicked()),music,SLOT(playPrevSong()));
    connect(ui->tooglePlayingButton,SIGNAL(clicked()),music,SLOT(changeState()));
    connect(music,SIGNAL(setPlayingUi()),this,SLOT(setPlayingUi()));
    connect(music,SIGNAL(setPausedUi()),this,SLOT(setPausedUi()));
    connect(ui->seekSlider,SIGNAL(sliderMoved(int)),music,SLOT(setPosition(int)));
    connect(music,SIGNAL(newPosition(qint64)),this,SLOT(positionChanged(qint64)));
    connect(music,SIGNAL(newRange(qint64)),this,SLOT(durationChanged(qint64)));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(currentSearch(QString)));
    connect(this,SIGNAL(loadToken(QString,QString)),netCore,SLOT(setToken(QString,QString)));
    connect(netCore,SIGNAL(audioListGet(QList<QUrl>)),music,SLOT(setPlayList(QList<QUrl>)));
    connect(ui->musicWidget,SIGNAL(cellClicked(int,int)),music,SLOT(setSelectedSong(int,int)));
    connect(netCore,SIGNAL(dataGot()),this,SLOT(setMusicTable()));
    ///connection area

    ///CONFIG LOADING==================================
    this->loadSettings();
}

void MainWindow::loadSettings()
{
    restoreGeometry(settings->value("geometry",saveGeometry()).toByteArray());
    ui->volumeSlider->setValue(settings->value("volume",50).toInt());
    QString localToken = settings->value("token","none").toString();
    QString localUid = settings->value("user_id","none").toString();
    emit loadToken(localToken,localUid);
    netCore->getAudioList();
}

void MainWindow::saveSettings()
{
    settings->setValue("token",netCore->getToken());
    settings->setValue("user_id",netCore->getUid());
    settings->setValue("volume",ui->volumeSlider->value());
    settings->setValue("geometry",saveGeometry());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible())
    {
        this->hide();
        event->ignore();
    }
}

void MainWindow::setPlayingUi()
{
    ui->tooglePlayingButton->setIcon(QIcon(QPixmap(":/icons/dark/gtk-media-pause.png")));
}

void MainWindow::setPausedUi()
{
    ui->tooglePlayingButton->setIcon(QIcon(QPixmap(":/icons/dark/gtk-media-play-ltr.png")));
}

void MainWindow::positionChanged(qint64 position)
{
    ui->seekSlider->setValue(position);
}

void MainWindow::durationChanged(qint64 duration)
{
    ui->seekSlider->setRange(0,duration);
}

void MainWindow::offlineDebugFunction()
{
    QStringList debugTableLine;
    debugTableLine<<"Amy Macdonald"<<"Barrowland Ballroom"<<"03:58"<<"1.mp3";
    this->setTableLine(debugTableLine);
    debugTableLine.clear();
    debugTableLine<<"Amy Macdonald"<<"This is the life"<<"03:06"<<"2.mp3";
    this->setTableLine(debugTableLine);
    debugTableLine.clear();
    debugTableLine<<"Год змеи"<<"Секс и рок-н-ролл"<<"03:04"<<"3.mp3";
    this->setTableLine(debugTableLine);
    debugTableLine.clear();
    debugTableLine<<"One Republic"<<"All The Right Moves"<<"03:58"<<"4.mp3";
    this->setTableLine(debugTableLine);
    debugTableLine.clear();
    debugTableLine<<"One Republic"<<"Everybody Loves Me"<<"3:40"<<"5.mp3";
    this->setTableLine(debugTableLine);
    debugTableLine.clear();
    QList<QUrl> plst;
    plst.append(QUrl("1.mp3"));
    plst.append(QUrl("2.mp3"));
    plst.append(QUrl("3.mp3"));
    plst.append(QUrl("4.mp3"));
    plst.append(QUrl("5.mp3"));
    qDebug()<<plst;
    emit setPlayingOrder(plst);
}

void MainWindow::setSongUi(int current,int /*prev*/)
{
    QFont boldFont;
    boldFont.setBold(true);
    QFont regularFont;
    regularFont.setBold(false);
    const int rowCount = ui->musicWidget->rowCount();
    const int columnCount = ui->musicWidget->columnCount();
    for(int i = 0; i < rowCount; ++i) {
        for(int j = 0; j < columnCount; ++j) {
            ui->musicWidget->item(i, j)->setFont(regularFont);
        }
    }
    ui->musicWidget->scrollToItem(ui->musicWidget->item(current,0));
    this->setWindowTitle(ui->musicWidget->item(current,0)->text()+"  -  "+
                         ui->musicWidget->item(current,1)->text());
    ui->musicWidget->setStyleSheet("QTableWidget{font-weight: normal;}");
    ui->musicWidget->clearSelection();
    ui->musicWidget->item(current,0)->setSelected(true);
    ui->musicWidget->item(current,1)->setSelected(true);
    ui->musicWidget->item(current,2)->setSelected(true);
    ui->musicWidget->item(current,3)->setSelected(true);
    ui->musicWidget->item(current,0)->setFont(boldFont);
    ui->musicWidget->item(current,1)->setFont(boldFont);
    ui->musicWidget->item(current,2)->setFont(boldFont);
    ui->musicWidget->item(current,3)->setFont(boldFont);
}


void MainWindow::about()
{
    QMessageBox::information(this, tr("About QVkPlayer"),
                             tr("It is a Player for playing music from Your Vkontakte playlist.\n"
                                "More features will be avalible later.\n"
                                "\tfirst released version  'alpha 0.4'\n"
                                " credits:\n"
                                "\tMe: kazak1377(Maxim Kozachenko)\n"
                                "Thanks to:\n"
                                "\tQt team\n"
                                "\tmembers of c_plus_plus jabber.ru conference"));
}


void MainWindow::trayHandler(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick)
    {
        setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        show();
        activateWindow();
    }
}

void MainWindow::currentSearch(QString text)
{
    qDebug()<<"USER IS SEARCHING======================================";
    ui->lineEdit->setStyleSheet("QLineEdit{background: #FFFFFF;}");    //white for search line
    qDebug()<<text;
    QList<QTableWidgetItem *> foundList;
    foundList = ui->musicWidget->findItems(text,Qt::MatchContains);
    if(!foundList.isEmpty())
    {
        qDebug()<<"Found at row: " ;
        qDebug()<<foundList[0]->row()+1;
        ui->musicWidget->selectRow(foundList[0]->row());
    }
    else
    {
        ui->lineEdit->setStyleSheet("QLineEdit{background: #FF6666;}");   //error for the searchline
    }
}

QString MainWindow :: durationToHuman(int d)
{
    int minutes = d/60;
    int seconds = d%60;
    QString out;
    if(seconds<10)
    {
        out = QString::number(minutes)+":0"+QString::number(seconds);
    }
    else
        out = QString::number(minutes)+":"+QString::number(seconds);
    return out;
}

void MainWindow::setTableLine(QStringList line)
{
    int lastRow = ui->musicWidget->rowCount();
    ui->musicWidget->insertRow(lastRow);
    for(int i=0;i<=3;i++)
    {
        QTableWidgetItem *item;
        if(i==2)
            item=new QTableWidgetItem (durationToHuman(line[i].toInt()));
        else
            item=new QTableWidgetItem (line[i]);
        ui->musicWidget->setItem(lastRow,i,item);
    }
}


void MainWindow::setMusicTable()
{
    setPausedUi();
    ui->musicWidget->setRowCount(0);
    ui->musicWidget->clear();
    ui->musicWidget->cellClicked(0,0);
    music->clearHistory();
    QStringList header;
    ui->musicWidget->setColumnCount(4);
    header <<"Artist"<<"Title"<<"Duration"<<"link";
    ui->musicWidget->hideColumn(3);
    ui->musicWidget->setHorizontalHeaderLabels(header);
    ui->musicWidget->verticalHeader()->setVisible(false);
    QList<QStringList> songsTable = netCore->getSongsTable();
    QStringList tableLine;
    foreach (tableLine, songsTable)
    {
        this->setTableLine(tableLine);
    }
}

MainWindow::~MainWindow()
{
    trayIcon->hide();
    saveSettings();
    delete ui;
    delete trayIcon;
    delete music;
}
