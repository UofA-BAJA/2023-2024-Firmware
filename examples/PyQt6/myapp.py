# Sys to access command line arguments
import sys
from PyQt6 import uic
from PyQt6.QtCore import QSize, Qt
from PyQt6.QtGui import QContextMenuEvent, QMouseEvent, QAction
from PyQt6.QtWidgets import QApplication, QMainWindow, QPushButton, QMenu

app = QApplication(sys.argv)


window = uic.loadUi("mainwindow.ui")
window.show()

app.exec()