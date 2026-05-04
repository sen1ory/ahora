# Ahora — Quiz Application

A quiz (trivia) application built with **Qt 6** (C++ + QML) and a **web-based client** (HTML/JS). The desktop app serves as the admin panel and WebSocket server, while players connect from their phones via a built-in HTTP server.

## Architecture

```
ahora/
├── CMakeLists.txt              # Root build config
├── app/                         # QML application code
│   ├── main/                    # Main desktop app
│   │   ├── CMakeLists.txt       # Qt Quick module + executable
│   │   ├── src/main.cpp         # Entry point, server setup, QML singleton registration
│   │   ├── qml/
│   │   │   ├── Main.qml         # Root Window: QR code view ↔ admin panel
│   │   │   ├── AdminPanel.qml   # Admin panel: team cards, timer, detail popup
│   │   │   └── TeamCard.qml     # Single team card widget
│   │   └── resources/           # Static assets (media, etc.)
│   └── qrcode/                  # QR Code rendering QML component
│       ├── QRCodeItem.h/.cpp    # QQuickPaintedItem for QR codes
│       └── CMakeLists.txt
├── core/                        # C++ backend modules
│   ├── httpserver/              # Minimal HTTP server on port 8080
│   │   ├── HttpServer.h/.cpp    # Serves web client HTML page
│   │   └── web_content.h        # Inlined HTML/JS/CSS for the web client
│   ├── ipadress/                # Network IP discovery
│   │   └── ipadress.h/.cpp      # Enumerates interfaces, selects best IP
│   ├── session/                 # Team session management
│   │   ├── SessionManager.h/.cpp # QAbstractListModel: teams, answers, scores, broadcast
│   │   └── QuizDef.h            # Hard-coded quiz questions (single, multiple, text)
│   └── wsserver/                # WebSocket server on port 8081
│       └── WsServer.h/.cpp      # Handles join, answer messages, delegates to SessionManager
├── external/qrcode/             # Embedded QR-Code-generator library
├── build/                       # CMake build output
├── tests/                       # Test files (empty)
└── data/                        # Data files (empty)
```

## How It Works

1. **Start the app** — The desktop app launches an HTTP server (port 8080) and a WebSocket server (port 8081).
2. **QR code screen** — Players scan the QR code from their phone, which opens `http://<ip>:8080/`.
3. **Join** — Players enter a team name and connect via WebSocket. The server registers them and sends the quiz questions.
4. **Answer** — Players answer questions (single-choice, multiple-choice, text). Answers are sent via WebSocket.
5. **Admin panel** — The admin sees all connected teams, their answer statuses (white/green/red/orange), can grade text answers, and controls the game timer.

## QML Singleton: `SM`

The `SessionManager` C++ object is exposed to QML as a singleton named `SM` via `qmlRegisterSingletonInstance`. In QML files, use:

- `SM.teamCount` — number of connected teams
- `SM.questions` — list of question texts
- `SM.teamName(row)` / `SM.teamStatuses(row)` / `SM.teamAnswers(row)` — per-team data
- `SM.addTeam(name, socket)` — register a new team
- `SM.broadcastTimerAction(action)` — send "pause"/"resume"/"timeout" to all clients
- `SM.approveTextAnswer(teamId, questionId, correct)` — grade a text answer
- `SM.setScore(teamId, questionId, score)` — assign points

## Building

```bash
# Prerequisites: Qt 6.5+, CMake 3.16+
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./ahora
```

The `./ahora` symlink points to `build/app/main/ahora`.

## Web Client (HTML/JS)

The web client is inlined in `core/httpserver/src/web_content.h` as a raw string. It is served by `HttpServer` on `GET /`. The client:

1. Shows a join screen (team name input)
2. Connects via WebSocket to `ws://<host>:8081`
3. Receives quiz questions and renders them
4. Sends answers and receives result statuses
5. Handles timer events (pause/resume/timeout overlay)

## Quiz Questions

Currently hard-coded in `core/session/include/QuizDef.h`:

| # | Type     | Question                          |
|---|----------|-----------------------------------|
| 0 | single   | "Which planet is the largest?"    |
| 1 | multiple | "Which languages are compiled?"   |
| 2 | text     | "What is the capital of France?"  |

## Data Flow

```
Phone Browser                     Desktop App
     │                                │
     │── HTTP GET / ──────────────►   HttpServer :8080
     │◄── HTML page ──────────────────│
     │                                │
     │── WS connect :8081 ──────────► WsServer
     │── {"type":"join",…} ──────────►│──► SessionManager.addTeam()
     │◄── {"type":"quiz",…} ──────────│
     │── {"type":"answer",…} ────────►│──► SessionManager.updateAnswer()
     │◄── {"type":"result",…} ────────│
     │◄── {"type":"timer",…} ─────────│──► SessionManager.broadcastTimerAction()
```

---

## Features Added

Implemented on top of the initial skeleton:

### Timer (AdminPanel.qml)
- Countdown timer `MM:SS` in the top-right corner of the admin panel
- **Left-click** toggles start/pause
- **Right-click** opens a context menu to set minutes and seconds (default 5:00)
- When paused or expired, a **full-screen overlay** blocks all interaction on both admin and player devices
- Timer state is broadcast to all connected clients via WebSocket (`pause`/`resume`/`timeout`)

### Web Client Timer Overlay (web_content.h)
- Fixed `ws://` vs `wss://` protocol detection
- Clients receive `{"type":"timer","action":"pause|resume|timeout"}` messages
- A styled overlay blocks interaction during pause and on timeout

### Team Display Fix
- Switched from `setContextProperty("sessionManager", …)` to `qmlRegisterSingletonInstance` — the `required property` declaration was shadowing the context property
- `SessionManager` is now registered as a QML singleton `SM` in the `Ahora_app_main` module
- Added a **placeholder** "⏳ Waiting for teams…" when no teams are connected
- Team cards are **centered both horizontally and vertically** in the scroll area

### Clickable Team Cards & Detail Popup
- Each team card is now clickable (pointing hand cursor)
- Clicking opens a **detail popup** showing:
  - Team name
  - All quiz questions with the team's answers and statuses
  - **Approve / Reject buttons** for text answers (question #2)
  - **Score spinner** (0–100 points) + "Save" button
- Colors follow the convention: white (unanswered), yellow/amber (awaiting review), green (correct), red (wrong)

### Answer Storage (SessionManager)
- `TeamData.answers` — stores the actual answer text per question
- `updateAnswer()` now accepts and persists the submitted answers
- Added `teamAnswersById()`, `teamStatusesById()`, `teamNameById()`, `setScore()`
- Added detailed logging for team additions, answers, and removals

### WebSocket Server
- `WsServer::handleAnswer()` now passes the raw answers to `SessionManager::updateAnswer()`
- Added logging of received answers for debugging
