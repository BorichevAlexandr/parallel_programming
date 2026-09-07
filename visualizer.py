import tkinter as tk
from tkinter import messagebox
import sys
import os

# ====================== Настройки ======================
GRID_SIZE = 15          # размер видимого поля
CELL_SIZE = 30          # размер одной клетки в пикселях
PATTERN_FILE = "pattern.txt"
RESULT_FILE = "result_pattern.txt"
# =======================================================

class GameOfLifeEditor:
    def __init__(self, mode="edit"):
        self.mode = mode  # "edit" или "view"
        self.grid = [[0 for _ in range(GRID_SIZE)] for _ in range(GRID_SIZE)]
        
        self.root = tk.Tk()
        self.root.title("Game of Life — Редактор паттерна" if mode == "edit" else "Game of Life — Результат")
        
        # Заголовок
        title_text = "Нарисуй начальное состояние (ЛКМ — живая, ПКМ — мёртвая)" if mode == "edit" else "Состояние после одного шага"
        title = tk.Label(self.root, text=title_text, font=("Arial", 12, "bold"))
        title.pack(pady=8)

        # Холст
        canvas_size = GRID_SIZE * CELL_SIZE
        self.canvas = tk.Canvas(self.root, width=canvas_size, height=canvas_size, bg="white")
        self.canvas.pack(padx=10, pady=5)

        # Рисуем сетку
        self.draw_grid()

        if mode == "edit":
            self.canvas.bind("<Button-1>", self.make_alive)      # ЛКМ — живая
            self.canvas.bind("<Button-3>", self.make_dead)       # ПКМ — мёртвая
            self.canvas.bind("<B1-Motion>", self.make_alive)     # можно водить мышкой
            self.canvas.bind("<B3-Motion>", self.make_dead)

            btn_frame = tk.Frame(self.root)
            btn_frame.pack(pady=10)

            tk.Button(btn_frame, text="Очистить", width=12, command=self.clear_grid).pack(side=tk.LEFT, padx=5)
            tk.Button(btn_frame, text="Готово", width=12, bg="#4CAF50", fg="white",
                      command=self.save_and_close).pack(side=tk.LEFT, padx=5)
        else:
            # Режим просмотра — загружаем результат
            self.load_result()
            tk.Button(self.root, text="Закрыть", width=15, command=self.root.destroy).pack(pady=10)

        self.root.mainloop()

    def draw_grid(self):
        self.canvas.delete("all")
        for i in range(GRID_SIZE):
            for j in range(GRID_SIZE):
                x1 = j * CELL_SIZE
                y1 = i * CELL_SIZE
                x2 = x1 + CELL_SIZE
                y2 = y1 + CELL_SIZE
                
                color = "black" if self.grid[i][j] == 1 else "white"
                self.canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline="gray")

    def make_alive(self, event):
        if self.mode != "edit":
            return
        j = event.x // CELL_SIZE
        i = event.y // CELL_SIZE
        if 0 <= i < GRID_SIZE and 0 <= j < GRID_SIZE:
            self.grid[i][j] = 1
            self.draw_grid()

    def make_dead(self, event):
        if self.mode != "edit":
            return
        j = event.x // CELL_SIZE
        i = event.y // CELL_SIZE
        if 0 <= i < GRID_SIZE and 0 <= j < GRID_SIZE:
            self.grid[i][j] = 0
            self.draw_grid()

    def clear_grid(self):
        self.grid = [[0 for _ in range(GRID_SIZE)] for _ in range(GRID_SIZE)]
        self.draw_grid()

    def save_and_close(self):
        with open(PATTERN_FILE, "w") as f:
            f.write(f"{GRID_SIZE}\n")
            for i in range(GRID_SIZE):
                for j in range(GRID_SIZE):
                    if self.grid[i][j] == 1:
                        f.write(f"{i} {j}\n")
        print(f"Паттерн сохранён в {PATTERN_FILE}")
        self.root.destroy()

    def load_result(self):
        if not os.path.exists(RESULT_FILE):
            messagebox.showerror("Ошибка", f"Файл {RESULT_FILE} не найден!")
            return
        
        with open(RESULT_FILE, "r") as f:
            lines = f.readlines()
        
        size = int(lines[0].strip())
        self.grid = [[0 for _ in range(size)] for _ in range(size)]
        
        for line in lines[1:]:
            if line.strip():
                i, j = map(int, line.strip().split())
                if 0 <= i < size and 0 <= j < size:
                    self.grid[i][j] = 1
        
        self.draw_grid()


if __name__ == "__main__":
    # Режим передаём аргументом командной строки
    # py visualizer.py edit   — редактор
    # py visualizer.py view   — просмотр результата
    mode = "edit"
    if len(sys.argv) > 1:
        mode = sys.argv[1]
    
    GameOfLifeEditor(mode)