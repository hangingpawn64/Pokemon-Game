# 🌟 Pokémon Starters Battle

A retro-style, 2D Pokémon-inspired adventure built in **C++** using **SFML**. Explore the world, walk into tall grass, and battle your favorite starter Pokémon — Squirtle, Charmander, Bulbasaur, and Pikachu!

---

## 🎮 Features

* Pixel-art overworld exploration
* Turn-based battle mechanics
* Classic starter Pokémon
* Lightweight and portable

---

## ✨ Quick Start (Run Game as App)

### 1. Download the AppImage

Go to the [**Releases Page**](https://github.com/hangingpawn64/Pokemon-Game/releases/latest) and download the latest `Overworld-x86_64.AppImage`.

### 2. Make it executable

```bash
chmod +x Overworld-x86_64.AppImage
```

### 3. Run the game

```bash
./Overworld-x86_64.AppImage
```

> ✅ No installation or compilation required!

---

## 🐧 FUSE Error? Do This:

If you get this error:

```
dlopen(): error loading libfuse.so.2
AppImages require FUSE to run.
```

Fix it by installing FUSE:

```bash
sudo apt install libfuse2
```

If you still can’t run the AppImage, you can extract and run it manually:

```bash
./Overworld-x86_64.AppImage --appimage-extract
cd squashfs-root
./AppRun
```

---

## 🛠️ Build from Source (Developers)

### Requirements

* C++17-compatible compiler
* SFML development libraries

### Instructions

```bash
sudo apt install libsfml-dev
make overworld
./overworld
```

---

## 📁 Project Structure

```
Pokemon/
├── assets/               # Game sprites and images
├── pokemons/             # Individual Pokémon logic
├── AppDir/               # AppImage build files
├── overworld             # Game binary
├── Overworld-x86_64.AppImage
├── Makefile
└── README.md
```

---

## 👤 Author

Created by **@hangingpawn64**
GitHub: [github.com/hangingpawn64](https://github.com/hangingpawn64)

---

## 📜 Disclaimer

This is a fan-made project inspired by Pokémon. All Pokémon names, sprites, and concepts are property of Nintendo and Game Freak.

---

## 🪪 License

Code is licensed under the **MIT License**.
See [LICENSE](LICENSE) for details.
