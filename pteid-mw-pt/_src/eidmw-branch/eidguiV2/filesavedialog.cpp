/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "filesavedialog.h"

#include <QApplication>
#include <QDebug>
#include <QQuickWindow>
#include <QStandardPaths>

FileSaveDialog::FileSaveDialog(QQuickItem *parent)
    : QQuickItem(parent)
    , m_dlgHelper(init_helper())
    , m_modality(Qt::WindowModal)
    #if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    , m_options(QFileDialogOptions::create())
    #else
    , m_options(QSharedPointer<QFileDialogOptions>(new QFileDialogOptions()))
    #endif
{
    /*
     * Qt Widgets support must be present, i.e. the main app is a QApplication.
     * The following line break at compile time, if the main app is a QGuiApplication
     */
    QApplication *appHasQtWidgetsSupport = qobject_cast<QApplication *>(QCoreApplication::instance());
    Q_ASSERT(appHasQtWidgetsSupport);
    Q_UNUSED(appHasQtWidgetsSupport);

    if (valid())
    {
        connect(m_dlgHelper, &QPlatformFileDialogHelper::accept,
                this, &FileSaveDialog::accept);
        connect(m_dlgHelper, &QPlatformFileDialogHelper::reject,
                this, &FileSaveDialog::reject);
    }else{
        qDebug() << "##########################################################################################";
        qDebug() << "ERROR: The application needs gtk3 platform plugin";
        qDebug() << "To automatically set gtk3 to QT_QPA_PLATFORMTHEME run the following at the command line";
        qDebug() << "echo \"export QT_QPA_PLATFORMTHEME=gtk3\">> ~/.profile && source ~/.profile";
        qDebug() << "##########################################################################################";
    }
}

FileSaveDialog::~FileSaveDialog()
{
    if (m_dlgHelper)
        m_dlgHelper->hide();
    delete m_dlgHelper;
}

bool FileSaveDialog::valid() const
{
    if (m_dlgHelper) return true;
    else return false;
}

QUrl FileSaveDialog::fileUrl() const
{
    return fileUrl_;
}

void FileSaveDialog::setFileUrl(QUrl fileUrl)
{
    if (fileUrl_ != fileUrl)
    {
        fileUrl_ = fileUrl;
        emit fileUrlChanged();
    }
}

QString FileSaveDialog::filename() const
{
    return filename_;
}

void FileSaveDialog::setFilename(QString filename)
{
    if (filename_ != filename)
    {
        filename_ = filename;
        emit filenameChanged();
    }
}

QString FileSaveDialog::title() const
{
    return title_;
}

void FileSaveDialog::setTitle(QString title)
{
    if (title_ != title)
    {
        title_ = title;
        emit titleChanged();
    }
}

QStringList FileSaveDialog::nameFilters() const
{
    return nameFilters_;
}

void FileSaveDialog::setNameFilters(QStringList nameFilters)
{
    if (nameFilters_ != nameFilters)
    {
        nameFilters_ = nameFilters;
        emit nameFiltersChanged();
    }
}

QPlatformFileDialogHelper* FileSaveDialog::init_helper()
{
    return static_cast<QPlatformFileDialogHelper*>(
                QGuiApplicationPrivate::platformTheme()->createPlatformDialogHelper(QPlatformTheme::FileDialog)
                );
}

void FileSaveDialog::open()
{
    if (!valid()) return;

    QQuickItem *parent = this->parentItem();
    Q_ASSERT(parent);

    QQuickWindow *window = parent->window();
    Q_ASSERT(window);

    m_parentWindow = window;

    m_options->setFileMode(QFileDialogOptions::AnyFile);
    m_options->setAcceptMode(QFileDialogOptions::AcceptSave);
    m_options->setWindowTitle(title());
    m_options->setNameFilters(nameFilters());

    /*
     * Mac:
     * Set filename incl. directory via setInitiallySelectedFiles()
     *
     * Windows:
     * Set filename via setInitiallySelectedFiles() and let Windows choose the directory.
     * Default directory: C:\\Users\XYZ\Downloads
     *
     * Gnome:
     * Set directory via QPlatformFileDialogHelper::setDirectory() and leave
     * filename empty, since QGtk2FileDialogHelper can not set non-existing filenames.
     *
     */
    const QString folder = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    const QString name = filename();

#ifdef Q_OS_OSX
    QUrl initialSelection = QUrl::fromLocalFile(QFileInfo(folder, name).absoluteFilePath());
    qDebug() << "Initial file:" << initialSelection;
    m_options->setInitiallySelectedFiles(QList<QUrl>() << initialSelection);
#endif
#ifdef Q_OS_WIN
    qDebug() << "Initial filename:" << name;
    m_options->setInitiallySelectedFiles(QList<QUrl>() << QUrl::fromLocalFile(name));
#endif
#ifdef Q_OS_LINUX
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 2))
    // Wohoo, big fix! https://codereview.qt-project.org/91501
    QUrl initialSelection = QUrl::fromLocalFile(QFileInfo(folder, name).absoluteFilePath());
    qDebug() << "Initial file:" << initialSelection;
    m_options->setInitiallySelectedFiles(QList<QUrl>() << initialSelection);
    #else
    qDebug() << "Initial directory:" << folder;
    m_dlgHelper->setDirectory(QUrl::fromLocalFile(folder));
    #endif
#endif

    m_dlgHelper->setOptions(m_options);
    m_dlgHelper->setFilter(); // applyOptions();

    Qt::WindowFlags flags = Qt::Dialog;
    if (!title().isEmpty()) flags |= Qt::WindowTitleHint;

    m_visible = m_dlgHelper->show(flags, m_modality, m_parentWindow);
}

void FileSaveDialog::close()
{
    if (!valid()) return;

    m_dlgHelper->hide();
    m_visible = false;
}

void FileSaveDialog::accept()
{
    if (!valid()) return;

    m_dlgHelper->hide();

    QList<QUrl> selectedUrls = m_dlgHelper->selectedFiles();
    if (!selectedUrls.empty())
    {
        setFileUrl(selectedUrls.at(0));
    }

    emit accepted();
}

void FileSaveDialog::reject()
{
    if (!valid()) return;

    m_dlgHelper->hide();
    emit rejected();
}
