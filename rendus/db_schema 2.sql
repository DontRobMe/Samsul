CREATE TABLE players (
  player_id      VARCHAR(16) PRIMARY KEY,
  name           TEXT NOT NULL,
  age            INT,
  email          TEXT NOT NULL,
  password       VARCHAR(255)
);

CREATE TABLE tables_foos (
  table_id       VARCHAR(8) PRIMARY KEY,
  location       TEXT,
  "condition"    TEXT,           -- "condition" est un mot réservé en SQL
  ball_type      TEXT
);

CREATE TABLE games (
  game_id        VARCHAR(16) PRIMARY KEY,
  game_datetime  TIMESTAMPTZ,
  table_id       VARCHAR(8) REFERENCES tables_foos(table_id) ON DELETE SET NULL,
  score_red      INT,
  score_blue     INT,
  winner         TEXT CHECK (winner IN ('Red','Blue')),
  duration_s     INT,
  attendance     INT,
  season         TEXT
);

CREATE INDEX idx_games_datetime ON games (game_datetime);
CREATE INDEX idx_games_winner ON games (winner);
CREATE INDEX idx_games_table  ON games (table_id);

CREATE TABLE game_players (
  game_id        VARCHAR(16) REFERENCES games(game_id) ON DELETE CASCADE,
  player_id      VARCHAR(16) REFERENCES players(player_id) ON DELETE CASCADE,
  team           TEXT CHECK (team IN ('Red','Blue')),
  player_role    TEXT CHECK (player_role IN ('Attack','Defense')),
  goals          INT DEFAULT 0,
  is_substitute  BOOLEAN DEFAULT FALSE,
  PRIMARY KEY (game_id, player_id, team)
);

CREATE TABLE telemetry (
  id             SERIAL PRIMARY KEY,
  game_id        VARCHAR(16) REFERENCES games(game_id) ON DELETE CASCADE,
  ping_ms        INT,
  created_dt     TIMESTAMPTZ DEFAULT now()
);

CREATE TABLE bettors (
  bettor_id      SERIAL PRIMARY KEY,
  player_id      VARCHAR(16) REFERENCES players(player_id),
  display_name   TEXT,
  balance_cents  BIGINT NOT NULL DEFAULT 0,
  created_at     TIMESTAMPTZ DEFAULT now()
);

CREATE TABLE bet_types (
  bet_type_id    VARCHAR(32) PRIMARY KEY,
  description    TEXT
);

INSERT INTO bet_types (bet_type_id, description) VALUES
  ('moneyline', 'Parier sur le vainqueur : Red ou Blue'),
  ('total_over_under', 'Parier si total de buts est Over/Under un seuil'),
  ('player_prop_goals', 'Parier sur le nombre de buts du joueur');

CREATE TABLE bets (
  bet_id         VARCHAR(32) PRIMARY KEY,
  bettor_id      INT REFERENCES bettors(bettor_id) ON DELETE CASCADE,
  game_id        VARCHAR(16) REFERENCES games(game_id) ON DELETE CASCADE,
  bet_type_id    VARCHAR(32) REFERENCES bet_types(bet_type_id),
  stake_cents    BIGINT NOT NULL CHECK (stake_cents > 0),
  odds_num       BIGINT NOT NULL,
  odds_den       BIGINT NOT NULL,
  placed_at      TIMESTAMPTZ DEFAULT now(),
  settled        BOOLEAN DEFAULT FALSE,
  won            BOOLEAN,
  payout_cents   BIGINT DEFAULT 0,
  CONSTRAINT odds_positive CHECK (odds_num > 0 AND odds_den > 0)
);

CREATE TABLE bet_selections (
  selection_id   SERIAL PRIMARY KEY,
  bet_id         VARCHAR(32) REFERENCES bets(bet_id) ON DELETE CASCADE,
  selection_key  TEXT NOT NULL,
  selection_value TEXT,
  odds_num       BIGINT NOT NULL,
  odds_den       BIGINT NOT NULL
);

CREATE INDEX idx_bets_game ON bets(game_id);
CREATE INDEX idx_bets_bettor ON bets(bettor_id);
CREATE INDEX idx_bet_selections_bet ON bet_selections(bet_id);

CREATE OR REPLACE FUNCTION settle_moneyline_bets(p_game_id VARCHAR)
RETURNS VOID AS $$
DECLARE
  r RECORD;
  sel RECORD;
  winner_text TEXT;
  payout BIGINT;
BEGIN
  SELECT winner INTO winner_text FROM games WHERE game_id = p_game_id;
  IF winner_text IS NULL THEN
    RAISE NOTICE 'Game % has no winner set, skipping settle.', p_game_id;
    RETURN;
  END IF;

  FOR r IN
    SELECT * FROM bets WHERE game_id = p_game_id AND settled = FALSE
  LOOP
    SELECT bs.* INTO sel
    FROM bet_selections bs
    WHERE bs.bet_id = r.bet_id AND bs.selection_key LIKE 'winner:%'
    LIMIT 1;

    IF NOT FOUND THEN
      UPDATE bets SET settled = TRUE, won = NULL WHERE bet_id = r.bet_id;
      CONTINUE;
    END IF;

    IF sel.selection_key = concat('winner:', winner_text) THEN
      payout := (r.stake_cents * sel.odds_num) / sel.odds_den;
      UPDATE bets
        SET settled = TRUE, won = TRUE, payout_cents = payout
        WHERE bet_id = r.bet_id;
      UPDATE bettors
        SET balance_cents = balance_cents + payout
        WHERE bettor_id = r.bettor_id;
    ELSE
      UPDATE bets
        SET settled = TRUE, won = FALSE, payout_cents = 0
        WHERE bet_id = r.bet_id;
    END IF;
  END LOOP;
END;
