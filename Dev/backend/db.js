// db.js (CommonJS)
require('dotenv').config();
const { Pool } = require('pg');

const ssl =
  String(process.env.PGSSL).toLowerCase() === 'true'
    ? { rejectUnauthorized: false }
    : false;

const pool = new Pool({
  host: process.env.PGHOST,
  port: Number(process.env.PGPORT || 5432),
  user: process.env.PGUSER,
  password: process.env.PGPASSWORD,
  database: process.env.PGDATABASE,
  ssl,
});

(async () => {
  try {
    await pool.query('SELECT 1');
    console.log('Connecté à PostgreSQL');
  } catch (err) {
    console.error('Connexion PostgreSQL échouée:', err.message);
  }
})();

module.exports = pool;
