@echo off
set OLDPATH=%PATH%
set UTILWIN_DIR=%~dp0
set UTILWIN_DIR=%UTILWIN_DIR:~0,-1%

rem TODO: Update VCPKG_DIR and VS paths according to your environment
set VCPKG_DIR=D:\dev.down\vcpkg
set VS_VCVARS64=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat
set VS_VCVARS32=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsamd64_x86.bat

rem ================== doing 64 =============
call :reset_env
call "%VS_VCVARS64%"
set VCPKG_ROOT=
cd /d "%VCPKG_DIR%"
rem vcpkg upgrade --no-dry-run utilwin:x64-windows-static --overlay-ports=%UTILWIN_DIR%\vcpkg.ports
vcpkg remove utilwin:x64-windows-static & rd /s /q %localappdata%\vcpkg\archives
vcpkg install utilwin:x64-windows-static --overlay-ports=%UTILWIN_DIR%\vcpkg.ports

rem ================== doing 32 =============
call :reset_env
call "%VS_VCVARS32%"
set VCPKG_ROOT=
cd /d "%VCPKG_DIR%"
rem vcpkg upgrade --no-dry-run utilwin:x86-windows-static --overlay-ports=%UTILWIN_DIR%\vcpkg.ports
vcpkg remove utilwin:x86-windows-static
vcpkg install utilwin:x86-windows-static --overlay-ports=%UTILWIN_DIR%\vcpkg.ports

rem ===== all done ====================
pause
exit /b 0


:reset_env
echo.
echo Resetting Env
echo.
set CommandPromptType=
set DevEnvDir=
set __DOTNET_ADD_64BIT=
set __DOTNET_PREFERRED_BITNESS=
set ExtensionSdkDir=
set EXTERNAL_INCLUDE=
set Framework40Version=
set FrameworkDir=
set FrameworkDir64=
set FrameworkVersion=
set FrameworkVersion64=
set FSHARPINSTALLDIR=
set HTMLHelpDir=
set INCLUDE=
set LIB=
set LIBPATH=
set NETFXSDKDir=
set Platform=
set UCRTVersion=
set UniversalCRTSdkDir=
set VCIDEInstallDir=
set VCINSTALLDIR=
set VCToolsInstallDir=
set VCToolsRedistDir=
set VCToolsVersion=
set VisualStudioVersion=
set VS170COMNTOOLS=
set VSCMD_ARG_app_plat=
set VSCMD_ARG_HOST_ARCH=
set VSCMD_ARG_TGT_ARCH=
set __VSCMD_PREINIT_PATH=
set VSCMD_VER=
set VSINSTALLDIR=
set VSSDK150INSTALL=
set VSSDKINSTALL=
set WindowsLibPath=
set WindowsSdkBinPath=
set WindowsSdkDir=
set WindowsSDK_ExecutablePath_x64=
set WindowsSDK_ExecutablePath_x86=
set WindowsSDKLibVersion=
set WindowsSdkVerBinPath=
set WindowsSDKVersion=
rem set PATH=%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;%SystemRoot%\System32\OpenSSH
set PATH=%OLDPATH%
exit /b 0
