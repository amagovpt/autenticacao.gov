/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QQuickItem>
#include <QFileDialog>
#include <QUrl>
#include <private/qguiapplication_p.h>
#include <QtGui/qpa/qplatformdialoghelper.h>
#include <QtGui/qpa/qplatformtheme.h>

class FileSaveDialog : public QQuickItem
{
    Q_OBJECT

public:
    explicit FileSaveDialog(QQuickItem *parent = 0);
    ~FileSaveDialog();

    Q_PROPERTY(bool valid READ valid NOTIFY validChanged)
    bool valid() const;

    Q_PROPERTY(QUrl fileUrl READ fileUrl NOTIFY fileUrlChanged)
    QUrl fileUrl() const;

    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    QString filename() const;
    void setFilename(QString filename);

    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    QString title() const;
    void setTitle(QString title);

    Q_PROPERTY(QStringList nameFilters READ nameFilters WRITE setNameFilters NOTIFY nameFiltersChanged)
    QStringList nameFilters() const;
    void setNameFilters(QStringList nameFilters);

    Q_INVOKABLE void open();
    Q_INVOKABLE void close();

signals:
    void fileUrlChanged();
    void filenameChanged();
    void titleChanged();
    void nameFiltersChanged();
    void accepted();
    void rejected();

    // unused
    void validChanged();

protected:
    QPlatformFileDialogHelper* init_helper();

protected:
    QPlatformFileDialogHelper *m_dlgHelper;
    QQuickWindow *m_parentWindow;
    Qt::WindowModality m_modality;
    bool m_visible;
    QSharedPointer<QFileDialogOptions> m_options;

protected Q_SLOTS:
    virtual void accept();
    virtual void reject();

private:
    void setFileUrl(QUrl fileUrl);

    QUrl fileUrl_;
    QString filename_;
    QString title_;
    QStringList nameFilters_;

    Q_DISABLE_COPY(FileSaveDialog)
};
