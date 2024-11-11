# Házi feladat

Név/Nevek NEPTUN kóddal:
- Lukácsi Sándor ()

# Feladat kiírás
A minta feladat egy klasszikus *Aknakereső (Minesweeper)* játék elkészítése grafikus felülettel. A játék célja a mezőn lévő összes akna megtalálása, illetve az összes aknát nem rejtő mező felfedése. A játék indulásakor a pálya egyszínű négyzetekből áll, amelyek alatt aknák rejtőzködhetnek. A tábla mérete és az aknák száma nehézségi szintek alapján változik. A mezők állapota lehet lefedett, felfedett (szomszédos aknák számát mutatja), felfedett aknával, zászlós, kérdőjeles. Az egyes állapotok között az egér gombjaival válthatunk. stb, stb.
A program az alábbi funkciókkal fog rendelkezni:
* Grafikus felület Qt-ban megvalósítva.
* Nehézségi szintek választása különböző tábla méretekkel és akna számokkal.
* Az aknák véletlenszerű elhelyezése.
* A játék menetének implementálása a felsorolt mezőállapotokkal, a köztük való váltással és a vége állapotok ellenőrzésével.
* A játék idejének mérése.

# Megvalósított program
Milyen funkciókat sikerült megvalósítani? Milyen eltérések vannak a kiíráshoz képest? Hogyan kell bekonfigurálni, elindítani?

**Bemutató videó URL:**
(Szerepeljen benne minden lényeges funkció és az esetleges eltérések a kiíráshoz képest. max. 5 perc)

# Tapasztalatok
Milyen tapasztalatokat gyűjtött a feladat elkészítése során? Mi volt egyszerűbb / nehezebb a tervezetnél? Visszatekintve mit csinálna másként? (pár mondatban)

# Configuring

You need to set the `DISCORD_BOT_TOKEN` environment variable in your build environment. To do this, run:

```bash
export DISCORD_BOT_TOKEN="your_token_here"
```

> Do not share your token!

# Building

Follow these steps to clone the repository, create a build directory, configure the project with CMake, and build the project:

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/lukacsi/joci
   ```
   This command clones the `joci` repository from GitHub to your local machine.

2. **Navigate to the Project Directory:**
   ```bash
   cd joci
   ```
   Change your current directory to the cloned `joci` directory.

3. **Create and navigate to the Build Directory:**
   ```bash
   mkdir build && cd build
   ```
   It's a good practice to perform out-of-source builds. This command creates a separate `build` directory.

4. **Configure the Project with CMake:**
   ```bash
   cmake ..
   ```
   This command configures the project using CMake, generating the necessary build files in the `build` directory.

5. **Navigate to the project root:**
   ```bash
   cd ..
   ```
   Change your current directory to the `build` directory where the build files are located.

6. **Build the Project:**
   ```bash
   cmake --build /build
   ```
   This command compiles the project based on the generated build files.

Easy to copy format:
```bash
git clone https://github.com/lukacsi/joci
cd joci
mkdir build
cd build
cmake ..
cd ..
cmake --build /build
```

# Running

After successfully building the project, you can run the executable with the following commands:

```bash
cd build
./joci
```

# Additional Tips

- **Verify Environment Variable:**
  Before running the bot, you can verify that the environment variable is set correctly:

  ```bash
  echo $DISCORD_BOT_TOKEN
  ```

- **Persistent Environment Variable:**
  If you want the `DISCORD_BOT_TOKEN` to persist across sessions, consider adding the export command to your shell's configuration file (e.g., `.bashrc`, `.zshrc`):

  ```bash
  echo 'export DISCORD_BOT_TOKEN="your_token_here"' >> ~/.bashrc
  source ~/.bashrc
  ```
