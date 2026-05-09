# Ahora — Quiz Application

**[Русская версия](README-ru.md)**

A quiz (trivia) application built with **Qt 6** (C++ + QML) and a **web-based client** (HTML/CSS/JS). The desktop app serves as the admin panel and WebSocket server, while players connect from their phones via a built-in HTTP server.

## Architecture

```
ahora/
├── CMakeLists.txt                    # Root build config
├── quiz.json                         # Quiz questions (16 questions, LaTeX support)
│
├── app/                              # QML application code
│   ├── main/                         # Main desktop app
│   │   ├── CMakeLists.txt            # Qt Quick module + executable
│   │   ├── src/main.cpp              # Entry point, server + singleton setup
│   │   └── qml/
│   │       ├── Main.qml              # Root Window: QR code ↔ admin panel
│   │       ├── AdminPanel.qml        # Admin panel: team cards, timer, detail popup
│   │       ├── TeamCard.qml          # Single team card widget
│   │       ├── GruvButton.qml        # Reusable styled button component
│   │       └── AhoraTheme.qml        # Color palette singleton
│   └── qrcode/                       # QR Code rendering QML component
│       ├── CMakeLists.txt
│       ├── include/QRCodeItem.h
│       └── src/QRCodeItem.cpp
│
├── core/                             # C++ backend modules (static libraries)
│   ├── httpserver/                   # Minimal HTTP server on port 8080
│   │   ├── CMakeLists.txt
│   │   ├── include/HttpServer.h
│   │   ├── src/
│   │   │   ├── HttpServer.cpp
│   │   │   ├── web_content.h         # Generated: full HTML with inlined CSS/JS
│   │   │   └── generate_web_content.cmake  # Build-time script reading web/ files
│   │   └── web/                      # Source files for the web client
│   │       ├── index.html            # HTML skeleton (@WEB_STYLE_CONTENT@, @WEB_SCRIPT_CONTENT@)
│   │       ├── style.css             # CSS (gruvbox dark theme)
│   │       └── script.js             # JS (WebSocket client, quiz rendering, KaTeX)
│   │
│   ├── wsserver/                     # WebSocket server on port 8081
│   │   ├── CMakeLists.txt
│   │   ├── include/WsServer.h
│   │   └── src/WsServer.cpp          # Handles join, answer; delegates to SessionManager
│   │
│   ├── session/                      # Team session management
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   │   ├── SessionManager.h      # QAbstractListModel: teams, answers, scores
│   │   │   ├── QuizDef.h             # QuestionDef struct, checkQuizAnswer()
│   │   │   └── QuizLoader.h          # Loads quiz from quiz.json (fallback to defaults)
│   │   └── src/SessionManager.cpp
│   │
│   └── ipaddress/                    # Network IP discovery
│       ├── CMakeLists.txt
│       ├── include/ipaddress.h
│       └── src/ipaddress.cpp
│
├── tests/                            # Unit tests
│   ├── CMakeLists.txt
│   ├── test_sessionmanager.cpp
│   └── test_quizanswer.cpp
│
├── external/                         # Third-party dependencies
│   └── qrcode/QR-Code-generator/     # Embedded QR code library
│
├── scripts/                          # Helper scripts (genqt, initqt)
│
├── build/                            # Linux build output
└── build-windows/                    # Windows cross-compile output (via MXE)
```

## How It Works

1. **Start the app** — The desktop app launches an HTTP server (port 8080) and a WebSocket server (port 8081).
2. **QR code screen** — Players scan the QR code from their phone, which opens `http://<ip>:8080/`.
3. **Join** — Players enter a team name and connect via WebSocket. The server registers them and sends the quiz questions.
4. **Answer** — Players answer questions (single-choice, multiple-choice, text). Answers are sent via WebSocket.
5. **Admin panel** — The admin sees all connected teams, their answer statuses and scores, can grade text answers, and controls the game timer.

## QML Singleton: `SM`

The `SessionManager` C++ object is exposed to QML as a singleton named `SM`:

- `SM.teamCount` — number of connected teams
- `SM.questions` — list of question texts (for QML display)
- `SM.teamName(row)` / `SM.teamStatuses(row)` / `SM.teamAnswers(row)` / `SM.teamId(row)` — per-team data
- `SM.teamAnswersById(id)` / `SM.teamStatusesById(id)` / `SM.teamNameById(id)` / `SM.teamScoreById(id)` — lookup by UUID
- `SM.getTeamDataMap(id)` — full team data as a QVariantMap
- `SM.addTeam(name, socket)` — register a new team (returns UUID)
- `SM.updateAnswer(id, questionId, status, answers)` — store an answer
- `SM.removeTeam(id)` — remove a disconnected team
- `SM.broadcastTimerAction(action)` — send "pause"/"resume"/"timeout" to all clients
- `SM.approveTextAnswer(teamId, questionId, correct)` — grade a text answer
- `SM.setScore(teamId, questionId, score)` — assign points

## QML Theme: `AhoraTheme`

Color constants exposed as a QML singleton:

| Property         | Color    | Usage               |
|------------------|----------|----------------------|
| `bgDark`         | `#1d2021`| Panel background     |
| `bgMedium`       | `#282828`| Header background    |
| `bgBorder`       | `#3c3836`| Card borders         |
| `bgMuted`        | `#504945`| Unanswered dot       |
| `textPrimary`    | `#ebdbb2`| Main text            |
| `textSecondary`  | `#a89984`| Muted text           |
| `accentGreen`    | `#98971a`| Correct answer       |
| `accentRed`      | `#cc241d`| Incorrect answer     |
| `accentOrange`   | `#d65d0e`| Pending review       |
| `accentYellow`   | `#d79921`| Accent / timer pause |
| `accentBlue`     | `#458588`| Submit button        |

## Admin Panel Features

- **Team cards** — dynamically created for each connected team, showing name and per-question status dots (white/green/red/orange). Questions are truncated with ellipsis.
- **Timer** — countdown MM:SS timer in the top-right corner. Left-click toggles start/pause, right-click opens settings. Broadcasts pause/resume/timeout to all clients.
- **Detail popup** — click a team card to see all questions with answers, approve/reject text answers, and set score (0–100). Status-colored answer text.
- **Auto-rebuild** — team cards are recreated on `teamCountChanged` and `teamDataChanged` signals.

## Web Client (HTML/CSS/JS)

The web client is served by `HttpServer` on `GET /`. Source files live in `core/httpserver/web/`:

- `index.html` — HTML skeleton with `@WEB_STYLE_CONTENT@` and `@WEB_SCRIPT_CONTENT@` placeholders
- `style.css` — gruvbox dark theme, responsive layout
- `script.js` — WebSocket client, quiz rendering, answer submission, reconnection with exponential backoff

At build time, CMake's `generate_web_content.cmake` script reads the three files, substitutes the placeholders, and produces `web_content.h` — a single C++ header with the full HTML page as a raw string literal. No manual assembly needed.

### LaTeX Support

Questions containing `$$...$$` blocks are rendered using **KaTeX** (loaded from CDN). The script adds `has-math` class to cards with LaTeX, which expands the card width dynamically to fit formulas while keeping text-only cards at a fixed 380px.

### WebSocket Protocol

| Direction | Message | Purpose |
|-----------|---------|---------|
| Client → Server | `{"type":"join","name":"..."}` | Team registration |
| Server → Client | `{"type":"joined","id":"..."}` | Join confirmation |
| Server → Client | `{"type":"quiz","questions":[...]}` | Quiz questions (answers excluded) |
| Client → Server | `{"type":"answer","questionId":N,"answers":[...]}` | Submit answer |
| Server → Client | `{"type":"result","questionId":N,"status":"..."}` | Answer status (green/red/orange) |
| Server → Client | `{"type":"timer","action":"pause|resume|timeout"}` | Timer state |

## Quiz Questions

Loaded from `quiz.json` at startup (searched in `applicationDirPath` and `currentPath`). Falls back to 3 hardcoded defaults in `QuizDef.h` if no valid file is found. Supports three question types:

- **single** — radio-button selection
- **multiple** — checkbox selection
- **text** — free-form text input (graded manually by admin)

## Answer Statuses

| Status  | Meaning |
|---------|---------|
| `white` | Unanswered |
| `green` | Correct |
| `red`   | Incorrect |
| `orange`| Pending manual review (text questions only) |

## Data Flow

```
Phone Browser                     Desktop App
     │                                │
     │── HTTP GET / ──────────────►   HttpServer :8080
     │◄── HTML page (web/ files) ─────│
     │                                │
     │── WS connect :8081 ──────────► WsServer
     │── {"type":"join",…} ──────────►│──► SessionManager.addTeam()
     │◄── {"type":"quiz",…} ──────────│
     │── {"type":"answer",…} ────────►│──► SessionManager.updateAnswer()
     │◄── {"type":"result",…} ────────│
     │◄── {"type":"timer",…} ─────────│──► SessionManager.broadcastTimerAction()
     │                                │
     │                  ┌─────────────────────┐
     │                  │  QML Admin Panel    │
     │                  │  (AdminPanel.qml)   │
     │                  │  ←→ SM singleton    │
     │                  └─────────────────────┘
```

## Building

### Linux (native)

```bash
# Prerequisites: Qt 6.5+, CMake 3.16+
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./ahora
```

The `./ahora` symlink points to `build/app/main/ahora`.

### Windows (cross-compile with MXE)

```bash
# Prerequisites: MXE with Qt6 (see mxe.cc)
export PATH=~/dev/ahorav2/mxe/usr/bin:$PATH
x86_64-w64-mingw32.static-cmake -S . -B build-windows
cd build-windows && make -j$(nproc)
# Output: build-windows/ahora.exe (static, no DLLs needed)
```

## Tests

```bash
cmake --build build -j$(nproc)
./build/tests/test_sessionmanager
./build/tests/test_quizanswer
```