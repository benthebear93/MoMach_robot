from tkinter import *
import numpy as np
import matplotlib.pylab as plt

class Cell():
    FILL_BG = "green"
    EMPTY_BG = "white"
    FILL_BORDER = "green"
    EMPTY_BORDER = "black"

    def __init__(self, master, x, y, size):
        self.master = master
        self.abs = x
        self.ord = y
        self.size = size
        self.fill = False

    def _switch(self):
        self.fill = not self.fill

    def draw(self):
        if self.master is not None:
            fill = Cell.FILL_BG
            outline = Cell.FILL_BORDER
            if not self.fill: #if Cell is not filled
                fill = Cell.EMPTY_BG
                outline = Cell.EMPTY_BORDER
            xmin = self.abs * self.size
            xmax = xmin + self.size
            ymin = self.ord * self.size
            ymax = ymin + self.size
            self.master.create_rectangle(xmin, ymin, xmax, ymax, fill=fill, outline=outline)

class CellGrid(Canvas):
    def __init__(self, master, row_number, column_number, cell_size, row_list, column_list, *args, **kwargs):
        Canvas.__init__(self, master, width=cell_size * column_number, height=cell_size*row_number, *args, **kwargs)
        self.cell_size = cell_size
        self.grid = []
        for row in range(row_number):
            line = []
            for column in range(column_number):
                line.append(Cell(self, column, row, cell_size))
            self.grid.append(line)
        self.draw()
        self._readCoords(row_list, column_list)

    def draw(self):
        for row in self.grid:
            for cell in row:
                cell.draw()

    def _readCoords(self, row_list, column_list):
        for i in range(len(row_list)):
            cell = self.grid[row_list[i]][column_list[i]]
            cell._switch()
            cell.draw()



if __name__ == "__main__":
    app = Tk()

    row = [0, 1, 2, 3, 4, 5, 6, 6, 6, 6]
    column = [0, 0, 0, 0, 0, 0, 1, 2, 3, 4]
    grid = CellGrid(app, 10, 10, 30, row, column)
    grid.pack()
    app.mainloop()