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
 * Adds more information to a QsciLexer class.
 * Inherits from both a QsciLexer-derived class, and LexerExInterface.
 * @author Elad Lahav
 */
template<class LexerT>
class Lexer : public LexerT, public LexerExInterface
{
public:
	/**
	 * Class constructor.
	 * @param  parent Parent object
	 */
	Lexer(QObject* parent) : LexerT(parent) {}
	~Lexer() {}

	/**
	 * @return Whether to use the global font for all styles.
	 */
	bool useGlobalFont() const {
		return useGlobalFont_;
	}

	/**
	 * Toggles the global font parameter.
	 * If the parameter is turned on, then the global font is applied to all
	 * styles.
	 * @param  use true to use the global font, false otherwise
	 */
	void setUseGlobalFont(bool use) {
		useGlobalFont_ = use;
		if (useGlobalFont_)
			LexerT::setFont(QsciLexer::defaultFont());
	}

	/**
	 * Changes the global font.
	 * The font is applied to all styles if the "use global font" flag is set.
	 * @param  font The new global font to set
	 */
	void setGlobalFont(const QFont& font) {
		LexerT::setDefaultFont(font);
		if (useGlobalFont_)
			LexerT::setFont(QsciLexer::defaultFont());
	}

protected:
	/**
	 * Reads lexer configuration parameters from a QSettings object.
	 * @param  settings The object to read from
	 * @param  prefix   Identifies the lexer in the settings object
	 * @return true if successful, false otherwise
	 */
	bool readProperties(QSettings& settings, const QString& prefix) {
		bool result = LexerT::readProperties(settings, prefix);
		useGlobalFont_ = settings.value(prefix + "UseGlobalFont",
		                                false).toBool();
		return result;
	}

	/**
	 * Writes lexer configuration parameters to a QSettings object.
	 * @param  settings The object to write to
	 * @param  prefix   Identifies the lexer in the settings object
	 * @return true if successful, false otherwise
	 */
	bool writeProperties(QSettings& settings, const QString& prefix) const {
		bool result = LexerT::writeProperties(settings, prefix);
		settings.setValue(prefix + "UseGlobalFont", useGlobalFont_);
		return result;
	}

protected:
	/**
	 * Whether all styles of the lexer should be using the global font.
	 * If this flag is set, the style's font cannot be modified separately.
	 * Instead, changes to the global font are applied to all styles.
	 */
	bool useGlobalFont_;
};

/**
 * Class constructor.
 * @param  parent Parent object
 */
Config::Config(QObject* parent) : QObject(parent)
{
	// Create the lexers.
	cppLexer_ = new Lexer<QsciLexerCPP>(this);
	makeLexer_ = new Lexer<QsciLexerMakefile>(this);
	bashLexer_ = new Lexer<QsciLexerBash>(this);

	lexers_ << cppLexer_ << makeLexer_ << bashLexer_;
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
	font_ = settings.value("Font", font_).value<QFont>();
	hlCurLine_ = settings.value("HighlightCurrentLine", hlCurLine_).toBool();
	indentTabs_ = settings.value("IndentWithTabs", indentTabs_).toBool();
	tabWidth_ = settings.value("TabWidth", tabWidth_).toInt();

	// Load the C lexer parameters.
	foreach (QsciLexer* lexer, lexers_) {
		lexer->readSettings(const_cast<QSettings&>(settings), lexer->lexer());
		lexer->setDefaultFont(font_);
	}

	// Create the file to lexer map.
	// TODO: Make this configurable.
	lexerMap_.clear();
	lexerMap_["*.c"] = cppLexer_;
	lexerMap_["*.cc"] = cppLexer_;
	lexerMap_["*.cpp"] = cppLexer_;
	lexerMap_["*.h"] = cppLexer_;
	lexerMap_["*.hpp"] = cppLexer_;
	lexerMap_["Makefile*"] = makeLexer_;
	lexerMap_["*.sh"] = bashLexer_;
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
	foreach (QsciLexer* lexer, lexers_)
		lexer->writeSettings(settings, lexer->lexer());
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
	QString name = QFileInfo(path).fileName();
	return lexerMap_.find(path);
}

} // namespace Editor

} // namespace KScope
