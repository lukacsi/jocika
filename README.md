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

Most of the configarable parameters are located in globals.h, and in the main function.

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

3. **Create the Build Directory:**
   ```bash
   mkdir build
   ```
   It's a good practice to perform out-of-source builds. This command creates a separate `build` directory.

4. **Configure the Project with CMake:**
   ```bash
   cmake -B build
   ```
   This command configures the project using CMake, generating the necessary build files in the `build` directory.

5. **Build the Project:**
   ```bash
   cmake --build build
   ```
   This command compiles the project based on the generated build files.

Easy to copy format:
```bash
git clone https://github.com/lukacsi/jocika
cd joci
mkdir build
cmake -B build
cmake --build build
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

