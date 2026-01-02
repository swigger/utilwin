# pre reinstall

* WSL
  1. ~/.dircolors
  2. /etc/fstab
  3. homedir
* Process Apps
  1. deauth itunes
  2. msvc user.props
* backup data: 
  1. Documents folder
  2. securecrt config dir
  3. outlook files
  4. Desktop folder
  5. Pictures folder
  6. .ssh folder
* save windows program list

```
Get-ItemProperty HKLM:\Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\* | Select-Object DisplayName, DisplayVersion, Publisher, InstallDate | Format-Table AutoSize > InstalledPrograms-PS.txt
Get-ItemProperty HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall\* | Select-Object DisplayName, DisplayVersion, Publisher, InstallDate > InstalledProgs.64.txt
Get-ItemProperty HKCU:\Environment | Select-Object  Path | Out-String -width 4000 > path.txt
```


# installing Windows
* create efi part

```
diskpart
list disk
select disk
clean
convert gpt
create partition efi size=200
format quick fs=fat32
create partition primary
format quick fs=ntfs
assign letter=W
exit
```

* create vhd

```
mkdir w:\vhd
diskpart
create vdisk file=”w:\vhd\win10.vhd” maximum=80000 type=expandable
attach vdisk
exit
```

* activate Windows

```bat
rem for enterprise
slmgr /ipk NPPR9-FWDCX-D2C8J-H872K-2YT43
rem for professional
slmgr /ipk W269N-WFGWX-YVC9B-4J6C9-T83GX
slmgr /skms 192.168.2.3
slmgr /ato
slmgr /dlv
```

# after reinstall
* Uninstall Pre-Installed Apps in Windows 10 Using Powershell

```
Get-AppxPackage | where-object {$_.name -notlike "*WindowsStore*"} | where-object {$_.name -notlike "*ubuntu*"}  | Remove-AppxPackage
```

* some settings

```
cd /d C:\Windows\system32\
mklink /D ibin D:\dev.down\vcpkg\installed\x64-windows\bin
cd ..\SysWOW64
mklink /D ibin D:\dev.down\vcpkg\installed\x86-windows\bin
reg add HKCU\Software\Microsoft\Windows\CurrentVersion\Run /v wslinit /t REG_SZ /d D:\work\linuxset\wsl\win.init.bat
fsutil behavior set disablelastaccess 1
```

* [install openssh for windows](https://docs.microsoft.com/en-us/windows-server/administration/openssh/openssh_install_firstuse)

```
cd $Env:ProgramData
mkdir ssh
cd ssh
New-Item -ItemType file administrators_authorized_keys
icacls administrators_authorized_keys /inheritance:r
icacls administrators_authorized_keys /grant SYSTEM:`(F`)
icacls administrators_authorized_keys /grant BUILTIN\Administrators:`(F`)
#  to debug file perm, edit C:\programdata\ssh\sshd_config
#  SyslogFacility LOCAL0
#  LogLevel INFO
# then see sshd.log in C:\programdata\ssh\logs
```

* install and initialize wsl

  1. mod /etc/fstab: `C: /mnt/c drvfs metadata,rw,noatime 0 0`
  2. sync homedir .dircolors, etc.

# other

* vs2019 settings.

```
cd /d %LOCALAPPDATA%
md Microsoft\MSBuild\v4.0
cd Microsoft\MSBuild\v4.0
psuac cmd /c mklink Microsoft.Cpp.x64.user.props <UTILWIN_PATH>\cfg\Microsoft.Cpp.x64.user.props
psuac cmd /c mklink Microsoft.Cpp.Win32.user.props <UTILWIN_PATH>\cfg\Microsoft.Cpp.Win32.user.props
```

Replace `<UTILWIN_PATH>` with your actual utilwin repository path, e.g., `D:\work\utilwin`

* create efi boot entry on linux:

```bash
MODS=search disk ext2 part_gpt part_msdos configfile fat normal linux ls boot echo reboot search_fs_file search_fs_uuid search_label help efi_gop efi_uga gfxterm
grub-mkimage -o /.../EFI/ubuntu/startup.efi -O x86_64-efi -c /boot/grub/x86_64-efi/load.cfg $MODS
efibootmgr -c -b 8 -L "ubuntu" -l '\EFI\ubuntu\startup.efi'
efibootmgr -o 3,5,7,8
```

* activate office 2016

```
rem cd /d C:\Program Files (x86)\Microsoft Office\Office15
rem cscript OSPP.VBS /inpkey:YC7DK-G2NP3-2QQC3-J6H88-GVGXT
cd /d C:\Program Files\Microsoft Office\Office16
cscript OSPP.VBS /inpkey:XQNVK-8JYDB-WJ9W3-YJ8YR-WFG99
cscript OSPP.VBS /sethst:192.168.2.3
cscript OSPP.VBS /act
```
