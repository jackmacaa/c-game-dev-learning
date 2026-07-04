# Building the Raylib Game Dev Project

## Prerequisites

### Windows

- **CMake**: Download from [cmake.org](https://cmake.org/download/)
- **C Compiler**: Visual Studio Build Tools or MinGW-w64
  - **Visual Studio**: Install "Desktop development with C++" workload
  - **MinGW**: Download from [mingw-w64.org](https://www.mingw-w64.org/)

### macOS

```bash
brew install cmake
```

### Linux (Ubuntu/Debian)

```bash
sudo apt-get install cmake build-essential
```

---

## Build Instructions

### Windows (Visual Studio)

```bash
cd d:\local-programming\Learning\C\game-dev
cmake -S . -B build -G "Visual Studio 17 2022" -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build --config Release
.\build\bin\Release\game.exe
```

### Faster dev loop (Bash)

```bash
./scripts/dev.sh
```

Optional flags:

- `--configure` force reconfigure
- `--debug` build and run Debug
- `--build-only` compile without running
- `--run-only` run existing binary
- `--clean` delete `build/` first

### Windows (MinGW)

```bash
cd d:\local-programming\Learning\C\game-dev
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build
\.\build\bin\game.exe
```

### macOS / Linux

```bash
cd path/to/game-dev
cmake -S . -B build -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build
./build/bin/game
```

---

## Project Structure

```
game-dev/
├── src/
│   ├── core/      # Core game loop/state implementation
│   └── entities/  # Entity implementation
├── include/
│   ├── core/      # Core game/config headers
│   └── entities/  # Entity headers
├── build/         # Build output (generated)
├── assets/        # Game assets (textures, sounds, etc.)
├── CMakeLists.txt # Build configuration
└── BUILD.md       # This file
```

---

## First Run

After building, you should see a window with:

- A sprite-based player that moves with WASD or arrow keys across a larger world
- Distinct map regions and a mountain altar at world center
- 6 colored triangle gems (3 primary + 3 secondary) distributed around the world
- Corrupted guardian enemies patrolling key routes and the mountain approach
- A gem HUD with collection progress
- Health tracking and melee strike combat (`F`)
- Sprint movement (`Shift`) for traversal and kiting
- Stamina management for sprinting and a live essence counter from defeated guardians
- Terrain obstacles (cliffs/mountain) that block movement
- Minimap markers for player, remaining gems, enemies, and mountain altar
- Dynamic day/night ambience tint over the world
- Manual fusion prompt at the altar when all gems are collected and guardians are defeated (`E`)
- Restart option after victory/defeat (`R`)
- Pause functionality (`Space`) and quit (`Esc`)

---

## Next Steps

1. **Experiment**: Modify the player speed, size, or colors
2. **Add Features**: Add enemies, collision detection, or scoring mechanics
3. **Understand the Architecture**: Study how `Game` struct holds state and how entities are updated each frame

---

## Troubleshooting

### CMake not found

- Ensure CMake is installed and added to PATH
- Restart terminal after installation

### Compiler not found

- Windows: Install Visual Studio Build Tools with C++ support
- Linux: Run `sudo apt-get install build-essential`

### Raylib download fails

- Check internet connection
- Raylib downloads from GitHub during CMake configuration
- May need to disable firewall temporarily

### CMake 4.x + Raylib policy error

- If you see compatibility errors from `_deps/raylib-src/CMakeLists.txt`, configure with:
  - `-DCMAKE_POLICY_VERSION_MINIMUM=3.5`
- This repo already sets that policy in `CMakeLists.txt`, but passing it explicitly is useful for one-off manual commands.

---

## Learning Notes

This project demonstrates:

- **Entity-Component pattern**: `Entity` struct for game objects
- **State machine**: `GameState` enum for game states
- **Delta time**: Frame-independent movement with `dt`
- **Input handling**: Raylib input API
- **Memory management**: Dynamic allocation with `malloc`/`free`
