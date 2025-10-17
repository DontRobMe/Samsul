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

app.post('/realtime', async (req, res) => {
  const { event, player1_score, player2_score, elapsed_time } = req.body || {};
  if (event !== 'realtime') {
    return res.status(400).json({ error: 'event doit être "realtime".' });
  }

  const score_red = Number(player1_score ?? 0);
  const score_blue = Number(player2_score ?? 0);
  const duration_s = Math.max(0, Math.round(Number(elapsed_time ?? 0)));

  try {
    await pool.query(
      `
      INSERT INTO games (game_datetime, score_red, score_blue, duration_s)
      VALUES (NOW(), $1, $2, $3)
      `,
      [score_red, score_blue, duration_s]
    );

    return res.status(200).json({
      ok: true,
      score_red,
      score_blue,
      duration_s
    });
  } catch (e) {
    console.error('[POST /realtime] DB error:', e);
    return res.status(500).json({ error: 'Erreur serveur.' });
  }
});

app.post('/final', async (req, res) => {
  const { event, winner, player1_score, player2_score, total_time } = req.body || {};
  if (event !== 'final') {
    return res.status(400).json({ error: 'event doit être "final".' });
  }

  const winner_text = winnerToText(winner);
  if (!winner_text) {
    return res.status(400).json({ error: 'winner doit être 1 (Red) ou 2 (Blue).' });
  }

  const score_red = Number(player1_score ?? 0);
  const score_blue = Number(player2_score ?? 0);
  const duration_s = Math.max(0, Math.round(Number(total_time ?? 0)));

  if (
    (winner_text === 'Red' && score_red <= score_blue) ||
    (winner_text === 'Blue' && score_blue <= score_red)
  ) {
    console.warn('[final] Winner ne concorde pas avec les scores.', {
      winner_text, score_red, score_blue
    });
  }

  const client = await pool.connect();
  try {
    await client.query('BEGIN');

    await client.query(
      `
      INSERT INTO games (game_datetime, score_red, score_blue, winner, duration_s)
      VALUES (NOW(), $1, $2, $3, $4)
      `,
      [score_red, score_blue, winner_text, duration_s]
    );

    await client.query('SELECT settle_moneyline_bets($1)', [score_red > score_blue ? 'Red' : 'Blue']);

    await client.query('COMMIT');

    return res.status(200).json({
      ok: true,
      winner: winner_text,
      score_red,
      score_blue,
      duration_s
    });
  } catch (e) {
    await client.query('ROLLBACK');
    console.error('[POST /api/events/final] DB error:', e);
    return res.status(500).json({ error: 'Erreur serveur.' });
  } finally {
    client.release();
  }
});

// Stockage en mémoire des réservations avec date et slot
const calendarReservations = [];

// Récupérer tous les créneaux réservés
app.get('/api/calendar/reservations', (req, res) => {
  res.json(calendarReservations);
});

// Réserver un créneau (date au format YYYY-MM-DD, slot = index 15min)
app.post('/api/calendar/reserve', (req, res) => {
  const { date, slot } = req.body || {};
  // Validation
  if (typeof date !== 'string' || !/^\d{4}-\d{2}-\d{2}$/.test(date) || typeof slot !== 'number') {
    return res.status(400).json({ error: 'Paramètres invalides (date ou slot)' });
  }
  // Vérifier si le créneau est déjà réservé
  if (calendarReservations.some(r => r.date === date && r.slot === slot)) {
    return res.status(409).json({ error: 'Créneau déjà réservé pour cette date' });
  }
  calendarReservations.push({ date, slot });
  res.json({ success: true });
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