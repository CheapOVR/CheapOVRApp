#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QSplashScreen>
#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QStyleFactory>
#include <QFontDatabase>

int main(int argc, char *argv[])
{ 
    QApplication a(argc, argv);
    int id = QFontDatabase::addApplicationFont(":/fonts/Cantarell-Regular.otf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont cantarell(family);
    QApplication::setFont(cantarell);
    a.setStyle(QStyleFactory::create("Fusion"));
    a.setWindowIcon(QIcon(":/images/logo.svg"));

    QWidget splash;
    splash.resize(600, 400);
    splash.setAttribute(Qt::WA_TranslucentBackground, true);
    QPixmap img (":/images/splash.svg");
    QLabel *label = new QLabel();
    label->setPixmap(img);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(label);
    splash.setLayout(layout);
    splash.setWindowFlags(Qt::WindowType::FramelessWindowHint);
    splash.show();
    a.processEvents();


    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "vrconf_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    // w.show();
    QTimer::singleShot(2000, &splash, SLOT(close()));
    QTimer::singleShot(2000, &w, SLOT(show()));
    // // splash.finish(&w);
    
    return a.exec();
}
