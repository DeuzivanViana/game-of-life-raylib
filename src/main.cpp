#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

extern "C"
{
#include <raylib.h>
}

const int
	WINDOW_WIDTH {720 * 2},
	WINDOW_HEIGHT {480 * 2},
	UNIVERSE_SIZE {80},
	ATOM_SIZE {16};

double tick = 0.;

struct Atom
{
	float temperature;
	std::vector<std::uint8_t> energy_levels;
};

Atom hydrogen {
	0.f,
	{1}
};

struct Universe
{
	std::vector<std::vector<Atom*>> atoms;
};

void createUniverse(Universe &universe) noexcept
{
	universe.atoms.clear();
	for(int y = 0; y < UNIVERSE_SIZE; y++)
	{
		std::vector<Atom*> atoms;
		for(int x = 0; x < UNIVERSE_SIZE; x++)
		{
			atoms.push_back(nullptr);
		}
		universe.atoms.push_back(atoms);
	}
}


void drawGrid(int size, int space) noexcept
{
	for(int i = 0; i < size; i++)
	{
		Vector2
			start {static_cast<float>(i * space), 0.f},
			end {static_cast<float>(i * space), static_cast<float>(space * size)};

		DrawLineV(start, end, GRAY);
	
		std::swap(start.x, start.y);
		std::swap(end.x, end.y);

		DrawLineV(start, end, GRAY);
	}
}

void drawUniverse(Universe &universe) noexcept
{
	for(int y = 0; y < UNIVERSE_SIZE; y++)
	{
		for(int x = 0; x < UNIVERSE_SIZE; x++)
		{
			if(universe.atoms[y][x] != nullptr)
				DrawRectangle(ATOM_SIZE * x, ATOM_SIZE * y, ATOM_SIZE, ATOM_SIZE, WHITE);
		}
	}
	drawGrid(UNIVERSE_SIZE, ATOM_SIZE);
}

void addAtom(Universe &universe, int x, int y, Atom atom)
{
	if(x >= 0 && x < UNIVERSE_SIZE && y >= 0 && y < UNIVERSE_SIZE)
		universe.atoms[y][x] = &atom;
}

void removeAtom(Universe &universe, int x, int y)
{
	universe.atoms[y][x] = nullptr;
}

Atom *atNeighbor(Universe &universe, int y, int x)
{
	return (x >= 0 && x < UNIVERSE_SIZE - 1 && y >= 0 && y < UNIVERSE_SIZE - 1) ? universe.atoms[y][x] : nullptr;
}

std::vector<Atom*> getNeighbors(Universe universe, int x, int y) noexcept
{
	std::vector<Atom*> atoms = {
		atNeighbor(universe, y + 1, x - 1),
		atNeighbor(universe, y + 1, x),
		atNeighbor(universe, y + 1, x + 1),
		atNeighbor(universe, y, x - 1),
		atNeighbor(universe, y, x + 1),
		atNeighbor(universe, y - 1, x -1),
		atNeighbor(universe, y - 1, x),
		atNeighbor(universe, y - 1, x + 1)
	};

	return atoms;
}

void updateUniverse(Universe &universe) noexcept
{
	Universe new_universe = universe;
	
	if(GetTime() > tick + 0.05f)
	{
		for(int y = 0; y < UNIVERSE_SIZE; y++)
		{
			for(int x = 0; x < UNIVERSE_SIZE; x++)
			{
				std::vector<std::vector<Atom*>> *atoms = &universe.atoms;
				std::vector<Atom*> neighbors = getNeighbors(universe, x, y);
				
				int count = std::count_if(neighbors.begin(), neighbors.end(), [&](const Atom *elmenet) {
					return elmenet != nullptr;
				});

				if(count == 0 && atoms->at(y)[x] == nullptr)
				{
					addAtom(new_universe, x, y, hydrogen);	
				}
				if(count == 3)
				{
					addAtom(new_universe, x, y, hydrogen);
				}
				else if(count > 3 || count < 2)
				{	
					removeAtom(new_universe, x, y);
				}
			}
		}

		tick = GetTime();
	}

	universe = new_universe;
}

int main(int, char*[])
{
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Atom Life");
	tick = GetTime();

	Universe universe;
	createUniverse(universe);

	Vector2 mouse = {0, 0};
	bool can_update = true;

	while(!WindowShouldClose())
	{
		mouse = GetMousePosition();
		BeginDrawing();
		ClearBackground(BLACK);
		
		if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			int x = mouse.x / 16;
			int y = mouse.y / 16;

			addAtom(universe, x, y, hydrogen);

			can_update = false;
		}
		else if(IsKeyDown(KEY_SPACE))
		{
			can_update = false;
		}
		else if(IsMouseButtonUp(MOUSE_BUTTON_LEFT) || IsKeyUp(KEY_SPACE))
		{
			can_update = true;
		}

		if(IsKeyPressed(KEY_C))
		{
			createUniverse(universe);
		}
		if(can_update) updateUniverse(universe);

		drawUniverse(universe);
		DrawFPS(16, 16);

		EndDrawing();
	}

	CloseWindow();

	return EXIT_SUCCESS;
}
