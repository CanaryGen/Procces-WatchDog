# Procces WatchDog
WatchDog for your process with a function to prevent the deletion of a protected file
***
# Editable Values
The changeable values are located from line 12 to 15, here are comments for each of them:
```cpp
const std::string PROCESS_PATH = "C:\\Windows\\SecurityManager.exe"; // Path to process for run
const std::string PROCESS_NAME = "SecurityManager.exe"; // Protected proccess
const std::string DOWNLOAD_URL = "https://filehost.example/000x000x000/file.exe"; // Link for download (Direct only!)
const int CHECK_INTERVAL = 5; // Check interval (sec)
```
