# Visual Studio Configuration Files

## Microsoft.Cpp.*.user.props

These files provide user-specific MSBuild configuration for Visual Studio projects.

### Installation

To use these configuration files, create symbolic links in your local MSBuild directory:

```bat
cd /d %LOCALAPPDATA%\Microsoft\MSBuild\v4.0
mklink Microsoft.Cpp.x64.user.props <UTILWIN_PATH>\cfg\Microsoft.Cpp.x64.user.props
mklink Microsoft.Cpp.Win32.user.props <UTILWIN_PATH>\cfg\Microsoft.Cpp.Win32.user.props
```

Replace `<UTILWIN_PATH>` with your actual utilwin repository path.

### Configuration

The `.props` files automatically include `inc_win` from the utilwin repository using relative paths.
You can customize additional paths according to your development environment by editing these files.

## VCPKG Configuration (Optional)

If you use custom vcpkg configuration:

1. Install vcpkg by its official method
2. Copy or link `vcpkg.user.*` files to `%LOCALAPPDATA%\vcpkg\`, overwriting existing files if needed

