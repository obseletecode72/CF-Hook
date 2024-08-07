# CF-Hook - Create File Hook

```plaintext
  /$$$$$$  /$$$$$$$$      /$$   /$$                     /$$      
 /$$__  $$| $$_____/     | $$  | $$                    | $$      
| $$  \__/| $$           | $$  | $$  /$$$$$$   /$$$$$$ | $$   /$$
| $$      | $$$$$ /$$$$$$| $$$$$$$$ /$$__  $$ /$$__  $$| $$  /$$/
| $$      | $$__/|______/| $$__  $$| $$  \ $$| $$  \ $$| $$$$$$/ 
| $$    $$| $$           | $$  | $$| $$  | $$| $$  | $$| $$_  $$ 
|  $$$$$$/| $$           | $$  | $$|  $$$$$$/|  $$$$$$/| $$ \  $$
 \______/ |__/           |__/  |__/ \______/  \______/ |__/  \__/ 
```

**EXE / JS**

![Language](https://img.shields.io/badge/language-C%2B%2B-brightgreen)
![Platform](https://img.shields.io/badge/platform-Windows-blue)

## 📖 Project Overview :

CF-Hook uses C++ lonely and a excelent library called "MinHook" and it hooks a NT function called "NtCreateFile", this project had the main focus on the service "SysMain" or "SuperFetch", after injecting the DLL of the Hook it will hook the "NtCreateFile" and verifying if the Prefetch was going to create a file ".pf" to a file, that us can modify in the code of the hook, then we return STATUS_UNSUCESSFUL if it was going to create the ."pf" file, if you want remove the hook before you can run the hookRemover, that will write the bool variable to true, unloading the dll, ok but why are you writing the variable instead of making like a verification inside of the thread in dll like GetAsyncKeyState? because for some reason, this service shits on every function, and you cant verify like this, so the easiest way that i found was writing the variable xd (memory on top).

You can easily modify it since its 100% open-source.

This project works in EXE (Injector) and DLL (Hook-Inject)

## 📚 Used Libraries :

- Windows API
- MinHook

## OBS:
- I am not responsible for anything, this project was made for educational purposes and for fun.

## OBS2:
- You must download and compile the library ![MinHook](https://github.com/TsudaKageyu/minhook)
