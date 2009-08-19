//
// Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
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
//
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#include "AspellLibrary.h"
#include "SpellChecker.h"
#include "SpellCheckerImpl.h"

#include "utils/SernaMessages.h"
#include "utils/utils_defs.h"
#include "utils/Config.h"

#include "common/String.h"
#include "common/PathName.h"
#include "common/Singleton.h"
#include "common/ScopeGuard.h"
#include "common/DynamicLibrary.h"
#include "common/PropertyTree.h"

#include "speller_debug.h"

#if defined(__APPLE__)
# include <sys/types.h>
# include <sys/sysctl.h>
# include <mach/machine.h>
#endif

#if !defined(_WIN32)
# include <unistd.h>
#endif

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

#if defined(SERNA_SYSPKG)
# ifdef FUN
#  undef FUN
# endif
# define FUN(x) x
#endif

// START_IGNORE_LITERALS

using namespace Common;
using namespace std;

static const nstring null;
static const ustring& Null(String::null());

AspellLibrary::AspellLibrary() {}
AspellLibrary::~AspellLibrary() {}

namespace cfg {

static const char SPELL_DICTDIR_VAR[]  = "data/dict-dir";
static const char SPELL_DATADIR_VAR[]  = "data/data-dir";

static const char SPELL_PWSDIR_VAR[]   = "pws-dir";
static const char SPELL_DICT_VAR[]     = "default-dict";
static const char SPELL_DLLNAME_VAR[]  = "data/lib";

static const char SPELL_DICTDIR[]      = "dict";
static const char SPELL_DATADIR[]      = "data";
static const char SPELL_DICT[]         = "en";
static const char SPELL_PWSDIR[]       = "";

static const char SPELL_DICT_FLAG[]    = ".aspell_dictdir";

#if defined(_WIN32)
# if defined(_DEBUG) && !defined(NDEBUG)
#  define DLL_BASE "aspell-15d"
# else
#  define DLL_BASE "aspell-15"
# endif
# define DLL_SFX ".dll"
#elif defined(__APPLE__)
# define DLL_BASE "libaspell"
# define DLL_SFX ".dylib"
#else
# define DLL_BASE "libaspell"
# define DLL_SFX ".so"
#endif

}

static String get_cfg_value(const char* var, const PropertyNode* cfgNode = 0)
{
    const PropertyNode* spPtn = cfgNode ? cfgNode :
                                          config().getProperty("speller");
    if (spPtn) {
        if (const PropertyNode* varPtn = spPtn->getProperty(var))
            return varPtn->getString();
    }
    return String::null();
}

static const ustring& get_error(const ustring& s)
{
    return (0 < s.size()) ? s : Null;
}

static inline void reset_error(ustring& s)
{
    if (0 < s.size())
        s.resize(0);
}

struct DictInfo {
    DictInfo(const char* c, const char* j) : code_(c), jargon_(j) {}
    nstring code_;
    nstring jargon_;
    nstring encoding_;
};

typedef std::map<nstring, DictInfo> DictInfoMap;

static void fill_encoding(DictInfoMap& dmap, DictInfoMap::iterator it,
                          const ustring& datapath)
{
    DictInfo& di = it->second;
    const unsigned LCODE_LEN = 2U;
    if (di.code_.size() < LCODE_LEN)
        return;
    const char* plc = di.code_.data();
    nstring basename(plc, plc + LCODE_LEN);
    DictInfoMap::iterator base_lang_it = dmap.end();
    if (LCODE_LEN < di.code_.size()) {
        base_lang_it = dmap.find(basename);
        if (dmap.end() != base_lang_it) {
            nstring& base_enc = base_lang_it->second.encoding_;
            if (!base_enc.empty()) {
                di.encoding_ = base_enc;
                return;
            }
        }
    }
    basename.append(".dat");
    PathName lang_dat(datapath);
    lang_dat.append(ustring(basename.begin(), basename.end()));
    if (!lang_dat.exists())
        return;
    nstring fname(local_8bit(lang_dat.name()));
    ifstream ifs(fname.c_str());
    if (!ifs)
        return;
    stringstream ss;
    ss << ifs.rdbuf();
    string d(ss.str());
    static const char chset[] = "\ncharset";
    string::size_type cpos(d.find(chset));
    if (d.npos == cpos)
        return;
    string::iterator begin(d.begin() + cpos + sizeof chset - 1), end(d.end());
    IsSpace<char> ws_pr;
    Range<string::iterator> cs_r(find_first_range_not_of(begin, end, ws_pr));
    if (cs_r.empty())
        return;
    di.encoding_.assign(cs_r.begin(), cs_r.end());
    if (dmap.end() != base_lang_it)
        base_lang_it->second.encoding_.assign(cs_r.begin(), cs_r.end());
}

class AspellInstance : public AspellLibrary {
public:
    AspellInstance();
    ~AspellInstance() { unload(); }
    virtual bool loadLibrary(const COMMON_NS::ustring& libPath)
    {
        if (!isLoaded() || libPath != lib_path_) {
            if (isLoaded())
                unload();
            if (!libPath.empty())
                DynamicLibrary::load(libPath);
        }
        return isLoaded();
    }
    virtual void* resolveSym(const char* name) const
    {
        if (void* sym = resolve(name)) {
            reset_error(sym_error_);
            return sym;
        }
        sym_error_ = errorMsg();
        return 0;
    }
    virtual const ustring& getLibError() const throw()
    {
        return get_error(lib_error_);
    }
    virtual const ustring& getSymError() const throw()
    {
        return get_error(sym_error_);
    }
    virtual AspellConfig* getDefaultConfig() { return getConfig(); }
    virtual AspellSpeller* makeSpeller(const nstring& dict);
    virtual bool getDictList(SpellChecker::Strings::Impl& si,
                             SpellChecker::Status* ps = 0);
    virtual void setDict(const nstring& dict)
    {
        dict_ = dict;
        FUN(aspell_config_replace)(getConfig(), "lang", dict_.c_str());
    }
    virtual const nstring& getDict() const { return dict_; }
    virtual const nstring& getEncoding(const nstring& dict);
    virtual const nstring& findDict(const nstring& dict);
    virtual bool  setConfig(const PropertyNode* configNode);
    //!
private:
    void            initConfig();
    //!
    AspellConfig*   getConfig();
    //!
    nstring         dict_;
    DictInfoMap     dict_map_;
    ustring         lib_error_;
    AspellConfig*   config_;
    mutable ustring sym_error_;
    String          data_dir_;
    String          dict_dir_;
    ustring         lib_path_;

    const PropertyNode* cfgNode_;
};

// END_IGNORE_LITERALS

AspellLibrary& AspellLibrary::instance()
{
    return SingletonHolder<AspellInstance>::instance();
}

static inline void
ins_dict(DictInfoMap& m, const char* name, const char* code, const char* jargon)
{
    m.insert(DictInfoMap::value_type(name, DictInfo(code, jargon)));
}

static String& strip_path_chars(String& path)
{
    while (ends_with(path, String(1, PathName::DIR_SEP)))
        path.resize(path.size() - 1);
    return path;
}

static bool get_aspell_dir(const PropertyNode* cfgNode, const ustring& base,
                           const char* var, const char* val, String& aspellDir)
{
    aspellDir = cfgNode->getSafeProperty(var)->getString();
    PathName dir(aspellDir);
    if (dir.exists())
        return true;
    dir.assign(base).append(val);
    if (dir.exists()) {
        aspellDir = dir.name();
        return true;
    }
    return false;
}

static String aspell_dir_error(const String& pfx, const String& dictDir)
{
    return pfx + from_latin1(NOTR(" '")) + dictDir + from_latin1(NOTR("' ")) +
           String(tr("does not exist or is not readable"));
}

static String find_system_aspell_lib()
{
#if !defined(_WIN32)
    static const char* paths[] = {
        "/lib", "/usr/lib", "/usr/local/lib", 0
    };
    static const char aspellLib[] = "/libaspell.so.15";
    char libpath[64];
    for (const char** dir = &paths[0]; 0 != *dir; ++dir) {
        strcpy(libpath, *dir);
        strcat(libpath, aspellLib);
        if (0 == ::access(libpath, F_OK))
            return String::fromLatin1(libpath);
    }
#endif
    return String();
}

bool AspellInstance::setConfig(const PropertyNode* cfgNode)
{
    DBG_TRACE(DBG_DEFAULT_TAG) << NOTR("AspellInstance::setConfig(), this: ")
                               << this << std::endl;
    if (cfgNode_)
        return true;

    cfgNode_ = cfgNode;
    using namespace cfg;
    if (!cfgNode_) {
        lib_error_ = String(tr("Invalid configuration data"));
        return false;
    }

    bool isSysAspell = false;
    String path(cfgNode_->getSafeProperty(SPELL_DLLNAME_VAR)->getString());
    PathName libPath(path);
    if (!PathName::exists(path)) {
        path = cfgNode_->getSafeProperty(NOTR("resolved-path"))->getString();
        PathName aspellDll(path);
        aspellDll.append( DLL_BASE DLL_SFX );
        if (!aspellDll.exists()) {
            path = find_system_aspell_lib();
            isSysAspell = true;
        }
        else
            path = aspellDll.name();
    }

    if (path.empty()) {
        lib_error_ = String(tr("Cannot find aspell shared library"));
        return false;
    }
    else if (!loadLibrary(path)) {
        lib_error_ = String(tr("Cannot load"));
        str_append(lib_error_, NOTR(" '")).append(fileName());
        str_append(lib_error_, NOTR("': ")).append(errorMsg());
        ustring::iterator it(remove_if(lib_error_.begin(),
                                       lib_error_.end(),
                                       bind2nd(less<Char>(), ' ')));
        lib_error_.erase(it, lib_error_.end());
        return false;
    }
    path = PathName(fileName()).dirname().name();
    reset_error(lib_error_);

    if (!isSysAspell) {
        String dictDir;
        if (!get_aspell_dir(cfgNode_, path, SPELL_DICTDIR_VAR, SPELL_DICTDIR,
                            dictDir)) {
            lib_error_ = aspell_dir_error(String(tr("Dictionary directory")),
                                          dictDir);
            return false;
        }
    #if defined(__APPLE__)
        PathName dictFlag(dictDir);
        dictFlag.append(cfg::SPELL_DICT_FLAG);
        DDBG << "DictFlag: " << dictFlag.name() << std::endl;
        if (!dictFlag.exists()) {
            int byteorder = 0;
            size_t len = sizeof(byteorder);
            if (0 == sysctlbyname(NOTR("hw.byteorder"),
                                  &byteorder, &len, 0, 0)) {
                if (1234 == byteorder) {
                    dictDir.append(1, PathName::DIR_SEP).append(NOTR("i386"));
                }
                else if (4321 == byteorder) {
                    dictDir.append(1, PathName::DIR_SEP).append(NOTR("ppc"));
                }
            }
            DDBG << "New dictDir: " << dictDir << std::endl;
            dictFlag = dictDir;
            dictFlag.append(cfg::SPELL_DICT_FLAG);
            DDBG << "New DictFlag: " << dictFlag.name() << std::endl;
        }
        if (!dictFlag.exists()) {
            lib_error_ = aspell_dir_error(String(tr("Dictionary directory")),
                                          dictDir);
            return false;
        }
    #endif

        dict_dir_ = strip_path_chars(dictDir);

        String dataDir;
        if (!get_aspell_dir(cfgNode_, path, SPELL_DATADIR_VAR, SPELL_DATADIR,
                            dataDir)) {
            lib_error_ = aspell_dir_error(String(tr("Data directory")), dataDir);
            return false;
        }
        data_dir_ = strip_path_chars(dataDir);
    }
    return true;
}

AspellConfig* AspellInstance::getConfig()
{
    initConfig();
    return config_;
}

static inline void report_error(SernaMessages::Messages m)
{
    SpellErrorInfoPtr ip(new SpellChecker::Error::Info);
    *ip << m << Message::L_ERROR;
    throw AspellErr(ip.get());
}

#if defined(__APPLE__)
static void set_cfg_value(const nstring& var, const nstring& val)
{
    PropertyNode* spPtn = config().root()->makeDescendant("speller");
    if (spPtn) {
        String varName(from_string<String>(var));
        if (PropertyNode* varPtn = spPtn->makeDescendant(varName))
            varPtn->setString(from_string<String>(val));
    }
}
#endif

void AspellInstance::initConfig()
{
    DBG_TRACE(DBG_DEFAULT_TAG) << NOTR("AspellInstance::initConfig(), this: ")
                               << this << std::endl;

    if (0 == config_) {
        config_ = FUN(new_aspell_config)();
        FUN(aspell_config_replace)(config_, "save-repl", "false");

        if (!data_dir_.empty()) {
            nstring tmp(local_8bit(data_dir_));
            FUN(aspell_config_replace)(config_, "data-dir", tmp.c_str());
        }

        if (!dict_dir_.empty()) {
            nstring tmp(local_8bit(dict_dir_));
            FUN(aspell_config_replace)(config_, "dict-dir", tmp.c_str());
        }

        dict_.clear();
        AspellDictInfoList* dlst = FUN(get_aspell_dict_info_list)(config_);
        if (0 == FUN(aspell_dict_info_list_empty)(dlst)) {
            AspellDictInfoEnumeration* ade;
            ade = FUN(aspell_dict_info_list_elements)(dlst);
            ON_BLOCK_EXIT(FUN(delete_aspell_dict_info_enumeration), ade);
            if (0 != ade) {
                const AspellDictInfo* adi;
                do
                    if ((adi = FUN(aspell_dict_info_enumeration_next)(ade))) {
                        DDBG << "Dict added: " << sqt(adi->name) << std::endl;
                        ins_dict(dict_map_, adi->name, adi->code, adi->jargon);
                    }
                while (0 == FUN(aspell_dict_info_enumeration_at_end)(ade));
            }
        }
    }

    nstring tmp(latin1(get_cfg_value(cfg::SPELL_DICT_VAR)));
    if (dict_.empty() || dict_ != tmp) {
        if (0 != dict_map_.count(tmp))
            dict_ = tmp;
        else if (!dict_map_.empty())
            dict_ = dict_map_.begin()->first;
        DDBG << "Default dict: " << sqt(dict_) << std::endl;
        FUN(aspell_config_replace)(config_, "lang", dict_.c_str());
    }
}

AspellInstance::AspellInstance()
 :  config_(0), cfgNode_(0)
{
}

/**
 * Given \a dict string tries to find corresponding aspell dictionary
 *
 * @param dict   dictionary string
 *
 * @return const reference to the installed aspell dictionary string
 * or null otherwise
 */
const nstring& AspellInstance::findDict(const nstring& dict)
{
    initConfig();

    if (0 < dict_map_.count(dict))
        return dict;

    if (dict.npos != dict.find('-')) {
        nstring tdict;
        tdict.reserve(dict.size());
        replace_copy(dict.begin(), dict.end(), back_inserter(tdict), '-', '_');
        DictInfoMap::iterator it = dict_map_.find(tdict);
        if (dict_map_.end() != it)
            return it->first;
    }
    return dict_;
}

bool AspellInstance::getDictList(SpellChecker::Strings::Impl& strings,
                                 SpellChecker::Status* ps)
{
    initConfig();

    if (!dict_map_.empty()) {
        DictInfoMap::const_iterator it = dict_map_.begin();
        for (; it != dict_map_.end(); ++it)
            strings.addString(from_latin1(it->first.c_str(), it->first.size()));
        return true;
    }
    if (0 != ps) {
        SpellErrorInfoPtr ip(new SpellChecker::Error::Info);
        *ip << SernaMessages::spellCheckerNoDictionaries << Message::L_ERROR;
        ps->reset(ip.get());
    }
    return false;
}

static inline void report_error(AspellCanHaveError* pche)
{
    const char* msg = FUN(aspell_error_message)(pche);
    AspellErr err(msg);
    throw err;
}

template<typename T> void report_error(SernaMessages::Messages m, const T& arg)
{
    SpellErrorInfoPtr ip(new SpellChecker::Error::Info);
    *ip << m << Message::L_ERROR << arg;
    throw AspellErr(ip.get());
}

AspellSpeller* AspellInstance::makeSpeller(const nstring& dict)
{
    DDBG << "makeSpeller: dict=" << sqt(dict) << std::endl;
    initConfig();

    if (dict_.empty() || dict_map_.empty())
        report_error(SernaMessages::spellCheckerNoDictionaries);

    const nstring& cur_dict = dict.empty() ? dict_ : dict;

    DDBG << "makeSpeller: cur_dict=" << sqt(cur_dict) << std::endl;

    if (0 == dict_map_.count(cur_dict))
        report_error(SernaMessages::spellCheckerInvalidDict,
                     from_latin1(cur_dict.data(), cur_dict.size()));

    if (getEncoding(cur_dict).empty())
        report_error(SernaMessages::spellCheckerEncodingError,
                     from_latin1(cur_dict.data(), cur_dict.size()));

    AspellConfig* acp = FUN(aspell_config_clone)(getConfig());
    FUN(aspell_config_replace)(acp, "lang", cur_dict.c_str());

    nstring base(local_8bit(get_cfg_value(cfg::SPELL_PWSDIR_VAR)));
    base.append(1, PathName::DIR_SEP).append("serna.").append(cur_dict, 0, 2);

    nstring tmp(base + ".pws");
    FUN(aspell_config_replace)(acp, "personal-path", tmp.c_str());
    FUN(aspell_config_replace)(acp, "personal", tmp.c_str());
    tmp = base + ".prepl";
    FUN(aspell_config_replace)(acp, "repl-path", tmp.c_str());
    FUN(aspell_config_replace)(acp, "repl", tmp.c_str());

    AspellCanHaveError* pche = FUN(new_aspell_speller)(acp);
    ScopeGuard pche_g(makeGuard(FUN(delete_aspell_can_have_error), pche));
    if (int aspellErrorNum = FUN(aspell_error_number)(pche)) {
        DDBG << "Aspell error number = " << aspellErrorNum << std::endl;
        DDBG << "Aspell error message = " << FUN(aspell_error_message)(pche)
             << std::endl;
        report_error(pche);
    }
    else {
        pche_g.dismiss();
        return FUN(to_aspell_speller)(pche);
    }
    return 0;
}

const nstring& AspellInstance::getEncoding(const nstring& dict)
{
    initConfig();

    DictInfoMap::iterator it = dict_map_.find(dict.empty() ? dict_ : dict);
    if (dict_map_.end() == it)
        return null;
    nstring& encoding = it->second.encoding_;
    if (encoding.empty()) {
        fill_encoding(dict_map_, it, data_dir_);
    }
    return encoding;
}
//!
void* AspellResolver::resolve(const char* name)
{
    AspellLibrary& al = AspellLibrary::instance();
    if (void* p = al.resolveSym(name))
        return p;
    throw AspellErr(al.getSymError());
    return 0; // prevent possible compiler warning
}

