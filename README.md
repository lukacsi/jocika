# Házi feladat

Név/Nevek NEPTUN kóddal:
- Lukácsi Sándor (DYT9Q1)
- Kutasi Rajmund (BWCJGS)

# Feladat kiírás
Egy discord alkalmazás, mely az alábbi funkciókra képes:
* slashcommandokra való reagálás
* szerverre kitejedő chat szűrő
* audio bejátszás fájlokból és kontroll
* audiofájlok keverése
* üzenet logolás
* adatok tárolása userenként
* beágyazott adatblokkok küldése
* időjátás lekérdezés
* üzenet ütemezés és emlékeztetők
* admninisztarív parancsok
* prancsok futtatása shell-ben
* dockerben való futtatás

# Megvalósított program
## Megvalósított funkciók:
### Zene:
- Játszás mp3 fileból melyek automatikusan importálódnak ./media/ mappából.
- Játszás Youtuberól, yt-dlp és ffmpeg összepipeolásával.
- Játszás könyvtárból, kulcsszóra illeszkedően csoportosan vagy egyesével.
- Youtube lejátszási listák bejátszása.
- Korhatáros youtube videók bejátszása sütik exportálásával cookies.txt-be.
- Könyvtár listázása és kulcsszavas keresése.
- Többszálasított lejátszási sorok, illetve zene bejátszás, minden szerverre külön ahhol a bot megtalálható.
- Lejátszási sor lekérdezése és aktuális szám eltelt idejének illetve teljes idejének kiírása.
- Lejátszási sorban álló számok mozgatása.
- Lejátszási sor keverése.
- Lejátszási sor törlése.
- Ismétlés állítása lejátszási sorra illetve éppen játszott zenére.
- Zene átugrása lejátszási sorban.
- Audio visszajátszás megállítása és indítása.
- Audio leállítása és lejátszási sor törlése.
- Csatornába való belépés és kilépés.

Minden bejátszott zene bekerül a könyvtárba a metaadataival együtt, illetve hozzáadódik a szerver lejátszási sorába, ha az éppen nem üres.

### Hang Felvétele
- Lehetséges elindítani egy szobára hogy a benne lévő felhasználók hangjának a felvéte elinduljon.
- Ha a hangfelvételt leállítjuk akkor ffmpeg segítségével mp3 formátumban a bot csatolja a felvett hangfájlt.

### Időjárás
- OpenWeather api és curl segítségével lekérhető az mostani időjárás minden városra.

### Teljesítmények
- Létrehozhatók teljesítmények melyeket felhasználókhoz lehet csatolni.
- A teljesítmények perszisztálnak indítások között SQLite3 adatbázisban való tárolással.
- Felhasználók le tudják kérni a teljesítményeiket
- Felhasználók le tudják kérni az összes teljesítményt.
- Felhasználók megnézhetik a teljesítményeket és a metaadataikat, illetve hogy mennyire ritkák.

Ez a rendszer még kissé WIP mivel minden felhasználó adhat hozzá új teljesítményt illetve az összes teljesítményt bárkihez hozzárendelhetik.

**Bemutató videó URL:**
(Szerepeljen benne minden lényeges funkció és az esetleges eltérések a kiíráshoz képest. max. 5 perc)

# Tapasztalatok

Fejlesztés során sokat tanultunk a discord apijáról, illetve hogy azzal hogyan lehet kommunikálni és elmélyűltünk a többszálas programok világában, mivel minden discord szerverre külön kellett kezelni az egyes adatok tárolását. A zene bejátszás és lejátszási sor többszálas megvalósítása miatt ez a része a megoldásnak sokkal több igényt és odafigyelést vett igénybe a tervezettekhez képest, de szerintünk elég jól meg tudtuk szépíteni és egy használható rendszert sikerült létrehoznunk hozzá. A youtuberól való bejátszás is egy nagy kihívást okozott mivel a yt-dlp process ffmpegbe való pipeolása mindig egy extra threaden kell hogy fusson hogy a letöltés ne akassza meg a főszálat, itt megismerkedtünk a future/promise-okkal is cpp nyelven. A megvalósított funkciók közt és a feladatkiírásban lettek eltérések, több funkció került megvalósításra mely nem szerepelt eredetileg a feladatkiírásban, viszont néhány el is maradt. Ennek az az oka hogy fejlesztés közben használtuk az applikációt és barátokkal konzultáltunk illetve éreztük hogy milyen funkciókba lenne érdemes több időt ölni, illetve hogy mitől lenne használhatóbb az aplikáció és abba raktunk több energiát, hogy döccenőmentesen tudjanak működni. A jövőben is szeretnék hobbi szinten ezzel foglalkozni, mivel új funkciókat egyre gyorsabban tudunk implementálni az alap könyvtárban való elmélyüléstől. Ami még biztosan kell a programba az a felhasználói jogosultságok kezelése melyekkel hamarabb kellett volna foglalkozni nem csak a Teljesítmények rendszernél, illetve még a dockerfilet is meg kell írni használhatóra, hogy könnyebb legyen a program futtatása.
# Dependencies

- [DPP](https://github.com/brainboxdotcc/DPP)
- [MPG123](https://www.mpg123.de/)
- yt-dlp
- curl
- nlohmann/json
- SQLite3


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
   git clone https://github.com/lukacsi/jocika
   ```
   This command clones the `jocika` repository from GitHub to your local machine.

2. **Navigate to the Project Directory:**
   ```bash
   cd jocika
   ```
   Change your current directory to the cloned `jocika` directory.

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
   cmake --build ./build
   ```
   This command compiles the project based on the generated build files.

Easy to copy format:
```bash
git clone https://github.com/lukacsi/jocika
cd joci
mkdir build
cd build
cmake ..
cd ..
cmake --build ./build
```

# Running

After successfully building the project, you can run the executable with the following commands:

```bash
cd build
./jocika
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
