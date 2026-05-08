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
