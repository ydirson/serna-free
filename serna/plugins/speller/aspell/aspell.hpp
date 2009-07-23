#ifndef ASPELL_HPP_
#define ASPELL_HPP_

// This file has been generated automatically

#include "FunTraits.h"
#include "DynFunctor.h"
#include "aspell.h"

#define SYM(x) extern const char nm_##x[];

SYM(aspell_mutable_container_add);
REGISTER_TYPEOF(int (AspellMutableContainer *, const char *), 1);

SYM(aspell_mutable_container_remove);
SYM(aspell_mutable_container_clear);
REGISTER_TYPEOF(void (AspellMutableContainer *), 2);

SYM(aspell_mutable_container_to_mutable_container);
REGISTER_TYPEOF(AspellMutableContainer * (AspellMutableContainer *), 3);

SYM(aspell_key_info_enumeration_at_end);
REGISTER_TYPEOF(int (const AspellKeyInfoEnumeration *), 4);

SYM(aspell_key_info_enumeration_next);
REGISTER_TYPEOF(const AspellKeyInfo * (AspellKeyInfoEnumeration *), 5);

SYM(delete_aspell_key_info_enumeration);
REGISTER_TYPEOF(void (AspellKeyInfoEnumeration *), 6);

SYM(aspell_key_info_enumeration_clone);
REGISTER_TYPEOF(AspellKeyInfoEnumeration * (const AspellKeyInfoEnumeration *), 7);

SYM(aspell_key_info_enumeration_assign);
REGISTER_TYPEOF(void (AspellKeyInfoEnumeration *, const AspellKeyInfoEnumeration *), 8);

SYM(new_aspell_config);
REGISTER_TYPEOF(AspellConfig * (), 9);

SYM(delete_aspell_config);
REGISTER_TYPEOF(void (AspellConfig *), 10);

SYM(aspell_config_clone);
REGISTER_TYPEOF(AspellConfig * (const AspellConfig *), 11);

SYM(aspell_config_assign);
REGISTER_TYPEOF(void (AspellConfig *, const AspellConfig *), 12);

SYM(aspell_config_error_number);
REGISTER_TYPEOF(unsigned int (const AspellConfig *), 13);

SYM(aspell_config_error_message);
REGISTER_TYPEOF(const char * (const AspellConfig *), 14);

SYM(aspell_config_error);
REGISTER_TYPEOF(const AspellError * (const AspellConfig *), 15);

SYM(aspell_config_set_extra);
REGISTER_TYPEOF(void (AspellConfig *, const AspellKeyInfo *, const AspellKeyInfo *), 16);

SYM(aspell_config_keyinfo);
REGISTER_TYPEOF(const AspellKeyInfo * (AspellConfig *, const char *), 17);

SYM(aspell_config_possible_elements);
REGISTER_TYPEOF(AspellKeyInfoEnumeration * (AspellConfig *, int), 18);

SYM(aspell_config_get_default);
REGISTER_TYPEOF(const char * (AspellConfig *, const char *), 19);

SYM(aspell_config_elements);
REGISTER_TYPEOF(AspellStringPairEnumeration * (AspellConfig *), 20);

SYM(aspell_config_replace);
REGISTER_TYPEOF(int (AspellConfig *, const char *, const char *), 21);

SYM(aspell_config_remove);
REGISTER_TYPEOF(int (AspellConfig *, const char *), 22);

SYM(aspell_config_have);
REGISTER_TYPEOF(int (const AspellConfig *, const char *), 23);

SYM(aspell_config_retrieve);
SYM(aspell_config_retrieve_list);
REGISTER_TYPEOF(int (AspellConfig *, const char *, AspellMutableContainer *), 24);

SYM(aspell_config_retrieve_bool);
SYM(aspell_config_retrieve_int);
SYM(aspell_error_is_a);
REGISTER_TYPEOF(int (const AspellError *, const AspellErrorInfo *), 25);

SYM(aspell_error_number);
REGISTER_TYPEOF(unsigned int (const AspellCanHaveError *), 26);

SYM(aspell_error_message);
REGISTER_TYPEOF(const char * (const AspellCanHaveError *), 27);

SYM(aspell_error);
REGISTER_TYPEOF(const AspellError * (const AspellCanHaveError *), 28);

SYM(delete_aspell_can_have_error);
REGISTER_TYPEOF(void (AspellCanHaveError *), 29);

SYM(new_aspell_speller);
REGISTER_TYPEOF(AspellCanHaveError * (AspellConfig *), 30);

SYM(to_aspell_speller);
REGISTER_TYPEOF(AspellSpeller * (AspellCanHaveError *), 31);

SYM(delete_aspell_speller);
REGISTER_TYPEOF(void (AspellSpeller *), 32);

SYM(aspell_speller_error_number);
REGISTER_TYPEOF(unsigned int (const AspellSpeller *), 33);

SYM(aspell_speller_error_message);
REGISTER_TYPEOF(const char * (const AspellSpeller *), 34);

SYM(aspell_speller_error);
REGISTER_TYPEOF(const AspellError * (const AspellSpeller *), 35);

SYM(aspell_speller_config);
REGISTER_TYPEOF(AspellConfig * (AspellSpeller *), 36);

SYM(aspell_speller_check);
REGISTER_TYPEOF(int (AspellSpeller *, const char *, int), 37);

SYM(aspell_speller_add_to_personal);
SYM(aspell_speller_add_to_session);
SYM(aspell_speller_personal_word_list);
REGISTER_TYPEOF(const AspellWordList * (AspellSpeller *), 38);

SYM(aspell_speller_session_word_list);
SYM(aspell_speller_main_word_list);
SYM(aspell_speller_save_all_word_lists);
REGISTER_TYPEOF(int (AspellSpeller *), 39);

SYM(aspell_speller_clear_session);
SYM(aspell_speller_suggest);
REGISTER_TYPEOF(const AspellWordList * (AspellSpeller *, const char *, int), 40);

SYM(aspell_speller_store_replacement);
REGISTER_TYPEOF(int (AspellSpeller *, const char *, int, const char *, int), 41);

SYM(delete_aspell_filter);
REGISTER_TYPEOF(void (AspellFilter *), 42);

SYM(aspell_filter_error_number);
REGISTER_TYPEOF(unsigned int (const AspellFilter *), 43);

SYM(aspell_filter_error_message);
REGISTER_TYPEOF(const char * (const AspellFilter *), 44);

SYM(aspell_filter_error);
REGISTER_TYPEOF(const AspellError * (const AspellFilter *), 45);

SYM(to_aspell_filter);
REGISTER_TYPEOF(AspellFilter * (AspellCanHaveError *), 46);

SYM(delete_aspell_document_checker);
REGISTER_TYPEOF(void (AspellDocumentChecker *), 47);

SYM(aspell_document_checker_error_number);
REGISTER_TYPEOF(unsigned int (const AspellDocumentChecker *), 48);

SYM(aspell_document_checker_error_message);
REGISTER_TYPEOF(const char * (const AspellDocumentChecker *), 49);

SYM(aspell_document_checker_error);
REGISTER_TYPEOF(const AspellError * (const AspellDocumentChecker *), 50);

SYM(new_aspell_document_checker);
REGISTER_TYPEOF(AspellCanHaveError * (AspellSpeller *), 51);

SYM(to_aspell_document_checker);
REGISTER_TYPEOF(AspellDocumentChecker * (AspellCanHaveError *), 52);

SYM(aspell_document_checker_reset);
SYM(aspell_document_checker_process);
REGISTER_TYPEOF(void (AspellDocumentChecker *, const char *, int), 53);

SYM(aspell_document_checker_next_misspelling);
REGISTER_TYPEOF(AspellToken (AspellDocumentChecker *), 54);

SYM(aspell_document_checker_filter);
REGISTER_TYPEOF(AspellFilter * (AspellDocumentChecker *), 55);

SYM(aspell_word_list_empty);
REGISTER_TYPEOF(int (const AspellWordList *), 56);

SYM(aspell_word_list_size);
REGISTER_TYPEOF(unsigned int (const AspellWordList *), 57);

SYM(aspell_word_list_elements);
REGISTER_TYPEOF(AspellStringEnumeration * (const AspellWordList *), 58);

SYM(delete_aspell_string_enumeration);
REGISTER_TYPEOF(void (AspellStringEnumeration *), 59);

SYM(aspell_string_enumeration_clone);
REGISTER_TYPEOF(AspellStringEnumeration * (const AspellStringEnumeration *), 60);

SYM(aspell_string_enumeration_assign);
REGISTER_TYPEOF(void (AspellStringEnumeration *, const AspellStringEnumeration *), 61);

SYM(aspell_string_enumeration_at_end);
REGISTER_TYPEOF(int (const AspellStringEnumeration *), 62);

SYM(aspell_string_enumeration_next);
REGISTER_TYPEOF(const char * (AspellStringEnumeration *), 63);

SYM(get_aspell_module_info_list);
REGISTER_TYPEOF(AspellModuleInfoList * (AspellConfig *), 64);

SYM(aspell_module_info_list_empty);
REGISTER_TYPEOF(int (const AspellModuleInfoList *), 65);

SYM(aspell_module_info_list_size);
REGISTER_TYPEOF(unsigned int (const AspellModuleInfoList *), 66);

SYM(aspell_module_info_list_elements);
REGISTER_TYPEOF(AspellModuleInfoEnumeration * (const AspellModuleInfoList *), 67);

SYM(get_aspell_dict_info_list);
REGISTER_TYPEOF(AspellDictInfoList * (AspellConfig *), 68);

SYM(aspell_dict_info_list_empty);
REGISTER_TYPEOF(int (const AspellDictInfoList *), 69);

SYM(aspell_dict_info_list_size);
REGISTER_TYPEOF(unsigned int (const AspellDictInfoList *), 70);

SYM(aspell_dict_info_list_elements);
REGISTER_TYPEOF(AspellDictInfoEnumeration * (const AspellDictInfoList *), 71);

SYM(aspell_module_info_enumeration_at_end);
REGISTER_TYPEOF(int (const AspellModuleInfoEnumeration *), 72);

SYM(aspell_module_info_enumeration_next);
REGISTER_TYPEOF(const AspellModuleInfo * (AspellModuleInfoEnumeration *), 73);

SYM(delete_aspell_module_info_enumeration);
REGISTER_TYPEOF(void (AspellModuleInfoEnumeration *), 74);

SYM(aspell_module_info_enumeration_clone);
REGISTER_TYPEOF(AspellModuleInfoEnumeration * (const AspellModuleInfoEnumeration *), 75);

SYM(aspell_module_info_enumeration_assign);
REGISTER_TYPEOF(void (AspellModuleInfoEnumeration *, const AspellModuleInfoEnumeration *), 76);

SYM(aspell_dict_info_enumeration_at_end);
REGISTER_TYPEOF(int (const AspellDictInfoEnumeration *), 77);

SYM(aspell_dict_info_enumeration_next);
REGISTER_TYPEOF(const AspellDictInfo * (AspellDictInfoEnumeration *), 78);

SYM(delete_aspell_dict_info_enumeration);
REGISTER_TYPEOF(void (AspellDictInfoEnumeration *), 79);

SYM(aspell_dict_info_enumeration_clone);
REGISTER_TYPEOF(AspellDictInfoEnumeration * (const AspellDictInfoEnumeration *), 80);

SYM(aspell_dict_info_enumeration_assign);
REGISTER_TYPEOF(void (AspellDictInfoEnumeration *, const AspellDictInfoEnumeration *), 81);

SYM(new_aspell_string_list);
REGISTER_TYPEOF(AspellStringList * (), 82);

SYM(aspell_string_list_empty);
REGISTER_TYPEOF(int (const AspellStringList *), 83);

SYM(aspell_string_list_size);
REGISTER_TYPEOF(unsigned int (const AspellStringList *), 84);

SYM(aspell_string_list_elements);
REGISTER_TYPEOF(AspellStringEnumeration * (const AspellStringList *), 85);

SYM(aspell_string_list_add);
REGISTER_TYPEOF(int (AspellStringList *, const char *), 86);

SYM(aspell_string_list_remove);
SYM(aspell_string_list_clear);
REGISTER_TYPEOF(void (AspellStringList *), 87);

SYM(aspell_string_list_to_mutable_container);
REGISTER_TYPEOF(AspellMutableContainer * (AspellStringList *), 88);

SYM(delete_aspell_string_list);
SYM(aspell_string_list_clone);
REGISTER_TYPEOF(AspellStringList * (const AspellStringList *), 89);

SYM(aspell_string_list_assign);
REGISTER_TYPEOF(void (AspellStringList *, const AspellStringList *), 90);

SYM(new_aspell_string_map);
REGISTER_TYPEOF(AspellStringMap * (), 91);

SYM(aspell_string_map_add);
REGISTER_TYPEOF(int (AspellStringMap *, const char *), 92);

SYM(aspell_string_map_remove);
SYM(aspell_string_map_clear);
REGISTER_TYPEOF(void (AspellStringMap *), 93);

SYM(aspell_string_map_to_mutable_container);
REGISTER_TYPEOF(AspellMutableContainer * (AspellStringMap *), 94);

SYM(delete_aspell_string_map);
SYM(aspell_string_map_clone);
REGISTER_TYPEOF(AspellStringMap * (const AspellStringMap *), 95);

SYM(aspell_string_map_assign);
REGISTER_TYPEOF(void (AspellStringMap *, const AspellStringMap *), 96);

SYM(aspell_string_map_empty);
REGISTER_TYPEOF(int (const AspellStringMap *), 97);

SYM(aspell_string_map_size);
REGISTER_TYPEOF(unsigned int (const AspellStringMap *), 98);

SYM(aspell_string_map_elements);
REGISTER_TYPEOF(AspellStringPairEnumeration * (const AspellStringMap *), 99);

SYM(aspell_string_map_insert);
REGISTER_TYPEOF(int (AspellStringMap *, const char *, const char *), 100);

SYM(aspell_string_map_replace);
SYM(aspell_string_map_lookup);
REGISTER_TYPEOF(const char * (const AspellStringMap *, const char *), 101);

SYM(aspell_string_pair_enumeration_at_end);
REGISTER_TYPEOF(int (const AspellStringPairEnumeration *), 102);

SYM(aspell_string_pair_enumeration_next);
REGISTER_TYPEOF(AspellStringPair (AspellStringPairEnumeration *), 103);

SYM(delete_aspell_string_pair_enumeration);
REGISTER_TYPEOF(void (AspellStringPairEnumeration *), 104);

SYM(aspell_string_pair_enumeration_clone);
REGISTER_TYPEOF(AspellStringPairEnumeration * (const AspellStringPairEnumeration *), 105);

SYM(aspell_string_pair_enumeration_assign);
REGISTER_TYPEOF(void (AspellStringPairEnumeration *, const AspellStringPairEnumeration *), 106);

#undef SYM

#endif // ASPELL_HPP_
