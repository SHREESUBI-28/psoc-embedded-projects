Below are the steps to be followed to make this workspace work in your system:

1. Go to Debug->Makefile
    Change the below path as per your installation:
        a. line 18: OCD = <YOUR_PATH>

2. Go to .vscode -> settings.json
    Change the below two paths:
        a. "OpenOCD_path" : "<YOUR_PATH>"
        b. "local_gdbPath" : "<YOUR_PATH>"

3. Create a path for openocd in user environment variables