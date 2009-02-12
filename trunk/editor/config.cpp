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

#include <QFileInfo>
#include "config.h"
#include "editor.h"

namespace KScope
{

namespace Editor
{

/**
 * Class constructor.
 * @param  parent Parent object
 */
Config::Config(QObject* parent) : QObject(parent)
{
	cppLexer_ = new QsciLexerCPP(this);
}

/**
 * Class destructor.
 */
Config::~Config()
{
}

/**
 * Reads editor configuration parameters from a QSettings object.
 * @param  settings The object to read from
 */
void Config::load(const QSettings& settings)
{
	// Get the current (default) configuration.
	Editor editor;
	font_ = editor.font();
	indentTabs_ = editor.indentationsUseTabs();
	tabWidth_ = editor.tabWidth();
	hlCurLine_ = false;

	// Read values from the settings object.
	font_ = settings.value("DefaultFont", font_).value<QFont>();
	hlCurLine_ = settings.value("HighlightCurrentLine", hlCurLine_).toBool();
	indentTabs_ = settings.value("IndentWithTabs", indentTabs_).toBool();
	tabWidth_ = settings.value("TabWidth", tabWidth_).toInt();

	// Load the C lexer parameters.
	cppLexer_->readSettings(const_cast<QSettings&>(settings), "CPPLexer");

	// Create the file suffix to lexer map.
	// TODO: Make this configurable.
	lexerMap_["c"] = cppLexer_;
	lexerMap_["cc"] = cppLexer_;
	lexerMap_["cpp"] = cppLexer_;
	lexerMap_["h"] = cppLexer_;
	lexerMap_["hpp"] = cppLexer_;
}

/**
 * Writes the current configuration parameters to a QSettings object.
 * @param  settings The object to write to
 */
void Config::store(QSettings& settings) const
{
	// Store editor configuration.
	settings.setValue("Font", font_);
	settings.setValue("HighlightCurrentLine", hlCurLine_);
	settings.setValue("IndentWithTabs", indentTabs_);
	settings.setValue("TabWidth", tabWidth_);

	// Store C lexer parameters.
	cppLexer_->writeSettings(settings, "CPPLexer");
}

/**
 * Updates the settings of the current editor to reflect the current
 * configuration parameters.
 * @param  editor The editor to which to apply the configuration
 */
void Config::apply(Editor* editor) const
{
	editor->setIndentationsUseTabs(indentTabs_);
	editor->setTabWidth(tabWidth_);
	editor->setCaretLineVisible(hlCurLine_);

	if (editor->lexer())
		editor->lexer()->refreshProperties();
}

/**
 * Provides a lexer for the given file.
 * @param  path The path to the file
 * @return The appropriate lexer
 */
QsciLexer* Config::lexer(const QString& path) const
{
	QString suffix = QFileInfo(path).suffix();
	QMap<QString, QsciLexer*>::ConstIterator itr = lexerMap_.find(suffix);
	if (itr == lexerMap_.end())
		return NULL;

	return *itr;
}

} // namespace Editor

} // namespace KScope
