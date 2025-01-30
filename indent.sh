#!/usr/bin/sh
find .. -type f \( -name "*.c" -o -name "*.h" \) -exec indent -kr -i8 -ts8 -sob -l80 -ss -bs -psl -bap {} \;
find .. -type f \( -name "*~" \) -exec rm {} \;