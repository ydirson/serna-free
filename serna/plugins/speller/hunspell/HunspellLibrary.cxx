//
// Copyright(c) 2011 Syntext, Inc. All Rights Reserved.
// Contact: info@syntext.com, http://www.syntext.com
//
// This file is part of Syntext Serna XML Editor.
//
// COMMERCIAL USAGE
// Licensees holding valid Syntext Serna commercial licenses may use this file
// in accordance with the Syntext Serna Commercial License Agreement provided
// with the software, or, alternatively, in accorance with the terms contained
// in a written agreement between you and Syntext, Inc.
//
// GNU GENERAL PUBLIC LICENSE USAGE
// Alternatively, this file may be used under the terms of the GNU General
// Public License versions 2.0 or 3.0 as published by the Free Software
// Foundation and appearing in the file LICENSE.GPL included in the packaging
// of this file. In addition, as a special exception, Syntext, Inc. gives you
// certain additional rights, which are described in the Syntext, Inc. GPL
// Exception for Syntext Serna Free Edition, included in the file
// GPL_EXCEPTION.txt in this package.
//
// You should have received a copy of appropriate licenses along with this
// package. If not, see <http://www.syntext.com/legal/>. If you are unsure
// which license is appropriate for your use, please contact the sales
// department at sales@syntext.com.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#include "HunspellLibrary.h"
#include "hunspelldll.h"
#include "common/Singleton.h"
#include "common/PathName.h"
#include "utils/Config.h"
#include <QTextCodec>
#include <QDir>
#include <map>
#include <iostream>

#define SYM(x) const char nm_Hunspell_##x[] = "Hunspell_" #x;
DECLARE_SYMS

// Configuration structure:       
// speller/hunspell
//  lib # path to shared library                    
//  dict-folder {multiple}  # paths to folders with dictionaries
// aliases
//  <lang>real-lang</lang> # e.g. <en>en_US</en>
// use-system-lib # use system library, not one packaged with Serna

using namespace Common;

static const char HSPELL_PROPS[]       = NOTR("speller/hunspell");
static const char HSPELL_DLL[]         = NOTR("lib");
static const char HSPELL_DICT_FOLDER[] = NOTR("dict-dir");
static const char HSPELL_USE_SYSLIB[]  = NOTR("use-system-lib");
static const char HSPELL_ALIASES[]     = NOTR("aliases");

class HunspellLibrary::DictMap : 
    public std::map<String, RefCntPtr<HunHandle> > {};

bool HunspellLibrary::getDictList(SpellChecker::Strings& si) const
{
    DictMap::const_iterator it = dictMap_->begin();
    for (; it != dictMap_->end(); ++it)
        si.push_back(it->first);
    return dictMap_->begin() != dictMap_->end();
}

static String find_system_hspell()
{
#ifndef _WIN32
    static const char* paths[] = { NOTR("/usr/lib"), NOTR("/lib"), 0 };
    QStringList n_filter;
    n_filter << NOTR("libhunspell-*.so*");
    for (const char** pp = paths; *pp; ++pp) {
        QDir pdir(*pp);
        if (!pdir.exists())
            continue;
        QStringList nl(pdir.entryList(n_filter, QDir::Files, QDir::Name));
        while (!nl.empty()) {
            QString n = nl.takeLast();
            if (pdir.exists(n))
                return pdir.absoluteFilePath(n);
        }
    }
#endif
    return String();
}

bool HunspellLibrary::setConfig()
{
    PropertyNode* hspell_props = config().root()->getProperty(HSPELL_PROPS);
    if (0 == hspell_props) {
        setLibError(tr("No hunspell configuration defined"));
        return false;
    }
    String hspell_dll = hspell_props->getString(HSPELL_DLL);
    if (hspell_dll.isEmpty())
        hspell_props->makeDescendant(HSPELL_USE_SYSLIB, "1", true);
    if (hspell_dll.isEmpty() || !PathName::exists(hspell_dll))
        hspell_dll = find_system_hspell();
    if (hspell_dll.isEmpty() || !PathName::exists(hspell_dll)) {
        setLibError(String(
            tr("Can't find hunspell shared library: ")) + hspell_dll);
        return false;
    }
    if (!loadLibrary(hspell_dll)) {
        setLibError(String(tr("Can't load hunspell shared library '")) +
            hspell_dll + NOTR("': ") + errorMsg());
        return false;
    }
    setLibError(); // ok
    QStringList dict_filter;
    dict_filter << NOTR("*_*.dic");
    const PropertyNode* pn = hspell_props->firstChild();
    for (; pn; pn = pn->nextSibling()) {
        if (pn->name() != HSPELL_DICT_FOLDER)
            continue;
        // process dictionary files first, then aliases
        QDir dict_dir(pn->getString());
        QStringList dlist = dict_dir.entryList(dict_filter);
        while (!dlist.empty()) {
            QString dict_file = dlist.takeLast();
            QString lang_code = dict_file.left(dict_file.length() - 4);
            dict_file = dict_dir.absoluteFilePath(dict_file);
            QString aff_file  = 
                dict_dir.absoluteFilePath(lang_code + NOTR(".aff"));
            if (!dict_dir.exists(dict_file) || !dict_dir.exists(aff_file))
                continue;
            RefCntPtr<HunHandle> hh(new HunHandle(dict_file, 
                aff_file, lang_code));
            // first occurence takes precedence
            if (dictMap_->find(lang_code) == dictMap_->end()) 
                (*dictMap_)[lang_code] = hh;
        }
    }
    // process aliases
    pn = hspell_props->getProperty(HSPELL_ALIASES);
    if (0 == pn)
        return true;  // no aliases to process
    for (pn = pn->firstChild(); pn; pn = pn->nextSibling()) {
        DictMap::const_iterator it = dictMap_->find(pn->getString());
        if (it != dictMap_->end()) 
            (*dictMap_)[pn->name()] = it->second;
    }
    return true;
}

HunHandle* HunspellLibrary::getHandle(const String& dict) 
{
    QString lang(dict);
    if (lang.isEmpty())
        lang = "en";
    lang.replace('-', '_');
    DictMap::const_iterator it = dictMap_->find(lang);
    if (it != dictMap_->end()) 
        return it->second->load();
    if (2 != lang.length()) 
        return 0;
    // provide implicit alias for two-char language code
    for (it = dictMap_->begin(); it != dictMap_->end(); ++it) {
        if (it->first.left(2).lower() == String(lang.toLower())) 
            return it->second->load();
    }
    return 0;
}

HunHandle* HunHandle::load()
{
    if (loaded_)
        return this;
    loaded_ = true;
    nstring aff_file(aff_file_.local8Bit());
    nstring dic_file(dic_file_.local8Bit());
    handle_ = HFUN(create)(aff_file.c_str(), dic_file.c_str());
    if (!handle_)
        return 0;
    codec_ = QTextCodec::codecForName(HFUN(get_dic_encoding)(handle_));
    if (!codec_) 
        codec_ = QTextCodec::codecForName(NOTR("iso-8859-1"));
    return this;
}

nstring HunHandle::from_rs(const RangeString& word) const
{
    QByteArray nw(codec_->fromUnicode(word.data(), word.size()));
    return nstring(nw.data(), nw.size());
}

String HunHandle::to_string(const char* s) const
{
    return codec_->toUnicode(s, strlen(s));
}

HunHandle::HunHandle(const Common::String& dic_file,
                     const Common::String& aff_file,
                     const Common::String& lang)
    : handle_(0), loaded_(false),
      dic_file_(dic_file), aff_file_(aff_file), lang_(lang), codec_(0)
{
}

void HunHandle::setSpellChecker(SpellChecker* sc)
{
    spellChecker_ = sc;
}

HunHandle::~HunHandle()
{
}

HunspellLibrary::HunspellLibrary()
    : dictMap_(new DictMap)
{
}

HunspellLibrary::~HunspellLibrary()
{
    DynamicLibrary::unload();
}

HunspellLibrary& HunspellLibrary::instance()
{
    return SingletonHolder<HunspellLibrary>::instance();
}
