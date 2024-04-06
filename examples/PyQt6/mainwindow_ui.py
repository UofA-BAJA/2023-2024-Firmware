# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'mainwindow.ui'
##
## Created by: Qt User Interface Compiler version 6.6.1
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QAction, QBrush, QColor, QConicalGradient,
    QCursor, QFont, QFontDatabase, QGradient,
    QIcon, QImage, QKeySequence, QLinearGradient,
    QPainter, QPalette, QPixmap, QRadialGradient,
    QTransform)
from PySide6.QtWidgets import (QApplication, QMainWindow, QMenu, QMenuBar,
    QSizePolicy, QStackedWidget, QStatusBar, QTextEdit,
    QVBoxLayout, QWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(825, 620)
        self.actionHello_there = QAction(MainWindow)
        self.actionHello_there.setObjectName(u"actionHello_there")
        self.actionWoah_that_s_neat = QAction(MainWindow)
        self.actionWoah_that_s_neat.setObjectName(u"actionWoah_that_s_neat")
        self.actionHello = QAction(MainWindow)
        self.actionHello.setObjectName(u"actionHello")
        self.actionHi = QAction(MainWindow)
        self.actionHi.setObjectName(u"actionHi")
        self.actionIdk_shrug = QAction(MainWindow)
        self.actionIdk_shrug.setObjectName(u"actionIdk_shrug")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.verticalLayout = QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.stackedWidget = QStackedWidget(self.centralwidget)
        self.stackedWidget.setObjectName(u"stackedWidget")
        self.page = QWidget()
        self.page.setObjectName(u"page")
        self.textEdit = QTextEdit(self.page)
        self.textEdit.setObjectName(u"textEdit")
        self.textEdit.setGeometry(QRect(230, 80, 104, 71))
        self.stackedWidget.addWidget(self.page)
        self.page_2 = QWidget()
        self.page_2.setObjectName(u"page_2")
        self.stackedWidget.addWidget(self.page_2)

        self.verticalLayout.addWidget(self.stackedWidget)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 825, 22))
        self.menuWhat_does_this_do = QMenu(self.menubar)
        self.menuWhat_does_this_do.setObjectName(u"menuWhat_does_this_do")
        self.menuAnother_little_fun_thingy = QMenu(self.menubar)
        self.menuAnother_little_fun_thingy.setObjectName(u"menuAnother_little_fun_thingy")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.menubar.addAction(self.menuWhat_does_this_do.menuAction())
        self.menubar.addAction(self.menuAnother_little_fun_thingy.menuAction())
        self.menuWhat_does_this_do.addSeparator()
        self.menuWhat_does_this_do.addAction(self.actionHello_there)
        self.menuWhat_does_this_do.addSeparator()
        self.menuWhat_does_this_do.addAction(self.actionWoah_that_s_neat)
        self.menuAnother_little_fun_thingy.addSeparator()
        self.menuAnother_little_fun_thingy.addAction(self.actionHello)
        self.menuAnother_little_fun_thingy.addAction(self.actionHi)
        self.menuAnother_little_fun_thingy.addAction(self.actionIdk_shrug)

        self.retranslateUi(MainWindow)

        self.stackedWidget.setCurrentIndex(1)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"MainWindow", None))
        self.actionHello_there.setText(QCoreApplication.translate("MainWindow", u"Hello there", None))
        self.actionWoah_that_s_neat.setText(QCoreApplication.translate("MainWindow", u"Woah that's neat", None))
        self.actionHello.setText(QCoreApplication.translate("MainWindow", u"Hello :)", None))
        self.actionHi.setText(QCoreApplication.translate("MainWindow", u"Hi", None))
        self.actionIdk_shrug.setText(QCoreApplication.translate("MainWindow", u"Idk :shrug:", None))
        self.textEdit.setHtml(QCoreApplication.translate("MainWindow", u"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">uh idrk</p></body></html>", None))
        self.menuWhat_does_this_do.setTitle(QCoreApplication.translate("MainWindow", u"What does this do?", None))
        self.menuAnother_little_fun_thingy.setTitle(QCoreApplication.translate("MainWindow", u"Another little fun thingy", None))
    # retranslateUi

