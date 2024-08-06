as generated.asm -o generated.o
ld -macos_version_min 14.4.1 generated.o -o generated -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _main -arch arm64
