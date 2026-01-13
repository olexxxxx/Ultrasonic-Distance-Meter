SOFTWARE_PART/
│
├── .vscode/                    # Налаштування середовища VS Code
│
├── business_logic/             
│   ├── interfaces/             
│   ├── logger_config/          
│   └── logic/                  
│
├── drivers/                    # Драйвери для периферії
│   ├── inc/                    
│   │   ├── buttons/            
│   │   ├── display/            
│   │   ├── indication/         
│   │   ├── logger/             
│   │   └── sensor/             
│   │
│   └── src/                    # Реалізація функціоналу драйверів (.c)
│       ├── buttons/
│       ├── display/
│       ├── indication/
│       ├── logger/
│       └── sensor/
│
├── platform_dependencies/      # Специфічні файли для конкретної платформи/МК
│   ├── inc/                    # Заголовки для платформозалежного коду
│   ├── registers_include/      # Визначення регістрів мікроконтролера
│   └── src/                    # Низькорівнева реалізація (HAL)
│
├── start/                      # Файли ініціалізації та точка входу (main)
│
├── build.bat                   # Скрипт для збірки проєкту
└── flash.bat                   # Скрипт для прошивки мікроконтролера


[business_logic/logic] (Прикладний рівень та алгоритми керування) 
                                    ⬇ 
[business_logic/interfaces] (Абстрактний контракт взаємодії) 
                                    ⬇ 
[drivers/src] (Реалізація логіки периферійних пристроїв) 
                                    ⬇ 
[platform_dependencies/src] (Адаптація до специфіки апаратної платформи) 
                                    ⬇ 
[platform_dependencies/registers_include] (Карта регістрів мікроконтролера)