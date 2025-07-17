#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

#include <raylib.h>
#include <raymath.h>

enum CellType
{
	NONE,
	NORMAL,
	VIRUS
};

typedef std::vector<std::vector<CellType>> CellMatrix;

class Grid
{
public:
	Grid(const std::uint32_t matrix_size = 8) noexcept
		: __position { Vector2 {0.f, 0.f} }, __cell_size { 10.f }, __matrix_size { matrix_size }
	{
		this->__cells = this->_createEmptyCellMatrix(this->__matrix_size);
	}
	virtual void draw() const noexcept
	{
		for(int y = 0, x = 0; y < this->__matrix_size; y++)
		{
			for(x = 0; x < this->__matrix_size; x++)
			{
				const CellType cell = this->__cells[y][x];

				if(cell != CellType::NONE) 
				{
					this->_drawCellAt(x, y);
				}
			}
			this->_drawLines(x, y);
		}
		this->_drawLines(0, 0, this->__matrix_size);
	}

	virtual void update() noexcept
	{
		double current_tick = GetTime();

		if(current_tick > this->__tick + 0.1)
		{
			this->_updateCells();
			this->__tick = current_tick;
		}
	}
	virtual void addCellAt(const std::uint32_t x, const std::uint32_t y) noexcept
	{
		if(x >= 0 && x < this->__matrix_size && y >= 0 && y < this->__matrix_size)
			this->__cells[y][x] = CellType::NORMAL;
	}
	virtual void setMatrixSize(const std::uint32_t size) noexcept
	{
		this->__matrix_size = size;
	}
	const std::uint32_t &getMatrixSize() const noexcept
	{
		return this->__matrix_size;
	}
	virtual void setPosition(const Vector2 position) noexcept
	{
		this->__position = position;
	}
	const Vector2 &getPosition() const noexcept
	{
		return this->__position;
	}
	virtual const std::uint32_t getNeighborCountAt(const std::uint32_t mx, const std::uint32_t my)
	{
		std::uint32_t amount = 0;
		for(int y = -1; y <= 1; y++)
		{
			for(int x = -1; x <= 1; x++)
			{
				const std::uint32_t
					rel_x = mx + x, 
					rel_y = my + y;

				if(!(mx == rel_x && my == rel_y) && (rel_x >= 0 && rel_x < this->__matrix_size && rel_y >= 0 && rel_y < this->__matrix_size))
					amount += this->getCellTypeAt(rel_x, rel_y) != CellType::NONE ? 1 : 0;
			}
		}
		return amount;
	}
	const double &getCellSize() const noexcept
	{
		return this->__cell_size;
	}
	const CellType &getCellTypeAt(const std::uint32_t x, const std::uint32_t y) const noexcept
	{
		if(x >= 0 && x < this->__matrix_size && y >= 0 && y < this->__matrix_size)
		{
			return this->__cells[y][x];
		}
		return CellType::NONE;
	}
	virtual ~Grid() noexcept
	{
	}
protected:
	virtual void _updateCells() noexcept
	{
		CellMatrix new_matrix = this->__cells;
		for(int y = 0; y < this->__matrix_size; y++)
		{
			for(int x = 0; x < this->__matrix_size; x++)
			{
				this->_processCellMatrix(new_matrix, x, y);
			}
		}
		this->__cells = new_matrix;
	}
	virtual void _processCellMatrix(CellMatrix &new_matrix, const std::uint32_t x, const std::uint32_t y) noexcept
	{
		CellType cell = this->__cells[y][x];

		const std::uint32_t amount = this->getNeighborCountAt(x, y);
		
		if(cell == CellType::NORMAL)
		{
			if(amount < 2 || amount > 3)
				new_matrix[y][x] = CellType::NONE;
			else
				new_matrix[y][x] = CellType::NORMAL;
		}
		else
		{
			if(amount == 3)
				new_matrix[y][x] = CellType::NORMAL;
			else
				new_matrix[y][x] = CellType::NONE;
		}
	}
	virtual void _drawLines(const int x, const int y, const int offset = 0) const noexcept
	{
		int  i = ((x + y) % this->__matrix_size) + offset;
		Vector2
			start {static_cast<float>(i * this->__cell_size), 0.f},
			end   {static_cast<float>(i * this->__cell_size), static_cast<float>(this->__cell_size * this->__matrix_size)};

		DrawLineV(Vector2Add(start, this->__position), Vector2Add(end, this->__position), GRAY);
		
		std::swap(start.x, start.y);
		std::swap(end.x, end.y);
		
		DrawLineV(Vector2Add(start, this->__position), Vector2Add(end, this->__position), GRAY);
	}
	virtual void _drawCellAt(const std::uint32_t x, const std::uint32_t y, Color color = WHITE) const noexcept
	{
		DrawRectangle(this->__position.x + x * this->__cell_size, this->__position.y + y * this->__cell_size, this->__cell_size, this->__cell_size, color);
	}
	CellMatrix _createEmptyCellMatrix(const std::uint32_t matrix_size) noexcept
	{
		CellMatrix matrix(matrix_size, std::vector<CellType>(matrix_size, CellType::NONE));
		return matrix;
	}
private:
	Vector2 __position;
	double __cell_size;
	std::uint32_t __matrix_size;
	CellMatrix __cells;
	double __tick;
};

int main(int, char*[])
{
	InitWindow(720, 480, "Game of Life");
	SetTargetFPS(60);
	
	Grid grid(42);
	grid.setPosition(Vector2 {
		(GetScreenWidth() - static_cast<float>(grid.getCellSize()) * static_cast<float>(grid.getMatrixSize())) / 2.f,
		(GetScreenHeight() - static_cast<float>(grid.getCellSize()) * static_cast<float>(grid.getMatrixSize())) / 2.f
	});

	bool can_update = false;

	while(!WindowShouldClose())
	{
		if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			Vector2 matrix_pos = Vector2Divide(Vector2Subtract(GetMousePosition(), grid.getPosition()), {
				grid.getCellSize(),
				grid.getCellSize()
			});
			
			grid.addCellAt(
				static_cast<std::uint32_t>(std::floor(matrix_pos.x)),
				static_cast<std::uint32_t>(std::floor(matrix_pos.y))
			);
		}
		if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		{
		}
		if(IsKeyPressed(KEY_SPACE))
		{
			can_update = !can_update;
		}
		BeginDrawing();
		ClearBackground(BLACK);

		if(can_update)
			grid.update();
		grid.draw();

		DrawFPS(8, 8);

		EndDrawing();
	}

	CloseWindow();
}
