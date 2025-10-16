require('dotenv').config();
const express = require('express');
const cors = require('cors');

const app = express();
app.use(cors());            // autorise le front en dev
app.use(express.json());    // parse JSON

// Routes d'exemple
app.get('/api/health', (req, res) => {
  res.json({ ok: true, time: new Date().toISOString() });
});

app.get('/api/items', (req, res) => {
  res.json([
    { id: 1, name: 'Raquette' },
    { id: 2, name: 'Balles' },
  ]);
});

const port = process.env.PORT || 4000;
app.listen(port, () => console.log(`API on http://localhost:${port}`));
