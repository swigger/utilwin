
## install command

Replace `<UTILWIN_PATH>` with the actual path to your utilwin repository.

```bat
rem optional remove: vcpkg remove utilwin:x64-windows-static & rd /s /q %localappdata%\vcpkg\archives
vcpkg install utilwin:x64-windows-static --overlay-ports=<UTILWIN_PATH>\vcpkg.ports
```

Example:
```bat
vcpkg install utilwin:x64-windows-static --overlay-ports=C:\Projects\utilwin\vcpkg.ports
```

## upgrade command

```bat
vcpkg upgrade --no-dry-run utilwin:x64-windows-static --overlay-ports=<UTILWIN_PATH>\vcpkg.ports
```
