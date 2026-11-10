#include "raylib.h"
#include <iostream>
using namespace std;

constexpr int BOARD_SIZE = 8;
constexpr float CELL_SIZE = 70.0f;
constexpr float BOARD_X = 40.0f;
constexpr float BOARD_Y = 150.0f;
const Color LIGHT_SQUARE = { 238, 218, 181, 255 };
const Color DARK_SQUARE = { 118, 78, 52, 255 };
const Color RED_PIECE_COLOR = { 190, 45, 45, 255 };
const Color BLACK_PIECE_COLOR = { 35, 35, 35, 255 };
int selectedRow = -1;
int selectedCol = -1;
int redCaptures = 0;
int blackCaptures = 0;
bool gameOver = false;
float elapsedSeconds = 0.0f;
const Rectangle RESTART_BUTTON = {650, 470, 220, 55 };

enum class Player
{
	Red,
	Black
};
Player currentPlayer = Player::Red;
Player winner = Player::Red;
class Piece
{
private: 
	Player owner_;
	bool active_;
	bool queen_;
public: 
	Piece(): owner_(Player::Red), queen_(false), active_(false){}
	void Place(Player owner)
	{
		owner_ = owner;
		active_ = true;
		queen_ = false;
	}

	bool IsActive() const
	{
		return active_;
	}
	Player GetOwner() const
	{
		return owner_;
	}
	void Remove()
	{
		active_ = false;
		queen_ = false;
	}

	bool IsQueen() const
	{
		return queen_;
	}

	void PromoteToQueen()
	{
		queen_ = true;
	}
	void Draw(float centerX, float centerY, float radius) const
	{
		if (!active_)
		{
			return;
		}
		Color pieceColor;

		if (owner_ == Player::Red)
		{
			pieceColor = RED_PIECE_COLOR;
		}
		else
		{
			pieceColor = BLACK_PIECE_COLOR;
		}
		DrawCircleV({ centerX, centerY }, radius, pieceColor);
		DrawCircleLines(static_cast<int>(centerX), static_cast<int>(centerY), radius, LIGHTGRAY);
		if (queen_)
		{
			DrawText("Q", static_cast<int>(centerX - 8), static_cast<int>(centerY - 12), 24, GOLD);

		}
	}
};
class Field
{
private:
	int row_;
	int col_;
	Rectangle bounds_;
	bool dark_;
	Piece piece_;

public:
	Field() : row_(0), col_(0), bounds_(0, 0, 0, 0), dark_(false) {}

	void Initialize(int row, int col)
	{
		row_ = row;
		col_ = col;
		bounds_ = Rectangle{ BOARD_X + col * CELL_SIZE, BOARD_Y + row * CELL_SIZE, CELL_SIZE, CELL_SIZE };
		dark_ = (row + col) % 2 != 0;
		piece_.Remove();

	}
	int GetRow() const
	{
		return row_;
	}
	int GetCol() const
	{
		return col_;
	}
	bool IsDark() const
	{
		return dark_;
	}
	bool IsOccupied() const
	{
		return piece_.IsActive();
	}
	bool Contains(Vector2 point)const
	{
		return CheckCollisionPointRec(point, bounds_);
	}
	void PlacePiece(Player owner)
	{
		piece_.Place(owner);
	}
	void SetPiece(const Piece& piece)
	{
		piece_ = piece;
	}
	Piece& GetPiece()
	{
		return piece_;
	}
	const Piece& GetPiece() const
	{
		return piece_;
	}
	void RemovePiece()
	{
		piece_.Remove();
	}
	void Draw(bool selected) const
	{
		Color squareColor = dark_ ? DARK_SQUARE : LIGHT_SQUARE;
		DrawRectangleRec(bounds_, squareColor);
		DrawRectangleLinesEx(bounds_, 1.0f, DARKGRAY);
		if (selected)
		{
			DrawRectangleLinesEx(bounds_, 4.0f, YELLOW);
		}
		if (!piece_.IsActive())
		{
			return;
		}

		float centerX = bounds_.x + bounds_.width / 2.0f;
		float centerY = bounds_.y + bounds_.height / 2.0f;
		float radius = CELL_SIZE * 0.34f;
		piece_.Draw(centerX, centerY, radius);

	}
};
Field board[BOARD_SIZE][BOARD_SIZE];
void InitializeBoard()
{
	for (int row = 0; row < BOARD_SIZE; row++)
	{
		for (int col = 0; col < BOARD_SIZE; col++)
		{
			board[row][col].Initialize(row, col);
			if (!board[row][col].IsDark())
			{
				continue;
			}
			if (row < 3)
			{
				board[row][col].PlacePiece(Player::Black);
			}
			else if (row >= 5)
			{
				board[row][col].PlacePiece(Player::Red);
			}
		}
	}
}
void ResetGame()
{
	InitializeBoard();
	currentPlayer = Player::Red;
	selectedRow = -1;
	selectedCol = -1;
	redCaptures = 0;
	blackCaptures = 0;
	gameOver = false;
	winner = Player::Red;
	elapsedSeconds = 0.0f;
}
int CountPieces(Player player)
{
	int count = 0;
	for (int row = 0; row < BOARD_SIZE; row++)
	{
		for (int col = 0; col < BOARD_SIZE; col++)
		{
			const Field& field = board[row][col];

			if (field.IsOccupied())
			{
				if (field.GetPiece().GetOwner() == player)
				{
					count++;
				}
			}
		}
	}
	return count;
}
void DrawFields()
{
	for (int row = 0; row < BOARD_SIZE; row++)
	{
		for (int col = 0; col < BOARD_SIZE; col++)
		{
			board[row][col].Draw(row == selectedRow && col == selectedCol);
		}
	}
	Rectangle outline = {
		BOARD_X,
		BOARD_Y,
		BOARD_SIZE * CELL_SIZE,
		BOARD_SIZE * CELL_SIZE
	};
	DrawRectangleLinesEx(outline, 3.0f, DARKGRAY);
}
void DrawRestartButton()
{
	Vector2 mousePosition = GetMousePosition();
	bool hovering = CheckCollisionPointRec(mousePosition, RESTART_BUTTON);
	Color buttonColor = hovering ? DARKGREEN : GREEN;
	DrawRectangleRec(RESTART_BUTTON, buttonColor);
	DrawRectangleLinesEx(RESTART_BUTTON, 2.0f, DARKGRAY);
	const char* buttonText = "Restart Game";
	int textWidth = MeasureText(buttonText, 20);
	int textX = static_cast<int> (RESTART_BUTTON.x + (RESTART_BUTTON.width - textWidth) / 2);
	int textY = static_cast<int> (RESTART_BUTTON.y + (RESTART_BUTTON.height - 20) / 2);
	DrawText("Restart Game", textX, textY, 20, WHITE);
}
Player OtherPlayer(Player player)
{
	if (player == Player::Red)
	{
		return Player::Black;
	}
	return Player::Red;
}
void HandleMouseInput()
{
	if (gameOver)
	{
		return;
	}
	if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		return;
	}
	Vector2 mousePosition = GetMousePosition();
	for (int row = 0; row < BOARD_SIZE; row++)
	{
		for (int col = 0; col < BOARD_SIZE; col++)
		{
			Field& clickedField = board[row][col];
			if (!clickedField.Contains(mousePosition))
			{
				continue;
			}

			if (selectedRow == -1 || selectedCol == -1)
			{
				if (clickedField.IsOccupied() && clickedField.GetPiece().GetOwner() == currentPlayer) {
					selectedRow = row;
					selectedCol = col;
				}
				return;
			}
			Field& selectedField = board[selectedRow][selectedCol];
			if (row == selectedRow && col == selectedCol) {
				selectedRow = -1;
				selectedCol = -1;
				return;
			}
			if (clickedField.IsOccupied() && clickedField.GetPiece().GetOwner() == currentPlayer)
			{
				selectedRow = row;
				selectedCol = col;
				return;
			}
			bool isCapture = clickedField.IsOccupied();
			Piece movingPiece = selectedField.GetPiece();
			selectedField.RemovePiece();
			clickedField.SetPiece(movingPiece);
			bool reachedOppositeEnd = (currentPlayer == Player::Red && row == 0) || (currentPlayer == Player::Black && row == BOARD_SIZE - 1);
			if (reachedOppositeEnd)
			{
				clickedField.GetPiece().PromoteToQueen();
			}

			if (isCapture)
			{
				if (currentPlayer == Player::Red)
				{
					redCaptures++;
				}
				else
				{
					blackCaptures++;
				}
			}
			selectedRow = -1;
			selectedCol = -1;
			Player opponent = OtherPlayer(currentPlayer);
			if (CountPieces(opponent) == 0)
			{
				gameOver = true;
				winner = currentPlayer;
				return;
			}
			currentPlayer = OtherPlayer(currentPlayer);
			
		}
	}
}
void HandleRestartInput()
{
	if(IsKeyPressed(KEY_R))
	{
		ResetGame();
		return;
	}
	if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		return;
	}
	Vector2 mousePosition = GetMousePosition();	
	if (CheckCollisionPointRec(mousePosition, RESTART_BUTTON))
	{
		ResetGame();
	}

}



int main() {

	InitWindow(1000, 760, "The Dame Game Simulator");
	SetTargetFPS(60);
	InitializeBoard();
	TraceLog(LOG_INFO, "Red pieces: %d", CountPieces(Player::Red));
	TraceLog(LOG_INFO, "Black pieces: %d", CountPieces(Player::Black));
	while (!WindowShouldClose()) {
		HandleRestartInput();
		HandleMouseInput();
		if (!gameOver)
		{
			elapsedSeconds += GetFrameTime();
		}
		int totalSeconds = static_cast<int> (elapsedSeconds);
		int minutes = totalSeconds / 60;
		int seconds = totalSeconds % 60;
		DrawText(TextFormat("Time: %02d:%02d", minutes, seconds), 650, 410, 20, DARKGRAY);
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText("The Dame Game Simulator", 40, 30, 32, DARKGRAY);
		DrawText("Red VS Black", 40, 80, 20, GRAY);
		if (gameOver)
		{
			DrawText("GAME OVER", 650, 150, 30, RED);
			DrawText(TextFormat("%s WINS!", winner == Player::Red ? "RED" : "BLACK"), 650, 185, 26, DARKGREEN);
		}
		else
		{
			DrawText(TextFormat("Current turn: %s", currentPlayer == Player::Red ? "RED" : "BLACK"), 650, 150, 24, RED);
		}
		DrawText(TextFormat("Red captures: %i", redCaptures), 650, 210, 20, RED_PIECE_COLOR);
		DrawText(TextFormat("Black captures: %i", blackCaptures), 650, 250, 20, BLACK_PIECE_COLOR);
		DrawText(TextFormat("Red remaining: %d", CountPieces(Player::Red)), 650, 310, 18, RED_PIECE_COLOR);
		DrawText(TextFormat("Black remaining: %d", CountPieces(Player::Black)), 650, 345, 18, BLACK_PIECE_COLOR);
		DrawFields();	
		
		EndDrawing();
	}
	CloseWindow();
	return 0;
}