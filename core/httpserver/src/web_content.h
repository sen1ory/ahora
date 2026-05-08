#ifndef WEB_CONTENT_H
#define WEB_CONTENT_H


#include <string>


inline const std::string &webContentHtml() {
    static const std::string html = std::string(R"raw(
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<title>Квиз</title>
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.css">
<script src="https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.js"></script>
<script src="https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/contrib/auto-render.min.js"></script>
<style>
* { margin: 0; padding: 0; box-sizing: border-box; }
body {
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
    background: #1d2021;
    color: #ebdbb2;
    min-height: 100vh;
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 24px;
}

h1 { color: #d79921; margin: 40px 0 16px; font-size: 28px; }
h2 { color: #ebdbb2; margin: 16px 0; font-size: 20px; }
p  { color: #a89984; margin: 8px 0; font-size: 14px; }

/* Join screen — team name input */
#joinScreen {
    width: 100%;
    max-width: 400px;
    text-align: center;
    margin-top: 80px;
}

/* Quiz screen */
#quizScreen {
    width: 100%;
    max-width: 100%;
    display: none;
}

#questionsContainer {
    display: flex;
    flex-direction: column;
    align-items: center;
}

/* Question card */
.question-card {
    background: #282828;
    border-radius: 12px;
    padding: 16px;
    margin: 12px 0;
    overflow-x: auto;
    width: fit-content;
    min-width: min(300px, 100%);
    max-width: 100%;
}

input, textarea, button {
    font-family: inherit;
    font-size: 16px;
    border: none;
    border-radius: 8px;
    padding: 12px 16px;
    width: 100%;
    margin: 8px 0;
}

input, textarea {
    background: #3c3836;
    color: #ebdbb2;
}

input:focus, textarea:focus {
    outline: 2px solid #d79921;
    background: #504945;
}

button {
    background: #d79921;
    color: #1d2021;
    font-weight: bold;
    cursor: pointer;
    transition: background 0.2s;
}

button:hover { background: #fabd2f; }
button:disabled {
    background: #504945;
    color: #a89984;
    cursor: not-allowed;
}

/* Question card */
.question-card {
    background: #282828;
    border-radius: 12px;
    padding: 16px;
    margin: 12px 0;
    width: 380px;
    max-width: 100%;
}

.question-card.has-math {
    width: fit-content;
    overflow-x: auto;
}

.question-card .question-text {
    font-size: 16px;
    font-weight: bold;
    margin-bottom: 12px;
    color: #ebdbb2;
}

.question-card .status-dot {
    display: inline-block;
    width: 14px;
    height: 14px;
    border-radius: 50%;
    margin-right: 8px;
    vertical-align: middle;
}

/* Radio buttons and checkboxes as block elements */
.option {
    display: block;
    background: #3c3836;
    border-radius: 8px;
    padding: 10px 14px;
    margin: 6px 0;
    cursor: pointer;
    transition: background 0.15s;
}

.option:hover { background: #504945; }
.option.selected { background: #d79921; color: #1d2021; }

.option input {
    display: none;
}

/* "Sent" message after answer submission */
.answer-sent {
    margin-top: 8px;
    padding: 6px 12px;
    border-radius: 8px;
    background: #504945;
    color: #a89984;
    font-size: 13px;
    display: none;
}

.answer-sent.visible { display: block; }

/* Block interaction after submission */
.question-card.sent .option {
    opacity: 0.6;
    pointer-events: none;
}
.question-card.sent .submit-answer {
    display: none;
}

/* Pause/timeout overlay */
#timerOverlay {
    position: fixed;
    top: 0; left: 0; right: 0; bottom: 0;
    background: rgba(0,0,0,0.85);
    display: none;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    z-index: 9999;
}
#timerOverlay h1 {
    font-size: 48px;
    margin-bottom: 16px;
}
#timerOverlay p {
    font-size: 18px;
    color: #a89984;
}

/* Submit answer button for each question */
.submit-answer {
    background: #458588;
    color: #ebdbb2;
    margin-top: 8px;
}

.submit-answer:hover { background: #83a598; }
.submit-answer:disabled {
    background: #504945;
    color: #a89984;
}

</style>
</head>
<body>
<!-- Screen 1: Team name input -->
<div id="joinScreen">
    <h1>Квиз</h1>
    <p>Введите название вашей команды</p>
    <input type="text" id="teamNameInput" placeholder="Название команды" maxlength="40"
           onkeydown="if(event.key==='Enter') join()">
    <button id="joinBtn" onclick="join()">Присоединиться</button>
    <p id="joinError" style="color:#cc241d;display:none;margin-top:12px;"></p>
</div>

<!-- Screen 2: Quiz questions -->
<div id="quizScreen">
    <h2>Команда: <span id="teamNameDisplay"></span></h2>
    <div id="questionsContainer"></div>
</div>

<!-- Pause / timeout overlay -->
<div id="timerOverlay">
    <h1 id="overlayTitle">Пауза</h1>
    <p id="overlaySubtext">Приостановлено администратором, пожалуйста, дождитесь подолжения</p>
</div>

<!-- Reconnection banner -->
<div id="reconnectBanner" style="display:none;position:fixed;top:0;left:0;right:0;background:#b71c1c;color:#fff;padding:12px;text-align:center;font-size:14px;z-index:10000;"></div>

<script>
let ws = null;
let teamName = '';
let teamId = '';
let quizQuestions = [];
let reconnectAttempts = 0;
let reconnectTimer = null;

// getWsUrl {{{
function getWsUrl() {
    const host = window.location.hostname;
    const wsProtocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    return wsProtocol + '//' + host + ':8081';
}
// }}}
// connectWs {{{
function connectWs() {
    ws = new WebSocket(getWsUrl());

    ws.onopen = function() {
        if (reconnectAttempts > 0) {
            document.getElementById('reconnectBanner').style.display = 'none';
            reconnectAttempts = 0;
            ws.send(JSON.stringify({ type: 'join', name: teamName }));
        } else {
            ws.send(JSON.stringify({ type: 'join', name: teamName }));
        }
    };

    ws.onmessage = function(event) {
        try {
            const msg = JSON.parse(event.data);
            handleMessage(msg);
        } catch(e) {
            console.error('Message parse error:', e);
        }
    };

    ws.onerror = function() {
        if (!teamId && reconnectAttempts === 0) {
            document.getElementById('joinBtn').disabled = false;
            const error = document.getElementById('joinError');
            error.textContent = 'Не удалось подключиться к серверу';
            error.style.display = 'block';
        }
    };

    ws.onclose = function() {
        if (teamId && reconnectAttempts < 5) {
            scheduleReconnect();
        } else if (reconnectAttempts >= 5) {
            showReconnectBanner('Переподключение...');
        } else {
            console.log('WebSocket closed');
        }
    };
}
// }}}
// scheduleReconnect {{{
function scheduleReconnect() {
    reconnectAttempts++;
    var delay = Math.min(1000 * Math.pow(2, reconnectAttempts - 1), 16000);
    showReconnectBanner('Переподключение... (попытка ' + reconnectAttempts + '/5)');
    if (reconnectTimer) clearTimeout(reconnectTimer);
    reconnectTimer = setTimeout(function() {
        connectWs();
    }, delay);
}
// }}}
// showReconnectBanner {{{
function showReconnectBanner(text) {
    var banner = document.getElementById('reconnectBanner');
    banner.textContent = text;
    banner.style.display = 'block';
}
// }}}
// join {{{
function join() {
    const name = document.getElementById('teamNameInput').value.trim();
    if (!name) {
        const error = document.getElementById('joinError');
        error.textContent = 'Введите название вашей команды';
        error.style.display = 'block';
        return;
    }

    document.getElementById('joinBtn').disabled = true;
    document.getElementById('joinError').style.display = 'none';
    teamName = name;

    connectWs();
}
// }}}
// handleMessage {{{
function handleMessage(msg) {
    switch (msg.type) {
        case 'joined':
            teamId = msg.id;
            break;

        case 'quiz':
            quizQuestions = msg.questions || [];
            showQuiz();
            break;

        case 'result':
            updateQuestionStatus(msg.questionId, msg.status);
            break;

        case 'timer':
            handleTimerAction(msg.action);
            break;

        default:
            console.log('Unknown message:', msg);
    }
}
// }}}
// showQuiz {{{
function showQuiz() {
    document.getElementById('joinScreen').style.display = 'none';
    document.getElementById('quizScreen').style.display = 'block';
    document.getElementById('teamNameDisplay').textContent = teamName;

    const container = document.getElementById('questionsContainer');
    container.innerHTML = '';

    quizQuestions.forEach(function(q, idx) {
        const card = document.createElement('div');
        card.className = 'question-card';
        card.id = 'question-' + q.id;
        if (q.text.indexOf('$$') !== -1) {
            card.classList.add('has-math');
        }

        const text = document.createElement('div');
        text.className = 'question-text';
        text.textContent = (idx + 1) + '. ' + q.text;
        card.appendChild(text);

        if (q.type === 'single') {
            q.options.forEach(function(opt, oi) {
                const div = document.createElement('div');
                div.className = 'option';
                div.textContent = opt;
                div.dataset.value = opt;

                div.onpointerdown = function(e) {
                    e.preventDefault();
                    card.querySelectorAll('.option').forEach(function(o) {
                        o.classList.remove('selected');
                    });
                    div.classList.add('selected');
                };

                card.appendChild(div);
            });
        } else if (q.type === 'multiple') {
            q.options.forEach(function(opt, oi) {
                const div = document.createElement('div');
                div.className = 'option';
                div.textContent = opt;
                div.dataset.value = opt;

                div.onpointerdown = function(e) {
                    e.preventDefault();
                    div.classList.toggle('selected');
                };

                card.appendChild(div);
            });
        } else if (q.type === 'text') {
            const textarea = document.createElement('textarea');
            textarea.rows = 3;
            textarea.placeholder = 'Введите текст вручную...';
            textarea.id = 'text-answer-' + q.id;
            textarea.style.width = '100%';
            card.appendChild(textarea);
        }

        const status = document.createElement('div');
        status.className = 'answer-status';
        status.id = 'status-' + q.id;
        card.appendChild(status);

        const btn = document.createElement('button');
        btn.className = 'submit-answer';
        btn.textContent = 'Ответить';
        btn.id = 'submit-btn-' + q.id;
        btn.onclick = function() { submitAnswer(q.id, q.type); };
        card.appendChild(btn);

        container.appendChild(card);
    });

    // Render LaTeX math in questions
    if (typeof renderMathInElement === 'function') {
        try {
            renderMathInElement(container, {
                delimiters: [{left: '$$', right: '$$', display: true}],
                throwOnError: false
            });
        } catch(e) { console.warn('Math render error:', e); }
    }
}
// }}}
// submitAnswer {{{
function submitAnswer(questionId, questionType) {
    const btn = document.getElementById('submit-btn-' + questionId);
    btn.disabled = true;
    btn.textContent = 'Отправка...';

    let answers = [];

    if (questionType === 'single') {
        const selected = document.querySelectorAll(
            '#question-' + questionId + ' .option.selected');
        selected.forEach(function(div) {
            answers.push(div.dataset.value);
        });
    } else if (questionType === 'multiple') {
        const selected = document.querySelectorAll(
            '#question-' + questionId + ' .option.selected');
        selected.forEach(function(div) {
            answers.push(div.dataset.value);
        });
    } else if (questionType === 'text') {
        const textarea = document.getElementById('text-answer-' + questionId);
        if (textarea && textarea.value.trim()) {
            answers.push(textarea.value.trim());
        }
    }

    if (answers.length === 0) {
        btn.disabled = false;
        btn.textContent = 'Ответить';
        return;
    }

    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({
            type: 'answer',
            questionId: questionId,
            answers: answers
        }));
        btn.textContent = 'Отправлено';
    } else {
        btn.disabled = false;
        btn.textContent = 'Ошибка соединения';
    }
}
// }}}
// updateQuestionStatus {{{
function updateQuestionStatus(questionId, status) {
    const card = document.getElementById('question-' + questionId);
    if (!card) return;

    card.classList.add('sent');

    const statusDiv = document.getElementById('status-' + questionId);
    if (statusDiv) {
        statusDiv.textContent = '\u2713 Отправлено';
        statusDiv.className = 'answer-sent visible';
    }
}
// }}}
// handleTimerAction {{{
function handleTimerAction(action) {
    const overlay = document.getElementById('timerOverlay');
    const title = document.getElementById('overlayTitle');
    const subtext = document.getElementById('overlaySubtext');

    if (action === 'pause') {
        title.textContent = 'ПАУЗА';
        subtext.textContent = 'Подождите, администратор скоро продолжит';
        overlay.style.display = 'flex';
    } else if (action === 'resume' || action === 'reset') {
        overlay.style.display = 'none';
    } else if (action === 'timeout') {
        title.textContent = 'ВРЕМЯ ВЫШЛО';
        subtext.textContent = 'Квиз завершён. Спасибо за участие!';
        overlay.style.display = 'flex';
    }
}
// }}}

</script>
</body>
</html>

)raw");
    return html;
}


#endif
