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
//
#ifndef SPELLER_HUNSPELL_H__
#define SPELLER_HUNSPELL_H__

#include "FunTraits.h"
#include "DynFunctor.h"

/* Definition of Hunspell shared library C API */

struct Hunhandle;

Hunhandle* Hunspell_create(const char* aff, const char* dic);
void       Hunspell_destroy(Hunhandle*);
int        Hunspell_spell(Hunhandle*, const char*);
char*      Hunspell_get_dic_encoding(Hunhandle *);
int        Hunspell_suggest(Hunhandle*, char*** slst, const char* word);
int        Hunspell_add(Hunhandle*, const char* word);
int        Hunspell_remove(Hunhandle*, const char* word);
void       Hunspell_free_list(Hunhandle*, char*** slst, int n);

#define DECLARE_SYMS \
    SYM(create);    SYM(destroy);   SYM(spell);     SYM(get_dic_encoding); \
    SYM(suggest);   SYM(add);       SYM(remove);    SYM(free_list);

REGISTER_TYPEOF(Hunhandle* (const char*, const char*), 1);
REGISTER_TYPEOF(void (Hunhandle*), 2);
REGISTER_TYPEOF(int (Hunhandle*, const char*), 3);
REGISTER_TYPEOF(char* (Hunhandle*), 4);
REGISTER_TYPEOF(int (Hunhandle*, char***, const char*), 5);
REGISTER_TYPEOF(void (Hunhandle*, char***, int), 6);

#define SYM(x) extern const char nm_Hunspell_##x[];
DECLARE_SYMS
#undef SYM

#endif // SPELLER_HUNSPELL_H__
