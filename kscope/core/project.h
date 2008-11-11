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

#ifndef __CORE_PROJECT_H
#define __CORE_PROJECT_H

#include <QSettings>
#include <QDir>
#include <QtDebug>
#include "engine.h"
#include "codebase.h"
#include "exception.h"

namespace KScope
{

namespace Core
{

/**
 * Abstract base-class for projects.
 * A project consists of a code base, and an engine that indexes it.
 * @author Elad Lahav
 */
class ProjectBase
{
public:
	ProjectBase() {}
	virtual ~ProjectBase() {}

	/**
	 * Parameters for to configuring projects.
	 */
	struct Params {
		/**
		 * The name of the project (to be displayed to the user).
		 */
		QString name_;

		/**
		 * The root path of the code base.
		 */
		QString rootPath_;

		/**
		 * A string used to initialise the engine (implementation-dependent).
		 */
		QString engineString_;

		/**
		 * A string used to initialise the code base (implementation-dependent).
		 */
		QString codebaseString_;
	};

	/**
	 * Opens the project.
	 */
	virtual void open() = 0;

	/**
	 * Creates a new project.
	 * @param  params  Project parameters
	 */
	virtual void create(const Params& params) = 0;

	/**
	 * Closes the project.
	 */
	virtual void close() = 0;

	/**
	 * @return The name of the project
	 */
	virtual QString name() = 0;

	/**
	 * @return The root path for the code base
	 */
	virtual QString rootPath() = 0;

	/**
	 * @return Pointer to the engine
	 */
	virtual const Engine* engine() = 0;

	/**
	 * @return Pointer to the code base
	 */
	virtual Codebase* codebase() = 0;
};

/**
 * Class template for projects using a standard configuration file.
 * This is expected to be the (parameterised) base class for most project
 * implementations.
 * @author Elad Lahav
 */
template<class EngineT, class CodebaseT>
class Project : public ProjectBase
{
public:
	/**
	 * Class constructor.
	 * Attempts to read the name of the project from the given configuration
	 * file.
	 * This given file may not exist (e.g., if this object is used to create a
	 * new project). The constructor will still succeed, but open() will fail.
	 * @param  configPath  The path to the configuration file
	 * @return
	 */
	Project(const QString& configPath) : configPath_(configPath) {
		QSettings projConfig(configPath_, QSettings::IniFormat);
		name_ = projConfig.value("Project/Name", "").toString();
		qDebug() << "Project loaded (name='" << name_ << "')";
	}

	/**
	 * Class destructor.
	 */
	virtual ~Project() {}

	/**
	 * Opens the project.
	 * Loads project parameters from the configuration file, and initialises the
	 * code base and engine.
	 * The configuration file must have been successfully opened in the
	 * costructor, or by a call to create(), for this method to succeed.
	 * @throw Exception
	 */
	virtual void open() {
		if (name_.isEmpty())
			throw new Exception("The project file does not exist");

		QSettings projConfig(configPath_, QSettings::IniFormat);
		QString cbString, engString;

		// Get the code base and engine initialisation strings from the
		// project's configuration file.
		projConfig.beginGroup("Project");
		rootPath_ = projConfig.value("RootPath", "/").toString();
		engString = projConfig.value("EngineString").toString();
		cbString = projConfig.value("CodebaseString").toString();
		projConfig.endGroup();

		qDebug() << "Project opened (EngineString='" << engString
		         << "' CodebaseString='" << cbString << "')";

		// Handle initialisation strings given as paths relative to the
		// configuration file's directory.
		QDir::setCurrent(QFileInfo(configPath_).dir().path());

		try {
			// Prepare the symbol engine.
			engine_.open(engString);

			// Load the code base.
			codebase_.load(cbString);
		}
		catch (Exception* e) {
			throw e;
		}
	}

	/**
	 * Creates a new project.
	 * Note that the project is not open after it has been created.
	 * @param  params  Project parameters
	 * @throw  Exception
	 */
	virtual void create(const Params& params) {
		// Do not write over an existing configuration file.
		QFileInfo fi(configPath_);
		if (fi.exists()) {
			throw new Exception(QString("The file '%1' already exists")
			                    .arg(configPath_));
		}

		// Make sure the directory to contain the new configuration file
		// exists. Create it if necessary.
		QDir dir(fi.dir());
		if (!dir.exists() && !dir.mkpath(fi.path())) {
			throw new Exception(QString("Failed to create the directory '%1'")
			                    .arg(fi.path()));
		}

		// Write the configuration file.
		QSettings projConfig(configPath_, QSettings::IniFormat);
		projConfig.beginGroup("Project");
		projConfig.setValue("Name", params.name_);
		projConfig.setValue("RootPath", params.rootPath_);
		projConfig.setValue("EngineString", params.engineString_);
		projConfig.setValue("CodebaseString", params.codebaseString_);
		projConfig.endGroup();

		name_ = params.name_;
	}

	virtual void close() {
		name_ = QString();
	}

	virtual QString name() { return name_; }
	virtual QString rootPath() { return rootPath_; }

	virtual const Engine* engine() { return &engine_; }
	virtual Codebase* codebase() { return &codebase_; }

protected:
	/**
	 * The path to the project's configuration file.
	 */
	QString configPath_;

	/**
	 * The name of the project.
	 */
	QString name_;

	/**
	 * An optional common root-path for all source files in the code base.
	 */
	QString rootPath_;

	/**
	 * The indexing engine.
	 */
	EngineT engine_;

	/**
	 * The code base.
	 */
	CodebaseT codebase_;
};

}

}

#endif /* __CORE_PROJECT_H */
