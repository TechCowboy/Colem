# `Colem`

This project is to add Fujinet support to the emulator and will be referred to as
5.6f

# How to build

change into the Unix directory and execute one of the following:

##Erase all generated files
```make clean```

##Make the executible
```make colem```
```make all```

##Run the emulator
```make go```


# How to debug under Unix
The emulator with ```make go``` issues the following command:

```./colem -verbose 128 -ntfs -adam reversi.ddp```


## Credits

The Portable ColecoVision and Coleco Adam Emulator was created by Marat Fayzullin
ColEm source code is freely distributable, but it is NOT public domain and is NOT released under GPL.

https://fms.komkon.org/ColEm/

This is based on ColEm 5.6, that was  published on March 3 2021, makes most command line options work on Windows and adds optional frame rate display. The previous release also added the "Hardware | Paste Clipboard" option to ColEm-Windows, when running in Coleco Adam mode. This option allows pasting Windows clipboard contents directly into Adam software.

Adding Fujinet support by Norman Davie
