# RTPlot
C/C++ real time plotting and serial communication application to read, plot and modify variables sent to/from a microcontroller. Based around ocornut's [Dear ImGUI](https://github.com/ocornut/imgui) and epezent's [ImPlot](https://github.com/epezent/implot). Using nlohmann's [JSON](https://github.com/nlohmann/json) and nothings' [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) library.

# Installation
## Windows
Download the latest installation file from the Releases page and run the installer.

## Arch Linux
1. Install the following packages:
```bash
sudo pacman -S cmake libserialport glfw
```

2. Add your user to the `uucp` group:
```bash
sudo usermod -a -G uucp $USER
```

4. Clone the repository:
```bash
git clone https://github.com/AdriTeixeHax/RTPlot.git
```

5. Build the project:
```bash
cd RTPlot
make
```

3. Change the default username in the `.desktop` file to yours. Then, give it access and copy the file to your `~/.local/share/applications` folder:
```bash
chmod 777 rtplot.desktop
sudo cp rtplot.desktop ~/.local/share/applications
```