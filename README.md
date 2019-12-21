# winfetch

__NOTE: Very experimental at the moment.__

winfetch is an attempt to create a packagable tool that installs from Microsoft store and makes it easy to fetch URLs from the web.

## Usage
If you install the package, it should be available to the logged in user system wide.

```powershell
PS C:\Directory> winfetch http://www.example.com
```

## Contributing
This is an attempt to build an easy to install windows tool, and there is no plan to make work down-level (older than Windows 1803). It currently depends on Appx and the desktop bridge to work, features that weren't available in earlier builds of Windows 10. 

Any contribution that works with these constraints is welcome.

## License

* MIT license (LICENSE-MIT or http://opensource.org/licenses/MIT)
