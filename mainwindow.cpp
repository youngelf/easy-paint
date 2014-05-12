/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "mainwindow.h"
#include "scribblearea.h"


MainWindow::MainWindow() {
	scribbleArea = new ScribbleArea;
	setCentralWidget(scribbleArea);

	createActions();
    createMenus();
    // But hide them by default.
    menuBar()->hide();

    QMainWindow::showFullScreen();

    setWindowTitle(tr("Easy Paint"));
}

void MainWindow::closeEvent(QCloseEvent * event) {
	if (maybeSave()) {
		event->accept();
	} else {
		event->ignore();
	}
}

void MainWindow::open() {
	if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(
                    this,
                    tr("Open File"),
                    QDir::
                    currentPath());
        if (!fileName.isEmpty())
			scribbleArea->openImage(fileName);
	}
}

void MainWindow::save() {
	QAction *action = qobject_cast < QAction * >(sender());
	QByteArray fileFormat = action->data().toByteArray();
	saveFile(fileFormat);
}

void MainWindow::penColor() {
	QColor newColor = QColorDialog::getColor(scribbleArea->penColor());
	if (newColor.isValid())
		scribbleArea->setPenColor(newColor);
}

void MainWindow::penWidth() {
	bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Scribble"),
						tr("Select pen width:"),
						scribbleArea->penWidth(),
						1, 50, 1, &ok);
	if (ok)
		scribbleArea->setPenWidth(newWidth);
}


void MainWindow::toggleMenu() {
    if (menuBar()->isVisible()) {
     menuBar()->hide();
    } else {
        menuBar()->show();
    }
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Easy Paint"),
			   tr
               ("<p><b>Easy Paint</b> is a simple program to allow a child"
                " to use a mouse.</p>"));
}

void MainWindow::createActions() {
	openAct = new QAction(tr("&Open..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	foreach(QByteArray format, QImageWriter::supportedImageFormats()) {
		QString text = tr("%1...").arg(QString(format).toUpper());

		QAction *action = new QAction(text, this);
		action->setData(format);
		connect(action, SIGNAL(triggered()), this, SLOT(save()));
		saveAsActs.append(action);
	}

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcuts(QKeySequence::Quit);
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	penColorAct = new QAction(tr("&Pen Color..."), this);
    penColorAct->setShortcut(tr("Ctrl+C"));
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));

	penWidthAct = new QAction(tr("Pen &Width..."), this);
    penWidthAct->setShortcut(tr("Ctrl+W"));
	connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));

    toggleMenuAct = new QAction(tr("&Toggle Menu"), this);
    toggleMenuAct->setShortcut(tr("Ctrl+Alt+T"));
    connect(toggleMenuAct, SIGNAL(triggered()),
        this, SLOT(toggleMenu()));

	aboutAct = new QAction(tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("About &Qt"), this);
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // Add these right to the main window.
    addAction(toggleMenuAct);
    addAction(penColorAct);
    addAction(penWidthAct);
}

void MainWindow::createMenus() {
	saveAsMenu = new QMenu(tr("&Save As"), this);
	foreach(QAction * action, saveAsActs)
	    saveAsMenu->addAction(action);

	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(openAct);
	fileMenu->addMenu(saveAsMenu);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	optionMenu = new QMenu(tr("&Options"), this);
	optionMenu->addAction(penColorAct);
	optionMenu->addAction(penWidthAct);
	optionMenu->addSeparator();

	helpMenu = new QMenu(tr("&Help"), this);
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);

	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(optionMenu);
	menuBar()->addMenu(helpMenu);
}

bool MainWindow::maybeSave() {
	if (scribbleArea->isModified()) {
		QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Easy Paint"),
					   tr("The image has been modified.\n"
					      "Do you want to save your changes?"),
					   QMessageBox::Save | QMessageBox::
					   Discard | QMessageBox::Cancel);
		if (ret == QMessageBox::Save) {
			return saveFile("png");
		} else if (ret == QMessageBox::Cancel) {
			return false;
		}
	}
	return true;
}

bool MainWindow::saveFile(const QByteArray & fileFormat) {
    QString initialPath = QDir::currentPath() + "/easy-p1." + fileFormat;

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
							initialPath,
							tr
							("%1 Files (*.%2);;All Files (*)")
							.
							arg(QString
							    (fileFormat.
							     toUpper()))
							.
							arg(QString
							    (fileFormat)));
	if (fileName.isEmpty()) {
		return false;
	} else {
		return scribbleArea->saveImage(fileName, fileFormat);
	}
}
