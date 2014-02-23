/*
 * Copyright, 2012 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */
#ifndef CATTYQT_H
#define CATTYQT_H


#include "inventory/inventory.h"

#include <QSystemTrayIcon>
#include <QDialog>


class ScannerWindow : public QDialog
{
public:
							ScannerWindow();
							~ScannerWindow();

			void			setVisible(bool visible);

protected:
			void			closeEvent(QCloseEvent *event);

private slots:
			void 			setIcon();
			void 			iconActivated(QSystemTrayIcon::ActivationReason
								reason);
			void 			showPopupMessage();

private:
			void			createActions();
			void			createTrayIcon();

			InventoryEngine* inventory;

			QAction*		minimizeAction;
			QAction*		restoreAction;
			QAction*		quitAction;

			QSystemTrayIcon* trayIcon;
			QMenu*			trayIconMenu;
};
#endif /* CATTYQT_H */
