/***************************************************************************
 *   Copyright (C) 2007-2008 by Elad Lahav
 *   elad_lahav@users.sourceforge.net
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#ifndef __APP_APPLICATION_H
#define __APP_APPLICATION_H

#include <QApplication>
#include "project.h"
#include "engine.h"
#include "codebase.h"

namespace KScope
{

namespace App
{

class MainWindow;

/**
 * The KScope application.
 * Runs the event loop and maintains the active project.
 * @author Elad Lahav
 */
class Application : public QApplication
{
	Q_OBJECT

public:
	Application(int&, char**);
	~Application();

	enum Event { AppInitEvent = QEvent::User };

	int run();
	void loadProject(const QString&);

	Core::ProjectBase* currentProject() { return proj_; }

public slots:
	void closeProject();

signals:
	void hasProject(bool has);

protected:
	void customEvent(QEvent*);

private:
	/**
	 * The main window.
	 */
	MainWindow* mainWnd_;

	/**
	 * The active project.
	 * NULL if no project is currently loaded
	 * @note This is a bit dangerous, as currentProject() is being used without
	 * checking for NULL pointers in several places. A better solution may be
	 * required.
	 */
	Core::ProjectBase* proj_;

	void init();
};

inline Application* theApp() { return static_cast<Application*>(qApp); }
inline Core::ProjectBase* currentProject() {
	return theApp()->currentProject();
}

} // namespace App

} // namespace KScope

#endif // __APP_APPLICATION_H
