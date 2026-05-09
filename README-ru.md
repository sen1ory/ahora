# Ahora — Приложение для квизов

Приложение для проведения квизов (викторин), построенное на **Qt 6** (C++ + QML) с **веб-клиентом** (HTML/CSS/JS). Десктопное приложение служит панелью администратора и WebSocket-сервером, а игроки подключаются со своих телефонов через встроенный HTTP-сервер.

## Архитектура

```
ahora/
├── CMakeLists.txt                    # Корневая конфигурация сборки
├── quiz.json                         # Вопросы квиза (16 вопросов, поддержка LaTeX)
│
├── app/                              # QML-код приложения
│   ├── main/                         # Основное десктопное приложение
│   │   ├── CMakeLists.txt            # Qt Quick модуль + исполняемый файл
│   │   ├── src/main.cpp              # Точка входа, настройка серверов и синглтонов
│   │   └── qml/
│   │       ├── Main.qml              # Корневое окно: QR-код ↔ панель администратора
│   │       ├── AdminPanel.qml        # Панель администратора: карточки команд, таймер, детальный просмотр
│   │       ├── TeamCard.qml          # Карточка одной команды
│   │       ├── GruvButton.qml        # Многоразовый стилизованный компонент кнопки
│   │       └── AhoraTheme.qml        # Синглтон цветовой палитры
│   └── qrcode/                       # QML-компонент для рендеринга QR-кода
│       ├── CMakeLists.txt
│       ├── include/QRCodeItem.h
│       └── src/QRCodeItem.cpp
│
├── core/                             # C++ модули бэкенда (статические библиотеки)
│   ├── httpserver/                   # Минимальный HTTP-сервер на порту 8080
│   │   ├── CMakeLists.txt
│   │   ├── include/HttpServer.h
│   │   ├── src/
│   │   │   ├── HttpServer.cpp
│   │   │   ├── web_content.h         # Сгенерировано: полный HTML со встроенными CSS/JS
│   │   │   └── generate_web_content.cmake  # Скрипт сборки, читающий файлы из web/
│   │   └── web/                      # Исходные файлы веб-клиента
│   │       ├── index.html            # HTML-скелет (плейсхолдеры @WEB_STYLE_CONTENT@, @WEB_SCRIPT_CONTENT@)
│   │       ├── style.css             # CSS (тёмная тема gruvbox)
│   │       └── script.js             # JS (WebSocket-клиент, рендеринг квиза, KaTeX)
│   │
│   ├── wsserver/                     # WebSocket-сервер на порту 8081
│   │   ├── CMakeLists.txt
│   │   ├── include/WsServer.h
│   │   └── src/WsServer.cpp          # Обрабатывает join, answer; делегирует в SessionManager
│   │
│   ├── session/                      # Управление сессиями команд
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   │   ├── SessionManager.h      # QAbstractListModel: команды, ответы, очки
│   │   │   ├── QuizDef.h             # Структура QuestionDef, функция checkQuizAnswer()
│   │   │   └── QuizLoader.h          # Загрузка вопросов из quiz.json (с запасным вариантом)
│   │   └── src/SessionManager.cpp
│   │
│   └── ipaddress/                    # Определение IP-адреса в сети
│       ├── CMakeLists.txt
│       ├── include/ipaddress.h
│       └── src/ipaddress.cpp
│
├── tests/                            # Модульные тесты
│   ├── CMakeLists.txt
│   ├── test_sessionmanager.cpp
│   └── test_quizanswer.cpp
│
├── external/                         # Сторонние зависимости
│   └── qrcode/QR-Code-generator/     # Встраиваемая библиотека QR-кодов
│
├── scripts/                          # Вспомогательные скрипты (genqt, initqt)
│
├── build/                            # Результат сборки для Linux
└── build-windows/                    # Результат кросс-компиляции для Windows (через MXE)
```

## Как это работает

1. **Запуск приложения** — Десктопное приложение запускает HTTP-сервер (порт 8080) и WebSocket-сервер (порт 8081).
2. **Экран QR-кода** — Игроки сканируют QR-код с телефона, который открывает `http://<ip>:8080/`.
3. **Подключение** — Игроки вводят название команды и подключаются через WebSocket. Сервер регистрирует их и отправляет вопросы квиза.
4. **Ответы** — Игроки отвечают на вопросы (один выбор, multiple choice, текст). Ответы отправляются через WebSocket.
5. **Панель администратора** — Администратор видит все подключённые команды, статусы их ответов и очки, может оценивать текстовые ответы и управлять таймером игры.

## QML Синглтон: `SM`

C++ объект `SessionManager` доступен в QML как синглтон с именем `SM`:

- `SM.teamCount` — количество подключённых команд
- `SM.questions` — список текстов вопросов (для отображения в QML)
- `SM.teamName(row)` / `SM.teamStatuses(row)` / `SM.teamAnswers(row)` / `SM.teamId(row)` — данные команды по индексу
- `SM.teamAnswersById(id)` / `SM.teamStatusesById(id)` / `SM.teamNameById(id)` / `SM.teamScoreById(id)` — поиск по UUID
- `SM.getTeamDataMap(id)` — полные данные команды в виде QVariantMap
- `SM.addTeam(name, socket)` — регистрация новой команды (возвращает UUID)
- `SM.updateAnswer(id, questionId, status, answers)` — сохранение ответа
- `SM.removeTeam(id)` — удаление отключившейся команды
- `SM.broadcastTimerAction(action)` — отправка "pause"/"resume"/"timeout" всем клиентам
- `SM.approveTextAnswer(teamId, questionId, correct)` — оценка текстового ответа
- `SM.setScore(teamId, questionId, score)` — назначение очков

## QML Тема: `AhoraTheme`

Константы цветов, доступные как QML-синглтон:

| Свойство         | Цвет     | Назначение               |
|------------------|----------|--------------------------|
| `bgDark`         | `#1d2021`| Фон панели               |
| `bgMedium`       | `#282828`| Фон заголовка            |
| `bgBorder`       | `#3c3836`| Границы карточек         |
| `bgMuted`        | `#504945`| Точка «не отвечено»      |
| `textPrimary`    | `#ebdbb2`| Основной текст           |
| `textSecondary`  | `#a89984`| Приглушённый текст       |
| `accentGreen`    | `#98971a`| Правильный ответ         |
| `accentRed`      | `#cc241d`| Неправильный ответ       |
| `accentOrange`   | `#d65d0e`| Ожидает проверки         |
| `accentYellow`   | `#d79921`| Акцент / пауза таймера   |
| `accentBlue`     | `#458588`| Кнопка отправки          |

## Возможности панели администратора

- **Карточки команд** — динамически создаются для каждой подключённой команды, показывают название и точки статуса по каждому вопросу (белый/зелёный/красный/оранжевый). Вопросы обрезаются с многоточием.
- **Таймер** — обратный отсчёт MM:SS в правом верхнем углу. Левый клик запускает/ставит на паузу, правый клик открывает настройки. Транслирует pause/resume/timeout всем клиентам.
- **Детальный просмотр** — клик по карточке команды открывает все вопросы с ответами, кнопки одобрения/отклонения текстовых ответов и поле оценки (0–100). Цвет ответа соответствует статусу.
- **Автообновление** — карточки команд пересоздаются при сигналах `teamCountChanged` и `teamDataChanged`.

## Веб-клиент (HTML/CSS/JS)

Веб-клиент раздаётся `HttpServer` по запросу `GET /`. Исходные файлы находятся в `core/httpserver/web/`:

- `index.html` — HTML-скелет с плейсхолдерами `@WEB_STYLE_CONTENT@` и `@WEB_SCRIPT_CONTENT@`
- `style.css` — тёмная тема gruvbox, адаптивная вёрстка
- `script.js` — WebSocket-клиент, рендеринг квиза, отправка ответов, переподключение с экспоненциальной задержкой

Во время сборки CMake-скрипт `generate_web_content.cmake` читает три файла, подставляет плейсхолдеры и создаёт `web_content.h` — один C++-заголовок с полной HTML-страницей в виде raw string literal. Ручная сборка не требуется.

### Поддержка LaTeX

Вопросы, содержащие блоки `$$...$$`, рендерятся с помощью **KaTeX** (загружается с CDN). Скрипт добавляет класс `has-math` карточкам с LaTeX, что динамически расширяет ширину карточки для формулы, сохраняя фиксированную ширину 380px для текстовых вопросов.

### Протокол WebSocket

| Направление | Сообщение | Назначение |
|-------------|-----------|------------|
| Клиент → Сервер | `{"type":"join","name":"..."}` | Регистрация команды |
| Сервер → Клиент | `{"type":"joined","id":"..."}` | Подтверждение подключения |
| Сервер → Клиент | `{"type":"quiz","questions":[...]}` | Вопросы квиза (без ответов) |
| Клиент → Сервер | `{"type":"answer","questionId":N,"answers":[...]}` | Отправка ответа |
| Сервер → Клиент | `{"type":"result","questionId":N,"status":"..."}` | Статус ответа (green/red/orange) |
| Сервер → Клиент | `{"type":"timer","action":"pause|resume|timeout"}` | Состояние таймера |

## Вопросы квиза

Загружаются из `quiz.json` при запуске (поиск в `applicationDirPath` и `currentPath`). Используются 3 жёстко заданных вопроса из `QuizDef.h`, если файл не найден. Поддерживаются три типа вопросов:

- **single** — выбор одного варианта (radio-button)
- **multiple** — выбор нескольких вариантов (checkbox)
- **text** — свободный текстовый ввод (оценивается администратором вручную)

## Статусы ответов

| Статус   | Значение                       |
|----------|--------------------------------|
| `white`  | Не отвечено                    |
| `green`  | Правильно                      |
| `red`    | Неправильно                    |
| `orange` | Ожидает ручной проверки (только текстовые вопросы) |

## Поток данных

```
Телефон игрока                   Десктопное приложение
     │                                │
     │── HTTP GET / ──────────────►   HttpServer :8080
     │◄── HTML-страница (web/ файлы) ─│
     │                                │
     │── WS connect :8081 ──────────► WsServer
     │── {"type":"join",…} ──────────►│──► SessionManager.addTeam()
     │◄── {"type":"quiz",…} ──────────│
     │── {"type":"answer",…} ────────►│──► SessionManager.updateAnswer()
     │◄── {"type":"result",…} ────────│
     │◄── {"type":"timer",…} ─────────│──► SessionManager.broadcastTimerAction()
     │                                │
     │                  ┌─────────────────────────┐
     │                  │  QML Панель администратора│
     │                  │  (AdminPanel.qml)         │
     │                  │  ←→ SM синглтон           │
     │                  └─────────────────────────┘
```

## Сборка

### Linux (нативная)

```bash
# Требования: Qt 6.5+, CMake 3.16+
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./ahora
```

Символическая ссылка `./ahora` указывает на `build/app/main/ahora`.

### Windows (кросс-компиляция через MXE)

```bash
# Требования: MXE с Qt6 (см. mxe.cc)
export PATH=~/dev/ahorav2/mxe/usr/bin:$PATH
x86_64-w64-mingw32.static-cmake -S . -B build-windows
cd build-windows && make -j$(nproc)
# Результат: build-windows/ahora.exe (статический, без DLL)
```

## Тесты

```bash
cmake --build build -j$(nproc)
./build/tests/test_sessionmanager
./build/tests/test_quizanswer
```