/*  IQNotes - Smarty notes
    Copyright (C) 2001, 2002 Peter Vrabel <kybu@kybu.sk>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#include "cfgFile.h"

CfgFileParser::CfgFileParser() : QXmlDefaultHandler()
{
}

bool CfgFileParser::startDocument()
{
    return true;
}

bool CfgFileParser::startElement(const QString &, const QString &, const QString &qName, const QXmlAttributes &attr)
{
    if (qName == "config")
    {
        NotesConfig config;

        config.rootNodeDecoration = attr.value("rootnodedecoration") == "yes" ? true : false;
        config.showScrollBars = attr.value("showscrollbars") == "yes" ? true : false;
        config.wordWrap = attr.value("wordwrap") == "yes" ? true : false;
        config.verticalLayout = attr.value("verticalLayout") == "no" ? false : true;
        config.font = attr.value("font");
        config.fontSize = attr.value("fontsize").toInt();

        emit configLoaded(config);
    }

    return true;
}

bool CfgFileParser::endElement(const QString&, const QString&, const QString& qName)
{
    return true;
}
