// ============================================
// SOLO SYSTEM - SISTEMA DE RPG PESSOAL
// ============================================

// Estado do Jogador
let gameState = {
    playerName: '',
    level: 1,
    xp: 0,
    maxXp: 100,
    availablePoints: 0,
    attributes: {
        STR: 10,
        VIT: 10,
        AGI: 10,
        INT: 10,
        SEN: 10
    },
    quests: {
        pushups: 0,
        situps: 0,
        squats: 0,
        running: 0
    },
    rank: 'E-RANK'
};

// Gráfico de Radar
let radarChart = null;

// Sistema de Ranks
const RANKS = [
    'E-RANK', 'D-RANK', 'C-RANK', 'B-RANK', 'A-RANK', 
    'S-RANK', 'SS-RANK', 'SSS-RANK', 'MONARCH', 'SOVEREIGN'
];

// Sons (usando Web Audio API)
const audioContext = new (window.AudioContext || window.webkitAudioContext)();

function playMetallicClick() {
    const oscillator = audioContext.createOscillator();
    const gainNode = audioContext.createGain();
    
    oscillator.connect(gainNode);
    gainNode.connect(audioContext.destination);
    
    oscillator.frequency.value = 800;
    oscillator.type = 'square';
    
    gainNode.gain.setValueAtTime(0.3, audioContext.currentTime);
    gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.1);
    
    oscillator.start(audioContext.currentTime);
    oscillator.stop(audioContext.currentTime + 0.1);
}

function playLevelUp() {
    const frequencies = [523.25, 659.25, 783.99, 1046.50]; // Notas musicais
    
    frequencies.forEach((freq, index) => {
        const oscillator = audioContext.createOscillator();
        const gainNode = audioContext.createGain();
        
        oscillator.connect(gainNode);
        gainNode.connect(audioContext.destination);
        
        oscillator.frequency.value = freq;
        oscillator.type = 'sine';
        
        const startTime = audioContext.currentTime + (index * 0.1);
        gainNode.gain.setValueAtTime(0, startTime);
        gainNode.gain.linearRampToValueAtTime(0.3, startTime + 0.05);
        gainNode.gain.exponentialRampToValueAtTime(0.01, startTime + 0.5);
        
        oscillator.start(startTime);
        oscillator.stop(startTime + 0.5);
    });
}

function playQuestCleared() {
    const frequencies = [659.25, 783.99, 987.77, 1318.51];
    
    frequencies.forEach((freq, index) => {
        const oscillator = audioContext.createOscillator();
        const gainNode = audioContext.createGain();
        
        oscillator.connect(gainNode);
        gainNode.connect(audioContext.destination);
        
        oscillator.frequency.value = freq;
        oscillator.type = 'sine';
        
        const startTime = audioContext.currentTime + (index * 0.15);
        gainNode.gain.setValueAtTime(0, startTime);
        gainNode.gain.linearRampToValueAtTime(0.4, startTime + 0.05);
        gainNode.gain.exponentialRampToValueAtTime(0.01, startTime + 0.8);
        
        oscillator.start(startTime);
        oscillator.stop(startTime + 0.8);
    });
}

// Funções de LocalStorage
function saveGameState() {
    localStorage.setItem('soloSystemState', JSON.stringify(gameState));
}

function loadGameState() {
    const saved = localStorage.getItem('soloSystemState');
    if (saved) {
        gameState = JSON.parse(saved);
        return true;
    }
    return false;
}

// Tela de Awakening
function initAwakening() {
    const nameInput = document.getElementById('player-name-input');
    const awakeningBtn = document.getElementById('awakening-btn');
    
    // Verificar se já existe um nome salvo
    if (gameState.playerName) {
        // Pular tela de awakening se já tem nome
        showDashboard();
        return;
    }
    
    nameInput.addEventListener('keypress', (e) => {
        if (e.key === 'Enter') {
            awakeningBtn.click();
        }
    });
    
    awakeningBtn.addEventListener('click', () => {
        const name = nameInput.value.trim();
        if (name === '') {
            alert('DIGITE SEU NOME PARA CONTINUAR');
            return;
        }
        
        playMetallicClick();
        gameState.playerName = name.toUpperCase();
        saveGameState();
        showDashboard();
    });
}

function showDashboard() {
    document.getElementById('awakening-screen').classList.add('hidden');
    document.getElementById('dashboard').classList.remove('hidden');
    updateDashboard();
    initRadarChart();
}

// Atualizar Dashboard
function updateDashboard() {
    // Nome do jogador
    document.getElementById('player-name-display').textContent = gameState.playerName;
    
    // Level e Rank
    document.getElementById('level-display').textContent = gameState.level;
    document.getElementById('rank-display').textContent = gameState.rank;
    
    // XP
    const xpPercentage = (gameState.xp / gameState.maxXp) * 100;
    document.getElementById('exp-bar').style.width = xpPercentage + '%';
    document.getElementById('current-xp').textContent = gameState.xp;
    document.getElementById('max-xp').textContent = gameState.maxXp;
    
    // Atributos
    document.getElementById('attr-str').textContent = gameState.attributes.STR;
    document.getElementById('attr-vit').textContent = gameState.attributes.VIT;
    document.getElementById('attr-agi').textContent = gameState.attributes.AGI;
    document.getElementById('attr-int').textContent = gameState.attributes.INT;
    document.getElementById('attr-sen').textContent = gameState.attributes.SEN;
    
    // Pontos disponíveis
    const pointsContainer = document.getElementById('points-available-container');
    const availablePointsSpan = document.getElementById('available-points');
    
    if (gameState.availablePoints > 0) {
        pointsContainer.classList.remove('hidden');
        availablePointsSpan.textContent = gameState.availablePoints;
        
        // Mostrar botões de atributo
        document.querySelectorAll('.attr-btn').forEach(btn => {
            btn.classList.remove('hidden');
        });
    } else {
        pointsContainer.classList.add('hidden');
        document.querySelectorAll('.attr-btn').forEach(btn => {
            btn.classList.add('hidden');
        });
    }
    
    // Missões
    updateQuestDisplay('pushups', gameState.quests.pushups, 100);
    updateQuestDisplay('situps', gameState.quests.situps, 100);
    updateQuestDisplay('squats', gameState.quests.squats, 100);
    updateQuestDisplay('running', gameState.quests.running, 10);
    
    // Verificar se todas as missões estão completas
    checkAllQuestsComplete();
    
    // Atualizar gráfico de radar
    if (radarChart) {
        updateRadarChart();
    }
}

function updateQuestDisplay(questName, current, max) {
    const progressElement = document.getElementById(`quest-${questName}`);
    const barElement = document.getElementById(`quest-bar-${questName}`);
    
    let percentage = 0;
    if (max > 0) {
        percentage = Math.min((current / max) * 100, 100);
    }
    
    if (questName === 'running') {
        progressElement.textContent = `${current.toFixed(1)} / ${max.toFixed(1)} km`;
    } else {
        progressElement.textContent = `${Math.floor(current)} / ${max}`;
    }
    
    barElement.style.width = percentage + '%';
}

function checkAllQuestsComplete() {
    const pushupsComplete = gameState.quests.pushups >= 100;
    const situpsComplete = gameState.quests.situps >= 100;
    const squatsComplete = gameState.quests.squats >= 100;
    const runningComplete = gameState.quests.running >= 10;
    
    const claimBtn = document.getElementById('claim-reward-btn');
    
    if (pushupsComplete && situpsComplete && squatsComplete && runningComplete) {
        claimBtn.classList.remove('hidden');
    } else {
        claimBtn.classList.add('hidden');
    }
}

// Sistema de Atributos
function increaseAttribute(attrName) {
    if (gameState.availablePoints <= 0) {
        return;
    }
    
    playMetallicClick();
    gameState.attributes[attrName]++;
    gameState.availablePoints--;
    saveGameState();
    updateDashboard();
}

// Sistema de XP e Level Up
function gainXP(amount) {
    gameState.xp += amount;
    
    while (gameState.xp >= gameState.maxXp) {
        gameState.xp -= gameState.maxXp;
        levelUp();
    }
    
    // Aumentar XP necessário para próximo nível (50% a mais)
    if (gameState.xp === 0) {
        gameState.maxXp = Math.floor(gameState.maxXp * 1.5);
    }
    
    saveGameState();
    updateDashboard();
}

function levelUp() {
    playLevelUp();
    gameState.level++;
    gameState.availablePoints += 5; // Ganha 5 pontos de atributo
    
    // Atualizar rank baseado no nível
    updateRank();
    
    // Animação de level up (opcional)
    showLevelUpAnimation();
}

function updateRank() {
    if (gameState.level < 5) {
        gameState.rank = 'E-RANK';
    } else if (gameState.level < 10) {
        gameState.rank = 'D-RANK';
    } else if (gameState.level < 20) {
        gameState.rank = 'C-RANK';
    } else if (gameState.level < 30) {
        gameState.rank = 'B-RANK';
    } else if (gameState.level < 40) {
        gameState.rank = 'A-RANK';
    } else if (gameState.level < 50) {
        gameState.rank = 'S-RANK';
    } else if (gameState.level < 60) {
        gameState.rank = 'SS-RANK';
    } else if (gameState.level < 70) {
        gameState.rank = 'SSS-RANK';
    } else if (gameState.level < 80) {
        gameState.rank = 'MONARCH';
    } else {
        gameState.rank = 'SOVEREIGN';
    }
}

function showLevelUpAnimation() {
    // Criar elemento temporário para animação
    const levelUpText = document.createElement('div');
    levelUpText.textContent = 'LEVEL UP!';
    levelUpText.style.cssText = `
        position: fixed;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
        font-size: 4rem;
        font-weight: 900;
        color: #4deeea;
        text-shadow: 0 0 20px #4deeea, 0 0 40px #4deeea, 0 0 60px #4deeea;
        letter-spacing: 10px;
        z-index: 10000;
        pointer-events: none;
        animation: levelUpAnimation 2s ease-out forwards;
    `;
    
    document.body.appendChild(levelUpText);
    
    setTimeout(() => {
        levelUpText.remove();
    }, 2000);
}

// Adicionar animação CSS para level up
const style = document.createElement('style');
style.textContent = `
    @keyframes levelUpAnimation {
        0% {
            opacity: 0;
            transform: translate(-50%, -50%) scale(0.5);
        }
        50% {
            opacity: 1;
            transform: translate(-50%, -50%) scale(1.2);
        }
        100% {
            opacity: 0;
            transform: translate(-50%, -50%) scale(1);
        }
    }
`;
document.head.appendChild(style);

// Sistema de Missões
function updateQuest(questName, amount) {
    playMetallicClick();
    
    if (questName === 'running') {
        gameState.quests.running = Math.min(gameState.quests.running + amount, 10);
    } else {
        gameState.quests[questName] = Math.min(gameState.quests[questName] + amount, 100);
    }
    
    saveGameState();
    updateDashboard();
}

function claimReward() {
    playQuestCleared();
    
    // Mostrar animação QUEST CLEARED
    const animation = document.getElementById('quest-cleared-animation');
    animation.classList.remove('hidden');
    
    // Ganhar XP
    gainXP(200);
    
    // Resetar missões
    setTimeout(() => {
        gameState.quests = {
            pushups: 0,
            situps: 0,
            squats: 0,
            running: 0
        };
        saveGameState();
        updateDashboard();
        
        // Esconder animação
        animation.classList.add('hidden');
    }, 3000);
}

// Gráfico de Radar
function initRadarChart() {
    const ctx = document.getElementById('radar-chart');
    if (!ctx) return;
    
    const attributes = gameState.attributes;
    
    radarChart = new Chart(ctx, {
        type: 'radar',
        data: {
            labels: ['STR', 'VIT', 'AGI', 'INT', 'SEN'],
            datasets: [{
                label: 'Atributos',
                data: [
                    attributes.STR,
                    attributes.VIT,
                    attributes.AGI,
                    attributes.INT,
                    attributes.SEN
                ],
                backgroundColor: 'rgba(77, 238, 234, 0.2)',
                borderColor: '#4deeea',
                borderWidth: 3,
                pointBackgroundColor: '#4deeea',
                pointBorderColor: '#ffffff',
                pointHoverBackgroundColor: '#ffffff',
                pointHoverBorderColor: '#4deeea',
                pointRadius: 6,
                pointHoverRadius: 8
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: true,
            plugins: {
                legend: {
                    display: false
                },
                tooltip: {
                    backgroundColor: 'rgba(5, 10, 15, 0.9)',
                    titleColor: '#4deeea',
                    bodyColor: '#4deeea',
                    borderColor: '#4deeea',
                    borderWidth: 2,
                    padding: 15,
                    boxShadow: '0 0 10px #4deeea',
                    font: {
                        family: "'Orbitron', monospace",
                        size: 14,
                        weight: 'bold'
                    }
                }
            },
            scales: {
                r: {
                    beginAtZero: true,
                    max: 50,
                    ticks: {
                        stepSize: 10,
                        color: '#4deeea',
                        font: {
                            family: "'Orbitron', monospace",
                            size: 12,
                            weight: 'bold'
                        },
                        backdropColor: 'rgba(5, 10, 15, 0.8)'
                    },
                    grid: {
                        color: 'rgba(77, 238, 234, 0.3)',
                        lineWidth: 1
                    },
                    pointLabels: {
                        color: '#4deeea',
                        font: {
                            family: "'Orbitron', monospace",
                            size: 14,
                            weight: 'bold'
                        },
                        padding: 15
                    },
                    angleLines: {
                        color: 'rgba(77, 238, 234, 0.3)',
                        lineWidth: 1
                    }
                }
            },
            animation: {
                duration: 500,
                easing: 'easeOutQuart'
            }
        }
    });
}

function updateRadarChart() {
    if (!radarChart) return;
    
    const attributes = gameState.attributes;
    radarChart.data.datasets[0].data = [
        attributes.STR,
        attributes.VIT,
        attributes.AGI,
        attributes.INT,
        attributes.SEN
    ];
    radarChart.update('active');
}

// Inicialização
document.addEventListener('DOMContentLoaded', () => {
    // Carregar estado salvo
    const hasSavedState = loadGameState();
    
    if (hasSavedState && gameState.playerName) {
        // Se já tem nome salvo, ir direto para o dashboard
        showDashboard();
    } else {
        // Caso contrário, mostrar tela de awakening
        initAwakening();
    }
    
    // Garantir que o contexto de áudio esteja iniciado (alguns navegadores exigem interação do usuário)
    document.addEventListener('click', () => {
        if (audioContext.state === 'suspended') {
            audioContext.resume();
        }
    }, { once: true });
});
