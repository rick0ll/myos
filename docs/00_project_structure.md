# PROJECT STRUCTURE

```text
.
├── docs                # Documentazione del progetto
│   └── kernel          # Documentazione specifica del kernel
│  
├── isodir              # Cartella temporanea per generare l'immagine ISO dell'OS
│   └── boot            # Cartella contenente il binario del kernel
│       └── grub        # Cartella contenente la configurazione di GRUB
│  
├── kernel              # Cartella principale del kernel
│   ├── arch            # Cartella specifica con i file specifici dell'architettura
│   │   └── i386
│   │  
│   ├── include         # Cartella con gli header (*.h) dei file usati dal kernel
│   │   └── kernel
│   │  
│   └── kernel          # Cartella con i file (*.c) usati dal kernel e il main
│  
├── libc                # Cartella con i file delle librerie dell'OS
│   ├── include         # Cartella con gli header dei file delle librerie
│   │   └── sys         # Cartella con l'header identificatore del Sistema Operativo
│   │  
│   ├── stdio
│   ├── stdlib
│   └── string
│  
├── sysroot             # Cartella root dell'OS ("/")
│   ├── boot            # Cartella con il file binario del kernel
│   └── usr
│       ├── include     # Cartella con gli header delle librerie dell'OS
│       │   ├── kernel  # Cartella con gli header necessari al kernel
│       │   └──sys      # Cartella con gli header delle system call
│       │  
│       └── lib         # Cartella con le librerie dell'user
│  
└── tests               # Cartella con i file di test
    ├── kernel          # Cartella con i file di test del kernel
    └── libc            # Cartella con i file di test della libc

```
