require('dotenv').config();
const { Pool } = require('pg');

const wantsSSL = String(process.env.PGSSL || '').toLowerCase();
const ssl =
  wantsSSL === 'true' || wantsSSL === 'require'
    ? { rejectUnauthorized: false }
    : false;

const pool = new Pool({
  host: process.env.PGHOST,
  port: Number(process.env.PGPORT || 5432),
  user: process.env.PGUSER,
  password: process.env.PGPASSWORD,
  database: process.env.PGDATABASE,
  ssl,
  connectionTimeoutMillis: 8000,
  idleTimeoutMillis: 10000,
  max: 10,
});

pool.on('error', (err) => {
  console.error('[PG] Idle client error:', err.message);
});

async function ping() {
  try {
    const { rows } = await pool.query('SELECT 1 AS ok');
    return rows?.[0]?.ok === 1;
  } catch (e) {
    console.error('[PG] Ping error:', e.message);
    return false;
  }
}

async function closePool() {
  try {
    await pool.end();
    console.log('[PG] Pool closed');
  } catch (e) {
    console.error('[PG] Error closing pool:', e.message);
  }
}

(async () => {
  try {
    await pool.query('SELECT 1');
    console.log('✅ PostgreSQL OK');
  } catch (err) {
    console.error('❌ PostgreSQL KO:', err.message);
  }
})();

module.exports = { pool, ping, closePool };
