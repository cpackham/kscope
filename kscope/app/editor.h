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

#ifndef __APP_EDITOR_H
#define __APP_EDITOR_H

#include <qsciscintilla.h>
#include <QSettings>

namespace KScope
{

namespace App
{

/**
 * An QScintilla editor widget used to view/edit files.
 * @author Elad Lahav
 */
class Editor : public QsciScintilla
{
	Q_OBJECT

public:
	Editor(QWidget* parent = 0);
	~Editor();

	/**
	 * Provides editor configuration parameters.
	 */
	struct Config
	{
		void load(QSettings& settings);
		void store(QSettings& settings);

		QFont font_;
		bool hlCurLine_;
		bool indentTabs_;
		int tabWidth_;
	};

	/**
	 * Parameters for text searches.
	 */
	struct FindParams
	{
		/**
		 * The pattern to search for.
		 */
		QString pattern_;

		/**
		 * Whether the pattern represents a regular expression.
		 */
		bool regExp_;

		/**
		 * Whether the search is case sensitive.
		 */
		bool caseSensitive_;

		/**
		 * Whether to look for whole words only.
		 */
		bool wholeWordsOnly_;

		/**
		 * Whether the search should wrap at the end of the document.
		 */
		bool wrap_;

		/**
		 * Whether to search forward in the document.
		 */
		bool forward_;
	};

	bool load(const QString&);
	void setCursorPosition(uint, uint);
	QString currentSymbol() const;
	void setFocus();
	void applyConfig(const Config&);
	void getConfig(Config&);

public slots:
	void find();
	void find(const Editor::FindParams&);

private:
	bool isLoading_;
	uint onLoadLine_;
	uint onLoadColumn_;
	bool onLoadFocus_;

private slots:
	void loadDone(const QString&);
};

} // namespace App

} // namespace KScope

#endif  // __APP_EDITOR_H
