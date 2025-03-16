# CLI Blackjack

Welcome to **CLI Blackjack** — a command-line implementation of the classic casino game **Blackjack**! This application uses a PostgreSQL database to manage user authentication, track balances, and maintain leaderboards.

---

## Features

### Realistic Blackjack Gameplay
- Uses **6 decks of cards** (312 cards total).
- Properly shuffles all decks before each game session.
- Draws cards from the **top of the deck** — just like in a real casino.
- Implements core Blackjack rules: hit, stand, double down, split.

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

### Hosted Database Support
- The game connects to a **hosted PostgreSQL database**, eliminating the need for local database setup.
- Users must provide their own `.env` file with database credentials to run the application.

---

## Future Enhancements

- Implement **Win-streaks** feature.
- Add **multiplayer support**.
- Introduce a **bet history log**.

---

## Security

- Passwords are securely handled with **libsodium**.
- Database credentials are stored in the `.env` file — please **DO NOT** share this file.
- A `.env.example` file is provided to help users configure their own database connection.

---

## Contributions

Feel free to open issues or create pull requests if you want to contribute. Let’s make this the most realistic CLI Blackjack ever!

---

Enjoy the game, and may the odds be ever in your favor! 🃏

