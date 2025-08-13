## Antique 2 - Simulate an old photo from the past with GEGL

High quality old photo effect inspired by a old 2022 plugin 

https://github.com/LinuxBeaver/GEGL-GIMP-PLUGIN_Old_Photo_Antique/


<img width="1042" height="830" alt="image" src="https://github.com/user-attachments/assets/020192d3-ea7e-4b79-b670-f3dcbe779c69" />

<img width="942" height="846" alt="image" src="https://github.com/user-attachments/assets/9c4c96f8-7c86-4fe2-b062-b22d940069a8" />



## Directory to put Binaries (They do NOT go in the normal plugins folder)

**Windows**
C:\Users\USERNAME\AppData\Local\gegl-0.4\plug-ins

**Linux** 
`~/.local/share/gegl-0.4/plug-ins`

**Linux (Flatpak)**
`~/.var/app/org.gimp.GIMP/data/gegl-0.4/plug-ins`


Then Restart GIMP and go to Filters>Decor and look for "Old Photo 2 Antique" or just search for it with /

GIMP 2.10 users will only find this in GEGL Operation drop down list and it will only work on 2.10 if they are using GEGL 0.4.50+, all normal windows builds of GIMP 2.10 ship with GEGL 0.4.48

## Compiling and Installing

Run ./build_plugin_(OSNAME).sh to auto compile, then you will find binaries in LinuxBinaries or WindowsBinaries folder,
 if you want to how to what the script or do it without the script read below

### Linux

To compile and install you will need the GEGL header files (`libgegl-dev` on
Debian based distributions or `gegl` on Arch Linux) and meson (`meson` on
most distributions).

```bash
meson setup --buildtype=release build
ninja -C build

```

### Windows

The easiest way to compile this project on Windows is by using msys2.  Download
and install it from here: https://www.msys2.org/

Open a msys2 terminal with `C:\msys64\mingw64.exe`.  Run the following to
install required build dependencies:

```bash
pacman --noconfirm -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-meson mingw-w64-x86_64-gegl
```

Then build the same way you would on Linux:

```bash
meson setup --buildtype=release build
ninja -C build
```
## More pics of this based plugin

<img width="605" height="856" alt="image" src="https://github.com/user-attachments/assets/e4ec0567-23c3-42bd-983a-8b6b46e34eb1" />

<img width="944" height="823" alt="image" src="https://github.com/user-attachments/assets/57faea1a-3b01-42dd-b020-e115e7a68b15" />




