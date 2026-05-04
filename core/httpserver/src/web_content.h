#ifndef WEB_CONTENT_H
#define WEB_CONTENT_H


// HTML-страница веб-клиента квиза
// Содержит форму ввода имени команды и интерфейс для ответов на вопросы
// Встроена в C++ как сырая строка, отдаётся через HttpServer на GET /
static const char *webContentHtml = R"raw(
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<title>Квиз</title>
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

/* Экран ввода имени */
#joinScreen {
    width: 100%;
    max-width: 400px;
    text-align: center;
    margin-top: 80px;
}

/* Экран квиза (скрыт изначально) */
#quizScreen {
    width: 100%;
    max-width: 500px;
    display: none;
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

/* Карточка вопроса */
.question-card {
    background: #282828;
    border-radius: 12px;
    padding: 16px;
    margin: 12px 0;
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

/* Радио-кнопки и чекбоксы как блоки */
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

/* Сообщение после отправки ответа (просто "Отправлено", без верно/неверно) */
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

/* Блокируем взаимодействие после отправки */
.question-card.sent .option {
    opacity: 0.6;
    pointer-events: none;
}
.question-card.sent .submit-answer {
    display: none;
}

/* Оверлей блокировки (пауза/таймаут) */
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

/* Кнопка отправки ответа для каждого вопроса */
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
<!-- Экран 1: Ввод названия команды -->
<div id="joinScreen">
    <h1>Квиз</h1>
    <p>Введите название вашей команды</p>
    <input type="text" id="teamNameInput" placeholder="Название команды" maxlength="40"
           onkeydown="if(event.key==='Enter') join()">
    <button id="joinBtn" onclick="join()">Присоединиться</button>
    <p id="joinError" style="color:#cc241d;display:none;margin-top:12px;"></p>
</div>

<!-- Экран 2: Вопросы квиза -->
<div id="quizScreen">
    <h2>Команда: <span id="teamNameDisplay"></span></h2>
    <p>Отвечайте на вопросы по одному</p>
    <div id="questionsContainer"></div>
</div>

<!-- Оверлей паузы / время вышло -->
<div id="timerOverlay">
    <h1 id="overlayTitle">ПАУЗА</h1>
    <p id="overlaySubtext">Подождите, администратор скоро продолжит</p>
</div>

<script>
let ws = null;
let teamName = '';
let teamId = '';
let quizQuestions = [];

// Функция входа: соединяемся через WebSocket и отправляем join
function join() {
    const name = document.getElementById('teamNameInput').value.trim();
    if (!name) {
        const error = document.getElementById('joinError');
        error.textContent = 'Введите название команды';
        error.style.display = 'block';
        return;
    }

    document.getElementById('joinBtn').disabled = true;
    document.getElementById('joinError').style.display = 'none';
    teamName = name;

    // Определяем адрес WebSocket-сервера (тот же хост, порт 8081)
    const host = window.location.hostname;
    const wsProtocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = wsProtocol + '//' + host + ':8081';

    ws = new WebSocket(wsUrl);

    ws.onopen = function() {
        // Отправляем запрос на присоединение
        ws.send(JSON.stringify({
            type: 'join',
            name: teamName
        }));
    };

    ws.onmessage = function(event) {
        try {
            const msg = JSON.parse(event.data);
            handleMessage(msg);
        } catch(e) {
            console.error('Ошибка парсинга сообщения:', e);
        }
    };

    ws.onerror = function() {
        document.getElementById('joinBtn').disabled = false;
        const error = document.getElementById('joinError');
        error.textContent = 'Не удалось подключиться к серверу';
        error.style.display = 'block';
    };

    ws.onclose = function() {
        console.log('WebSocket закрыт');
    };
}

// Обработка входящих сообщений от сервера
function handleMessage(msg) {
    switch (msg.type) {
        case 'joined':
            // Подтверждение регистрации
            teamId = msg.id;
            break;

        case 'quiz':
            // Получили вопросы — показываем экран квиза
            quizQuestions = msg.questions || [];
            showQuiz();
            break;

        case 'result':
            // Результат ответа на вопрос
            updateQuestionStatus(msg.questionId, msg.status);
            break;

        case 'timer':
            // Таймер: пауза / возобновление / время вышло
            handleTimerAction(msg.action);
            break;

        default:
            console.log('Неизвестное сообщение:', msg);
    }
}

// Показываем экран с вопросами
function showQuiz() {
    document.getElementById('joinScreen').style.display = 'none';
    document.getElementById('quizScreen').style.display = 'block';
    document.getElementById('teamNameDisplay').textContent = teamName;

    const container = document.getElementById('questionsContainer');
    container.innerHTML = '';

    // Рендерим каждый вопрос
    quizQuestions.forEach(function(q, idx) {
        const card = document.createElement('div');
        card.className = 'question-card';
        card.id = 'question-' + q.id;

        // Текст вопроса
        const text = document.createElement('div');
        text.className = 'question-text';
        text.textContent = (idx + 1) + '. ' + q.text;
        card.appendChild(text);

        // Варианты ответа в зависимости от типа
        if (q.type === 'single') {
            // Радио-кнопки — используем div с pointerdown, не label
            q.options.forEach(function(opt, oi) {
                const div = document.createElement('div');
                div.className = 'option';
                div.textContent = opt;
                // Храним значение в data-атрибуте
                div.dataset.value = opt;

                div.onpointerdown = function(e) {
                    e.preventDefault();
                    // Снимаем выделение со всех опций в этом вопросе
                    card.querySelectorAll('.option').forEach(function(o) {
                        o.classList.remove('selected');
                    });
                    div.classList.add('selected');
                };

                card.appendChild(div);
            });
        } else if (q.type === 'multiple') {
            // Чекбоксы — используем div с pointerdown
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
            // Текстовое поле
            const textarea = document.createElement('textarea');
            textarea.rows = 3;
            textarea.placeholder = 'Введите ответ...';
            textarea.id = 'text-answer-' + q.id;
            textarea.style.width = '100%';
            card.appendChild(textarea);
        }

        // Статус ответа (пока скрыт)
        const status = document.createElement('div');
        status.className = 'answer-status';
        status.id = 'status-' + q.id;
        card.appendChild(status);

        // Кнопка "Отправить ответ"
        const btn = document.createElement('button');
        btn.className = 'submit-answer';
        btn.textContent = 'Ответить';
        btn.id = 'submit-btn-' + q.id;
        btn.onclick = function() { submitAnswer(q.id, q.type); };
        card.appendChild(btn);

        container.appendChild(card);
    });
}

// Отправка ответа на вопрос
function submitAnswer(questionId, questionType) {
    const btn = document.getElementById('submit-btn-' + questionId);
    btn.disabled = true;
    btn.textContent = 'Отправка...';

    let answers = [];

    if (questionType === 'single') {
        // Собираем выбранный вариант (div с классом selected)
        const selected = document.querySelectorAll(
            '#question-' + questionId + ' .option.selected');
        selected.forEach(function(div) {
            answers.push(div.dataset.value);
        });
    } else if (questionType === 'multiple') {
        // Собираем все выбранные варианты (div с классом selected)
        const selected = document.querySelectorAll(
            '#question-' + questionId + ' .option.selected');
        selected.forEach(function(div) {
            answers.push(div.dataset.value);
        });
    } else if (questionType === 'text') {
        // Текст из textarea
        const textarea = document.getElementById('text-answer-' + questionId);
        if (textarea && textarea.value.trim()) {
            answers.push(textarea.value.trim());
        }
    }

    if (answers.length === 0) {
        btn.disabled = false;
        btn.textContent = 'Ответить';
        return; // Ничего не выбрано
    }

    // Отправляем на сервер
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

// Обновление статуса вопроса после ответа
// Клиент не видит верно/неверно — только "Отправлено"
function updateQuestionStatus(questionId, status) {
    const card = document.getElementById('question-' + questionId);
    if (!card) return;

    // Блокируем вопрос (убираем возможность менять ответ)
    card.classList.add('sent');

    // Показываем "Отправлено"
    const statusDiv = document.getElementById('status-' + questionId);
    if (statusDiv) {
        statusDiv.textContent = '✓ Отправлено';
        statusDiv.className = 'answer-sent visible';
    }
}

// Обработка действий таймера от админа
function handleTimerAction(action) {
    const overlay = document.getElementById('timerOverlay');
    const title = document.getElementById('overlayTitle');
    const subtext = document.getElementById('overlaySubtext');

    if (action === 'pause') {
        title.textContent = 'ПАУЗА';
        subtext.textContent = 'Подождите, администратор скоро продолжит';
        overlay.style.display = 'flex';
    } else if (action === 'resume') {
        overlay.style.display = 'none';
    } else if (action === 'timeout') {
        title.textContent = 'ВРЕМЯ ВЫШЛО';
        subtext.textContent = 'Квиз завершён. Спасибо за участие!';
        overlay.style.display = 'flex';
    }
}
</script>
</body>
</html>
)raw";


#endif
