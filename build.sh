clang main.c lexerf.c parserf.c codegeneratorf.c hashmap/hashmapoperators.c -o build/main -Wall -Wextra -Wpedantic -fsanitize=address -fsanitize=undefined