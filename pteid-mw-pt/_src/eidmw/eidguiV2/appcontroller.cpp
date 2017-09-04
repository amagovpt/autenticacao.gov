#include "appcontroller.h"
#include <QObject>
#include <QCursor>
#include <QDebug>

AppController::AppController(GUISettings& settings,QObject *parent) :
    QObject(parent)
  , m_Language(GenPur::LANG_EN)
  , m_Settings(settings)
{
    //------------------------------------
    // install the translator object and load the .qm file for
    // the given language.
    //------------------------------------
    qApp->installTranslator(&m_translator);

    GenPur::UI_LANGUAGE CurrLng   = m_Settings.getGuiLanguageCode();
    GenPur::UI_LANGUAGE LoadedLng = LoadTranslationFile(CurrLng);

    m_Language = LoadedLng;
}

GenPur::UI_LANGUAGE AppController::LoadTranslationFile(GenPur::UI_LANGUAGE NewLanguage)
{

    QString strTranslationFile;
    strTranslationFile = QString("eidmw_") + GenPur::getLanguage(NewLanguage);

    if (!m_translator.load(strTranslationFile,m_Settings.getExePath()+"/"))
    {
        // this should not happen, since we've built the menu with the translation filenames
    }

    return NewLanguage;
}

QVariant AppController::getCursorPos()
{
    return QVariant(QCursor::pos());
}
bool AppController::getAutoCardReadingValue (void){

    return m_Settings.getAutoCardReading();
}
void AppController::setAutoCardReadingValue (bool bAutoCardReading ){

    m_Settings.setAutoCardReading(bAutoCardReading);
}
bool AppController::getStartAutoValue (void){

    return m_Settings.getAutoStartup();
}
void AppController::setStartAutoValue (bool bAutoStartup ){

    m_Settings.setAutoStartup(bAutoStartup);
}
bool AppController::getStartMinimizedValue (void){

    return m_Settings.getStartMinimized();
}
void AppController::setStartMinimizedValue (bool bStartMinimized ){

    m_Settings.setStartMinimized(bStartMinimized);
}
int AppController::getGuiLanguageCodeValue (void){

    return m_Settings.getGuiLanguageCode();
}
void AppController::setGuiLanguageCodeValue (int language){
     m_Settings.setGuiLanguage((GenPur::UI_LANGUAGE)language);
}
bool AppController::getShowNotificationValue (void){

    return m_Settings.getShowNotification();
}
bool AppController::getShowPictureValue (void){

    return m_Settings.getShowPicture();
}
bool AppController::getShowAnimationsValue(void){
    return m_Settings.getShowAnimations();
}
bool AppController::getRegCertValue (void){

    return m_Settings.getRegCert();
}
bool AppController::getRemoveCertValue (void){

    return m_Settings.getRemoveCert();
}
void AppController::setShowNotificationValue (bool bShowNotification){

    m_Settings.setShowNotification(bShowNotification);
}
void AppController::setShowPictureValue (bool bShowPicture){

    m_Settings.setShowPicture(bShowPicture);
}
void AppController::setShowAnimationsValue(bool bShowAnimations){
    m_Settings.setShowAnimations(bShowAnimations);
}
void AppController::setRegCertValue (bool bRegCert){

    m_Settings.setRegCert(bRegCert);
}
void AppController::setRemoveCertValue (bool bRemoveCert){

    m_Settings.setRemoveCert(bRemoveCert);
}
QString AppController::getTimeStampHostValue (void){
    return m_Settings.getTimeStampHost();
}
void AppController::setTimeStampHostValue (QString const& timeStamp_host){
    m_Settings.setTimeStampHost(timeStamp_host);
}
QString AppController::getProxyHostValue (void){

    return m_Settings.getProxyHost();
}
void AppController::setProxyHostValue (QString const& proxy_host){
    m_Settings.setProxyHost(proxy_host);
}
long AppController::getProxyPortValue (void){

    return m_Settings.getProxyPort();
}
void AppController::setProxyPortValue (int proxy_port){
    m_Settings.setProxyPort(proxy_port);
}
QString AppController::getProxyUsernameValue (void){

    return m_Settings.getProxyUsername();
}
void AppController::setProxyUsernameValue (QString const& proxy_user){
    m_Settings.setProxyUsername(proxy_user);
}
QString AppController::getProxyPwdValue (void){

    return m_Settings.getProxyPwd();
}
void AppController::setProxyPwdValue (QString const& proxy_pwd){
    m_Settings.setProxyPwd(proxy_pwd);
}
