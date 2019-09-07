#-------------------------------------------------
#
# Project created by QtCreator 2015-04-25T15:33:53
#
#-------------------------------------------------

QT       += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include ( C:/Qwt-6.1.3/features/qwt.prf ) # Qwt (graphic charts framework)

#RC_FILE = applogo.rc

TARGET = YesYouCAN
TEMPLATE = app

INCLUDEPATH += src ui ressources src/canTraceTab src/canTransmitterTab src/common src/dbcTreeTab src/decoding src/interfaces/drivers src/interfaces/dialogs src/messageViewerTab src/misc src/signalViewerTab src/signalViewerTab/graphicsObjects src/interfaces/drivers/pcan-driver


SOURCES += \ 
    src/canTraceTab/dialog_exportdialog.cpp \
    src/canTraceTab/model_cantrace.cpp \
    src/canTraceTab/model_customfilter.cpp \
    src/canTraceTab/widget_cantrace.cpp \
    src/canTransmitterTab/model_cantransmit.cpp \
    src/common/dialog_dbcdialog.cpp \
    src/common/dialog_itemdialog.cpp \
    src/common/pannel_canmsgdetails.cpp \
    src/common/widget_abstracttab.cpp \
    src/dbcTreeTab/widget_dbctree.cpp \
    src/decoding/candecoder.cpp \
    src/decoding/struct_canmessage.cpp \
    src/decoding/struct_cansignal.cpp \
    src/decoding/struct_dbc.cpp \
    src/interfaces/dialogs/dialog_interfaceconfigdialog.cpp \
    src/interfaces/dialogs/dialog_pcandialog.cpp \
    src/interfaces/dialogs/dialog_wcandialog.cpp \
    src/interfaces/drivers/pcan-driver/pcandriver.cpp \
    src/interfaces/drivers/interface_hwinterface.cpp \
    src/interfaces/drivers/interface_pcan.cpp \
    src/interfaces/drivers/interface_wcan.cpp \
    src/misc/string_manip.cpp \
    src/signalViewerTab/graphicsObjects/customgraphicsitem.cpp \
    src/signalViewerTab/graphicsObjects/textitem.cpp \
    src/signalViewerTab/customgraphicsscene.cpp \
    src/dock_project.cpp \
    src/document.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/canTransmitterTab/widget_cantransmitter.cpp \
    src/canTransmitterTab/dialog_messageconstruction.cpp \
    src/common/model_documenttree.cpp \
    src/messageViewerTab/model_canmessageviewer.cpp \
    src/messageViewerTab/widget_canmessageviewer.cpp \
    src/signalViewerTab/widget_cansignalviewer.cpp \
    src/signalViewerTab/dialog_graphicsobject.cpp \
    src/canTransmitterTab/view_customtransmittable.cpp \
    src/signalViewerTab/graphicsObjects/baritem.cpp \
    src/misc/graphicsitem_manip.cpp \
    src/signalViewerTab/customgraphicsview.cpp \
    src/interfaces/drivers/interface_loopback.cpp \
    src/signalViewerTab/graphicsObjects/colorshapeitem.cpp \
    src/application.cpp \
    src/decoding/filterexpression.cpp \
    src/canTraceTab/filterlineedit.cpp \
    src/common/signalselectordialog.cpp
    

HEADERS  += \ 
    src/canTraceTab/dialog_exportdialog.h \
    src/canTraceTab/model_cantrace.h \
    src/canTraceTab/model_customfilter.h \
    src/canTraceTab/widget_cantrace.h \
    src/canTransmitterTab/model_cantransmit.h \
    src/common/dialog_dbcdialog.h \
    src/common/pannel_canmsgdetails.h \
    src/common/widget_abstracttab.h \
    src/dbcTreeTab/widget_dbctree.h \
    src/decoding/candecoder.h \
    src/decoding/struct_canmessage.h \
    src/decoding/struct_cansignal.h \
    src/decoding/struct_dbc.h \
    src/interfaces/dialogs/dialog_interfaceconfigdialog.h \
    src/interfaces/dialogs/dialog_pcandialog.h \
    src/interfaces/dialogs/dialog_wcandialog.h \
    src/interfaces/drivers/pcan-driver/PCANBasic.h \
    src/interfaces/drivers/pcan-driver/pcandriver.h \
    src/interfaces/drivers/interface_hwinterface.h \
    src/interfaces/drivers/interface_pcan.h \
    src/interfaces/drivers/interface_wcan.h \
    src/misc/string_manip.h \
    src/signalViewerTab/graphicsObjects/customgraphicsitem.h \
    src/signalViewerTab/graphicsObjects/textitem.h \
    src/signalViewerTab/customgraphicsscene.h \
    src/dock_project.h \
    src/document.h \
    src/mainwindow.h \
    src/canTransmitterTab/widget_cantransmitter.h \
    src/canTransmitterTab/dialog_messageconstruction.h \
    src/common/dialog_itemdialog.h \
    src/common/model_documenttree.h \
    src/messageViewerTab/model_canmessageviewer.h \
    src/messageViewerTab/widget_canmessageviewer.h \
    src/signalViewerTab/widget_cansignalviewer.h \
    src/signalViewerTab/dialog_graphicsobject.h \
    src/canTransmitterTab/view_customtransmittable.h \
    src/signalViewerTab/graphicsObjects/baritem.h \
    src/misc/graphicsitem_manip.h \
    src/signalViewerTab/customgraphicsview.h \
    src/interfaces/drivers/interface_loopback.h \
    src/signalViewerTab/graphicsObjects/colorshapeitem.h \
    src/application.h \
    src/decoding/filterexpression.h \
    src/canTraceTab/filterlineedit.h \
    src/common/signalselectordialog.h
    

DISTFILES += \
    TODO.txt \
    applogo.rc

RESOURCES += \
    ressources/ressources.qrc

FORMS += \
    ui/graphicsobjectdialog.ui
