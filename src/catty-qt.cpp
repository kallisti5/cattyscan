/*
 * Copyright, 2012 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */


#include "catty-qt.h"

#include <QtGui>

#include "inventory/inventory.h"


ScannerWindow::ScannerWindow()
{
	inventory = new InventoryEngine();

	createActions();
	createTrayIcon();

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

	#if 0
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(iconGroupBox);
	mainLayout->addWidget(messageGroupBox);
	setLayout(mainLayout);
	#endif

	setWindowTitle(tr("CattyScan"));

	trayIcon->show();
	setIcon();

	resize(550, 300);

	// TODO: We just collect a baseline for now on every startup
	inventory->Baseline();
}


ScannerWindow::~ScannerWindow()
{
	if (inventory != NULL)
		delete inventory;
}


void
ScannerWindow::createActions()
{
	minimizeAction = new QAction(tr("Mi&nimize"), this);
	connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

	restoreAction = new QAction(tr("&Restore"), this);
	connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

	quitAction = new QAction(tr("&Quit"), this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}


void
ScannerWindow::createTrayIcon()
{
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(minimizeAction);
	trayIconMenu->addAction(restoreAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
}


void
ScannerWindow::setIcon()
{
	QIcon icon = QIcon("data/CattyScan.svg");

	trayIcon->setIcon(icon);
	setWindowIcon(icon);

	trayIcon->setToolTip("CattyScan");
}


void
ScannerWindow::setVisible(bool visible)
{
	minimizeAction->setEnabled(visible);
	restoreAction->setEnabled(isMaximized() || !visible);

	QDialog::setVisible(visible);
}


void
ScannerWindow::closeEvent(QCloseEvent *event)
{
	if (trayIcon->isVisible()) {
		QMessageBox::information(this, tr("CattyScan"),
			tr("The program will keep running in the "
				"system tray. To terminate the program, "
				"choose <b>Quit</b> in the context menu "
				"of the system tray entry."));
		hide();
		event->ignore();
	}
}


void
ScannerWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
			break;
		case QSystemTrayIcon::MiddleClick:
			showPopupMessage();
			break;
	}
}


void
ScannerWindow::showPopupMessage()
{
	#if 0
	QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
		typeComboBox->itemData(typeComboBox->currentIndex()).toInt());
	trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), icon,
		durationSpinBox->value() * 1000);
	#endif
}


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("Systray"),
			QObject::tr("I couldn't detect any system tray "
			"on this system."));
			return 1;
	}

	QApplication::setQuitOnLastWindowClosed(false);

	ScannerWindow window;
	window.show();

	return app.exec();
}
