#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vkauth.h"
#include "qdebug.h"
#include "musiccontrol.h"
#include <QSettings>
#include <QSystemTrayIcon>
#include <QtConcurrent/QtConcurrent>
#ifdef WIN32
#include <windows.h>
#include <qt_windows.h>
#endif
#ifdef Q_OS_LINUX
#include "dbus/dbusmethods.h"
#include "dbus/dbusadaptor.h"
#include "dbus/dbusadaptor1.h"
#include <QtDBus/QDBusConnection>
#endif


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug()<<QApplication::applicationVersion();
    music = new MusicControl;
    netCore = new NetWorker;
    settings = new QSettings(this);
    settingsWindow = new PrefWindow(this);
    settingsWindow->setWindowFlags(Qt::Dialog);
    ui->lineEdit->setPlaceholderText("Search here");
    ui->seekSlider->setRange(0,0);
    ui->repeatButton->hide();
    isUnity = false;

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
    trayIcon = new QSystemTrayIcon(QIcon(":/icons/qvk.svg"));
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,
            SLOT(trayHandler(QSystemTrayIcon::ActivationReason)));
    //trayIcon->setVisible(true);
    //trayIcon->show();


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
    QAction *showSettingsWindow = new QAction(tr("Settings"),this);
    connect(showSettingsWindow, SIGNAL(triggered()),settingsWindow,SLOT(show()));
    connect(refrsh,SIGNAL(triggered()), netCore, SLOT(getAudioList()));
    connect(login, SIGNAL(triggered()), netCore, SLOT(loginSlot()));
    connect(About, SIGNAL(triggered()), this, SLOT(about()));
    ////////////////////////////////////////////////Creating menu
    QMenu *gearButtonMenu = new QMenu("Options", ui->toolButton);
    gearButtonMenu->addAction(refrsh);
    gearButtonMenu->addAction(login);
    gearButtonMenu->addAction(showSettingsWindow);
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
    connect(ui->repeatButton,SIGNAL(clicked(bool)),music,SLOT(repeatMode(bool)));
    connect(netCore,SIGNAL(dataGot()),this,SLOT(setMusicTable()));
    connect(this,SIGNAL(setPrefWindowsHotkeysUi(bool,bool)),settingsWindow,SLOT(setUseHotkeysUi(bool,bool)));
    connect(this,SIGNAL(setMinToTray(bool)),settingsWindow,SLOT(setUseMinTray(bool)));
    connect(settingsWindow,SIGNAL(setNewSettings(bool,bool,bool,QString,bool)),
            this,SLOT(setNewSettings(bool,bool,bool,QString,bool)));
    ///connection area



    ///DBUS setting
#ifdef Q_OS_LINUX
    DBusMethods* demo = new DBusMethods();
    new DBusAdaptor(demo);
    new DBusAdaptor1(demo);

    QDBusConnection connection = QDBusConnection::sessionBus();
    bool ret = connection.registerService("org.mpris.MediaPlayer2.qvkplayer.mainwindow");
    ret = connection.registerObject("/org/mpris/MediaPlayer2", demo);
    connect(demo,SIGNAL(dbusNext()),ui->nextButton,SIGNAL(clicked()));
    connect(demo,SIGNAL(dbusPlayPause()),ui->tooglePlayingButton,SIGNAL(clicked()));
    connect(demo,SIGNAL(dbusPrev()),ui->prevButton,SIGNAL(clicked()));
    connect(demo,SIGNAL(dbusQuit()),qApp,SLOT(quit()));
    connect(demo,SIGNAL(dbusRaise()),this,SLOT(show()));
#endif

    ///DBUS setting



    ///CONFIG LOADING==================================
    this->loadSettings();
}
#ifdef WIN32
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)  //GLOBAL HOTKEYS handler
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    if(msg->wParam == 66613)        //our defined wParam id for media key next track
    {
        ui->nextButton->click();
    }
    else if(msg->wParam == 66612)  //our defined wParam id for media key prev track
    {
        ui->prevButton->click();
    }
    else if((msg->wParam == 69 && useMediaHotkeys)|| msg->wParam == 66611)   //69 is wParam of media key play/pause
    {
        ui->tooglePlayingButton->click();
    }

    return false;
}
#endif

#ifdef Q_OS_LINUX
void MainWindow::linuxIconShow() {
#include <stdlib.h>
    system("python2 tray.py");
}
#endif

void MainWindow::loadSettings()
{
#ifdef Q_OS_LINUX
    desktop = getenv("XDG_CURRENT_DESKTOP");
    isUnity = (desktop.toLower() == "unity");
    qDebug()<<"CURRENT DESKTOP: " << desktop;
    if (isUnity)
    {
        QtConcurrent::run(this, &MainWindow::linuxIconShow);
    } else {
        trayIcon->show();
    }
#endif
#ifdef WIN32
    trayIcon->show();
#endif
    restoreGeometry(settings->value("geometry",saveGeometry()).toByteArray());
    ui->volumeSlider->setValue(settings->value("volume",50).toInt());
    QString localToken = settings->value("token","none").toString();
    QString localUid = settings->value("user_id","none").toString();
    minToTray = settings->value("minToTrayOnClose",false).toBool();
    emit loadToken(localToken,localUid);
    netCore->getAudioList();
    //this->offlineDebugFunction();
    useHotkeys = settings->value("useHotkeys",false).toBool();
    useMediaHotkeys = settings->value("useMediaHotkeys",false).toBool();
    emit setPrefWindowsHotkeysUi(useHotkeys, useMediaHotkeys);
    setNewSettings(useHotkeys,useMediaHotkeys,false,"",minToTray);
}

void MainWindow::saveSettings()
{
    settings->setValue("token",netCore->getToken());
    settings->setValue("user_id",netCore->getUid());
    settings->setValue("volume",ui->volumeSlider->value());
    settings->setValue("geometry",saveGeometry());
    settings->setValue("useHotkeys",useHotkeys);
    settings->setValue("useMediaHotkeys",useMediaHotkeys);
    settings->setValue("minToTrayOnClose",minToTray);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(minToTray)
    {
#ifdef WIN32
        if (trayIcon->isVisible())
        {
            this->hide();
            event->ignore();
        }
#elif defined Q_OS_LINUX
        this->hide();
        event->ignore();
#endif
    }
    else
    {
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
    setPausedUi();
    ui->musicWidget->setRowCount(0);
    ui->musicWidget->clear();
    music->clearHistory();
    QStringList header;
    ui->musicWidget->setColumnCount(4);
    header <<"Artist"<<"Title"<<"Duration"<<"link";
    ui->musicWidget->hideColumn(3);
    ui->musicWidget->setHorizontalHeaderLabels(header);
    ui->musicWidget->verticalHeader()->setVisible(false);
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


void MainWindow::setNewSettings(bool use, bool media, bool cache, QString path, bool minTray)
{
    qDebug()<<use<<" "<<media<<" "<<cache<<" "<<path<<" "<<minTray;
#ifdef WIN32
    UnregisterHotKey((HWND)winId(),66613);
    UnregisterHotKey((HWND)winId(),66612);
    UnregisterHotKey((HWND)winId(),66611);
#endif
#ifdef Q_OS_LINUX

#endif
    useHotkeys = use;
    useMediaHotkeys = media;
    useCache = cache;
    minToTray = minTray;
    saveSettings();
    emit setPrefWindowsHotkeysUi(useHotkeys, useMediaHotkeys);
    emit setMinToTray(minToTray);
    if (useHotkeys)
    {
        if (useMediaHotkeys)
        {
#ifdef WIN32
            RegisterHotKey((HWND)winId(),66613, 0, VK_MEDIA_NEXT_TRACK);
            RegisterHotKey((HWND)winId(),66612, 0, VK_MEDIA_PREV_TRACK);
            RegisterHotKey((HWND)winId(),66611, 0, VK_MEDIA_PLAY_PAUSE);
            qDebug()<<"Registered media hotkeys";
#endif
#ifdef Q_OS_LINUX

#endif
        }
        else
        {
#ifdef WIN32
            RegisterHotKey((HWND)winId(),66613, MOD_CONTROL, VK_RIGHT);
            RegisterHotKey((HWND)winId(),66612, MOD_CONTROL, VK_LEFT);
            RegisterHotKey((HWND)winId(),66611, MOD_CONTROL, VK_DOWN);
            qDebug()<<"Registered desktop hotkeys";
#endif
#ifdef Q_OS_LINUX

#endif
        }
    }
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
    QString ab = "It is a Player for playing music from Your Vkontakte playlist.\n"
            "More features will be avalible later.\n"
            "\tcurrent version:  '"+QCoreApplication::applicationVersion()+"'\n"
            " credits:\n"
            "\tMe: kazak1377(Maxim Kozachenko)\n"
            "Thanks to:\n"
            "\tQt team\n"
            "\tmembers of c_plus_plus jabber.ru conference";
    QMessageBox::information(this, tr("About QVkPlayer"),ab);
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
    emit ui->musicWidget->cellClicked(0,0);
}

MainWindow::~MainWindow()
{
    trayIcon->hide();
    saveSettings();
#ifdef Q_OS_LINUX
    system("dbus-send --print-reply --dest=org.mpris.MediaPlayer2.qvkplayer.icon /org/mpris/MediaPlayer2 org.mpris.MediaPlayer2.Quit");
#endif
    delete ui;
    delete trayIcon;
    delete music;
    delete netCore;
}
