import os, sys

mod_dir = os.path.dirname(os.path.abspath(__file__))
if not sys.path.__contains__(mod_dir):
    sys.path.append(mod_dir)
