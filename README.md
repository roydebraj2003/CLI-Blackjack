# CLI Blackjack

Welcome to **CLI Blackjack** — a command-line implementation of the classic casino game **Blackjack**! This application uses a PostgreSQL database to manage user authentication, track balances, and maintain leaderboards.

---

## Features

### Realistic Blackjack Gameplay
- Uses **6 decks of cards** (312 cards total).
- Properly shuffles all decks before each game session.
- Draws cards from the **top of the deck** — just like in a real casino.
- Implements core Blackjack rules: hit, stand, double down, split (coming soon!).

### User Authentication
- **Login/Register** system using PostgreSQL.
- Secure password handling (via libsodium).

### Balance & Winnings Tracking
- Tracks user **balances** in the database.
- Calculates and updates **total winnings** after each game.
- Ensures accurate balance updates for bets, wins, and losses.

### Leaderboard
- Displays the **top players** ranked by their total winnings.
- View your current standing on the leaderboard.

---

## Setup

### 1. Clone the Repository
```bash
git clone https://github.com/roydebraj2003/CLI-Blackjack.git
cd CLI-Blackjack
```

### 2. Set Up PostgreSQL Database

**Install PostgreSQL** (if not already installed):
```bash
sudo apt update && sudo apt install postgresql postgresql-contrib
```

**Start PostgreSQL Service**:
```bash
sudo service postgresql start
```

**Access PostgreSQL Shell**:
```bash
sudo -u postgres psql
```

**Create the Database and Tables**:
```sql
-- Create a new database for the Blackjack app
CREATE DATABASE blackjack;

-- Create a new user with a password (replace 'your_password' with your desired password)
CREATE USER admin WITH PASSWORD 'your_password';

-- Grant privileges to the user on the blackjack database
GRANT ALL PRIVILEGES ON DATABASE blackjack TO admin;

-- Connect to the blackjack database
\c blackjack

-- Create the Players table
CREATE TABLE Players (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password TEXT NOT NULL,
    balance INTEGER DEFAULT 0,
    total_winnings INTEGER DEFAULT 0
);
```

**Set Up the .env File**
Create a `.env` file in the root directory with the following content:
```plaintext
DB_URI="dbname=blackjack user=admin password=your_password host=localhost port=5432"
```
_Note: Replace the above values with your actual database credentials._

### 3. Build and Run the Application

**Compile the program using GCC**:
```bash
gcc -o blackjack main.c db.c -lpq -lsodium
```

**Run the application**:
```bash
./blackjack
```

---

## Usage

Once the app is running:

- **Login or Register**: Enter your username and password.
- **Start Playing**: Place your bet and choose to Hit, Stand, or Double Down.
- **Track Your Balance**: Your balance updates after each round.
- **View the Leaderboard**: See the top players ranked by their winnings.

---

## Future Enhancements

- Implement the **Split** feature.
- Add **multiplayer support**.
- Introduce a **bet history log**.

---

## Security

- Passwords are securely handled with **libsodium**.
- Database credentials are stored in the `.env` file — please **DO NOT** share this file.

---

## Contributions

Feel free to open issues or create pull requests if you want to contribute. Let’s make this the most realistic CLI Blackjack ever!

---

Enjoy the game, and may the odds be ever in your favor! 🃏
