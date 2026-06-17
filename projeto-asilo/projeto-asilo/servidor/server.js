const express = require('express');
const path = require('path');

const app = express();
const PORT = 3000;

app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

let alertas = [];

app.post('/api/chamada', (req, res) => {
  const { tipo, dispositivo } = req.body;

  if (!tipo || !dispositivo) {
    return res.status(400).json({ erro: 'Campos "tipo" e "dispositivo" sao obrigatorios.' });
  }

  const alerta = {
    id: Date.now(),
    tipo: tipo.toUpperCase(),       
    dispositivo,                    
    horario: new Date().toLocaleString('pt-BR')
  };

  alertas.unshift(alerta);
  alertas = alertas.slice(0, 50); 

  console.log(`Novo alerta: [${alerta.tipo}] - ${alerta.dispositivo} - ${alerta.horario}`);
  res.status(201).json({ status: 'ok', alerta });
});

app.get('/api/chamadas', (req, res) => {
  res.json(alertas);
});

app.delete('/api/chamada/:id', (req, res) => {
  const id = Number(req.params.id);
  alertas = alertas.filter(a => a.id !== id);
  res.json({ status: 'ok' });
});

app.listen(PORT, () => {
  console.log(`Servidor de notificacoes rodando em http://localhost:${PORT}`);
});
