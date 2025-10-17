require('dotenv').config();
const express = require('express');
const cors = require('cors');
const { pool, ping, closePool } = require('./db');

const app = express();
app.use(cors());
app.use(express.json());

app.get('/', (_req, res) => res.send('API up'));

app.get('/health/db', async (_req, res) => {
  try {
    const ok = await ping();
    res.json({ ok, status: ok ? 'up' : 'down' });
  } catch (e) {
    res.status(500).json({ ok: false, status: 'down', error: e.message });
  }
});

app.get('/now', async (_req, res) => {
  try {
    const { rows } = await pool.query('SELECT NOW() AS now');
    res.json(rows[0]);
  } catch (e) {
    res.status(500).json({ error: e.message });
  }
});

const PORT = Number(process.env.PORT || 4000);
const server = app.listen(PORT, async () => {
  console.log(`API running on http://localhost:${PORT}`);
  const ok = await ping();
  console.log(`[Startup] DB status: ${ok ? 'up' : 'down'}`);
});

async function graceful(signal) {
  console.log(`\n[${signal}] Shutting down...`);
  server.close(async () => {
    await closePool();
    process.exit(0);
  });
}
process.on('SIGINT', () => graceful('SIGINT'));
process.on('SIGTERM', () => graceful('SIGTERM'));