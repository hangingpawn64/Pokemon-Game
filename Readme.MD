# 🎮 Pokémon Starters Battle

A 2D retro-style Pokémon-inspired game i built in **C++ with SFML** to work on OOP. Choose your starter Pokémon, explore the world, and battle the first three starters—plus Pikachu!

---

## 📦 Releases

To get started **quickly** without compiling code:

### ✅ Linux Users (Ubuntu or similar):

1. **Download the latest release** from the [Releases Page](https://github.com/hangingpawn64/Pokemon-Game/releases)
2. **Make it executable:**

   ```bash
   chmod +x Overworld-x86_64.AppImage
   ```
3. **Run the game:**

   ```bash
   ./Overworld-x86_64.AppImage
   ```

> ℹ️ If you see a `libfuse.so.2` error, run:
>
> ```bash
> sudo apt install libfuse2
> ```

---

### ✅ Windows Users (in future releases):

> ⚠️ A native Windows `.exe` is not yet available. For now, you can:
>
> * Use **WSL (Windows Subsystem for Linux)** to build and run
> * Or wait for a Windows build (coming soon)

---

## 🛠️ Build from Source

You can compile the game manually on Linux or WSL:

### 🔧 Requirements

* A **C++17-compatible compiler** (e.g. `g++`)
* **SFML library**

### 📅 Clone and Build

```bash
sudo apt update
sudo apt install build-essential libsfml-dev
git clone https://github.com/hangingpawn64/Pokemon-Game
cd Pokemon-Game
make overworld
./overworld
```

---

## 🧑‍💻 Development Notes

* Game is built with `make` and SFML.
* Assets and sprites are stored locally and loaded at runtime.

### To Clean Build Files:

```bash
make clean
```

---

## 📁 Repository Structure

```
Pokemon-Game/
├── AppDir/                 # Structure used for AppImage packaging
├── assets/                # Game images, icons, audio
├── src/                   # Source code (.cpp, .hpp)
├── Makefile               # Build instructions
├── overworld              # Compiled binary (after make)
└── Overworld-x86_64.AppImage # Linux release
```

---

## 🙌 Contributing

Pull requests and issues are welcome!


