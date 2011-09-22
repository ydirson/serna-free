You can switch between use of aspell and hunspell by setting Serna 
configuration property serna-config/speller/use to ether aspell or hunspell.
The latter is default; aspell support will be eventually removed.

hunspell dictionaries should be placed to 'hunspell' folder here. Additional
dictionaries(e.g. domain-specific dictionaries) should be put to the 
hunspell/extra folder (only .dic files, affix file from main 
dictionary is used).

It is possible to add new folders to hunspell dictionary search path by adding
<dict-dir> properties to the serna-config/speller/hunspell.
