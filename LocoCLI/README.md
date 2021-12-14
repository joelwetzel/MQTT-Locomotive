## To build for MacOS

1. Open a terminal to this directory.
2. Run 'dotnet publish -c Debug --runtime osx-x64'.  This will create an executable in the ./bin/Debug/netcoreapp3.1/osx-x64 directory
3. For convenience, I have made a symlink: 'loco -> /Users/jwetzel/Documents/PlatformIO/Projects/Locomotive/LocoCLI/bin/Debug/netcoreapp3.1/osx-x64/LocoCLI'