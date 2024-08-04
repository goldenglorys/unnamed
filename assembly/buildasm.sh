# Assemble the HelloWorld.asm file into an object file (HelloWorld.o)
# The 'as' command is used to assemble the assembly code into machine code
as assembly/HelloWorld.asm -o HelloWorld.o

# Link the object file into an executable file (HelloWorld)
# The 'ld' command is used to link the object file with the necessary libraries
# The options used here are specific to macOS and arm64 architecture
ld -macos_version_min 14.4.1 -o HelloWorld HelloWorld.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64


# Specify the minimum macOS version required to run the executable (-macos_version_min 14.4.1)

# Specify the output file name (-o HelloWorld)

# Specify the object file to link (HelloWorld.o)

# Link against the System library, which provides basic system functions (-lSystem)

# Specify the SDK path for the macOS system libraries (-syslibroot `xcrun -sdk macosx --show-sdk-path`)

# Specify the entry point of the program (_start) (-e _start)

# Specify the architecture (arm64) (-arch arm64)