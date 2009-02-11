/***************************************************************************
 *   Copyright (C) 2007-2009 by Elad Lahav
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

#ifndef __EDITOR_CONFIG_H__
#define __EDITOR_CONFIG_H__

#include <QObject>
#include <QSettings>
#include <qscilexercpp.h>

namespace KScope
{

namespace Editor
{

class Editor;

/**
 * Manages editor configuration.
 * @author Elad Lahav
 */
class Config : public QObject
{
	Q_OBJECT

public:
	Config(QObject* parent = NULL);
	~Config();

	void load(const QSettings&);
	void store(QSettings&) const;
	void apply(Editor*) const;
	QsciLexer* lexer(const QString&) const;

private:
	/**
	 * Default font.
	 */
	QFont font_;

	/**
	 * Whether to highlight the current line.
	 */
	bool hlCurLine_;

	/**
	 * Whether to use tabs for indentation.
	 */
	bool indentTabs_;

	/**
	 * The tab width, in characters.
	 */
	int tabWidth_;

	/**
	 * Manages styles for C/C++ text formatting.
	 */
	QsciLexerCPP* cppLexer_;

	/**
	 * Maps file suffixes to lexers.
	 */
	QMap<QString, QsciLexer*> lexerMap_;

	void fromEditor(Editor* editor = NULL);
};

} // namespace Editor

} // namespace KScope

#endif // __EDITOR_CONFIG_H__
